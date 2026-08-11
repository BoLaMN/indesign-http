//========================================================================================
//
//  HttpLinkServerAPIWrapper.cpp
//
//  IHTTPLinkResourceServerAPIWrapper against any ordinary web server, rather
//  than the sample's bespoke JSON API on a fixed host.
//
//  Status comes from HTTP validators: a ranged GET (Range: bytes=0-0) gives us
//  ETag / Last-Modified / Content-Range, so we detect changes without pulling
//  the asset. The ETag becomes the resource's unique ID, which the base state
//  updater hashes into the stamp behind the Links panel's out-of-date badge.
//
//  No batch endpoint (plain HTTP has none) and auth is optional.
//
//========================================================================================

#include "VCPlugInHeaders.h"

// Definitions
#include "HttpLinkID.h"

// Interfaces
#include "CPMUnknown.h"
#include "IHTTPLinkResourceServerAPIWrapper.h"
#include "IHTTPServerInfo.h"

// Boost
#include <boost/bind.hpp>
#include "boost/shared_ptr.hpp"

// Utils
#include "IDHTTPWrapper.h"
#include "IJsonUtils.h"
#include "StringUtils.h"

#include <cstdlib>

// The framework calls the sync probes on the main thread, so this bounds how long
// InDesign can hang on one unresponsive origin. Async revalidation isn't capped
// by it.
const int kStatusRequestTimeout_InMS = 4 * 1000;

// Header names we read off a response. Compared case-insensitively, since servers
// vary, and HTTP header names are case-insensitive per RFC 9110.
static const char* kHDR_ETAG           = "etag";
static const char* kHDR_LASTMODIFIED   = "last-modified";
static const char* kHDR_CONTENTLENGTH  = "content-length";
static const char* kHDR_CONTENTRANGE   = "content-range";

// Headers we send.
static const HTTPLinkSubsystemTypes::HeaderKey kHDR_AUTHORIZATION(L"authorization");
static const HTTPLinkSubsystemTypes::HeaderKey kHDR_RANGE(L"range");
static const HTTPLinkSubsystemTypes::HeaderValue kRANGE_FIRSTBYTE(L"bytes=0-0");

//========================================================================================
// class HttpLnkServerAPIWrapper
//========================================================================================
/** Talks plain HTTP/HTTPS to an arbitrary origin on behalf of the links subsystem.

    @ingroup httplink.sdk
*/
class HttpLnkServerAPIWrapper : public CPMUnknown<IHTTPLinkResourceServerAPIWrapper>
{
public:
    typedef CPMUnknown<IHTTPLinkResourceServerAPIWrapper> inherited;

    HttpLnkServerAPIWrapper(IPMUnknown* boss);
    ~HttpLnkServerAPIWrapper();

    virtual AssetStatusWithMetadataPtr RequestAssetStatusWithMetadata(IHTTPLinkResourceConnection* connection,
                                                                     const URI& assetURI);

    virtual bool16 AsyncRequestAssetStatusWithMetadata(IHTTPLinkResourceConnection* connection,
                                                       const URI& assetURI,
                                                       RequestAssetStatusCompletionFunction completionFunction);

    virtual bool16 AsyncRequestBatchAssetStatusWithMetadata(IHTTPLinkResourceConnection* connection,
                                                            const URIVector& assetURIVector,
                                                            RequestBatchAssetStatusCompletionFunction completionFunction);

    virtual bool16 GenerateSessionToken(IHTTPLinkResourceConnection* connection, std::string& token);

    /** Plain HTTP has no batch status endpoint, so there are no batch limits. */
    virtual bool16 GetConfiguredBatchLimits(IHTTPLinkResourceConnection* connection,
                                            int32& batchLimit, long& maxTimeinMs, long& minTimeinMs)
    {
        return kFalse;
    }

    /** ...and therefore batch updates are never supported. */
    virtual bool16 IsBatchUpdateSupported(const URI& uri) const
    {
        return kFalse;
    }

    virtual bool16 IsServerAccessible(IHTTPLinkResourceConnection* connection, bool16& serverAccessible);

    virtual bool16 FetchAssetPathAndRequestHeadersForDownload(IHTTPLinkResourceConnection* connection,
                                                              const URI& inURI,
                                                              URI& assetURL,
                                                              HTTPLinkSubsystemTypes::Headers& headers) const;

    /** No FPO/proxy renditions: a generic web server only has the one asset. */
    virtual URI GetAssetURI(const URI& inURI, ILinkResourceRenditionData::ResourceRenditionType type) const
    {
        return inURI;
    }

    virtual ILinkResourceRenditionData::ResourceRenditionType GetURIRenditionType(const URI& inURI) const
    {
        return ILinkResourceRenditionData::kOriginal;
    }

    /** Text-fragment links are an internal feature; not supported here. */
    virtual AssetMetadataContentPtr RequestAssetTextFragmentContent(IHTTPLinkResourceConnection* connection,
                                                                    const URI& inURI,
                                                                    ErrorCode err = kCantPlaceMetadataError)
    {
        return nil;
    }

    virtual bool16 GetHTTPBatchRequestObjectType(const URI& uri,
                                                 HTTPLinkSubsystemTypes::BatchRequestObjectType& objType) const
    {
        objType = HTTPLinkSubsystemTypes::kAsset;
        return kTrue;
    }

    virtual bool16 AdjustURI(const URI& inUri, URI& outUri)
    {
        return kFalse;
    }

    /** Maps one of our private schemes onto the real wire scheme.
        idhttps -> https, anything else -> http. Authority, path, query and
        fragment are carried across untouched, so any host/port works.
    */
    static URI ToWireURI(const URI& inURI);

private:
    // Header/response helpers -----------------------------------------------
    static bool16 FindHeader(const HTTPLinkSubsystemTypes::Headers& headers,
                             const char* lowercaseName,
                             std::string& outValue);

    /** Builds AssetMetadata from a response's headers. */
    static AssetMetadataPtr MetadataFromResponse(const URI& assetURI,
                                                 const IDHTTPWrapper::Response& response);

    /** Maps an HTTP response onto an AssetStatusWithMetadata. */
    static AssetStatusWithMetadataPtr StatusFromResponse(const URI& assetURI,
                                                         const IDHTTPWrapper::Response& response);

    /** Collects the request headers for a connection (auth if configured). */
    bool16 GetRequestHeaders(HTTPLinkSubsystemTypes::Headers& headers,
                             IHTTPLinkResourceConnection* connection) const;

    // Worker/main-thread callbacks ------------------------------------------
    static void ProbeCallbackOnWorkerThread(const bool isAsyncRequest,
                                            RequestAssetStatusCompletionFunction completionFunction,
                                            URI assetURI,
                                            IDHTTPWrapper::RequestID inRequestID,
                                            IDHTTPWrapper::Response inResponse);

    static void ProbeCallbackOnMainThread(RequestAssetStatusCompletionFunction completionFunction,
                                          IDHTTPWrapper::RequestID inRequestID,
                                          AssetStatusWithMetadataPtr statusPtr);

    static void ServerAccessibleCallbackOnWorkerThread(IDHTTPWrapper::RequestID inRequestID,
                                                       IDHTTPWrapper::Response inResponse);

    /** Fans a batch request out into individual probes, re-assembling the
        results into one vector before invoking the batch completion function. */
    struct BatchCollector
    {
        AssetStatusWithMetadataPtrVectorPtr fResults;
        size_t                              fRemaining;
        RequestBatchAssetStatusCompletionFunction fCompletion;
    };
    typedef boost::shared_ptr<BatchCollector> BatchCollectorPtr;

    static void BatchItemComplete(BatchCollectorPtr collector,
                                  size_t index,
                                  AssetStatusWithMetadataPtr statusPtr);

    // Synchronous request rendezvous
    static AssetStatusWithMetadataPtr  ts_SyncStatus;
    static bool16                      ts_SyncServerAccessible;
    static bool16                      ts_SyncServerAccessibleValid;
};

CREATE_PMINTERFACE(HttpLnkServerAPIWrapper, kHttpLnkResourceServerAPIWrapperImpl)

IHTTPLinkResourceServerAPIWrapper::AssetStatusWithMetadataPtr HttpLnkServerAPIWrapper::ts_SyncStatus = nil;
bool16                                                 HttpLnkServerAPIWrapper::ts_SyncServerAccessible = kFalse;
bool16                                                 HttpLnkServerAPIWrapper::ts_SyncServerAccessibleValid = kFalse;

//========================================================================================
HttpLnkServerAPIWrapper::HttpLnkServerAPIWrapper(IPMUnknown* boss)
: inherited(boss)
{
}

HttpLnkServerAPIWrapper::~HttpLnkServerAPIWrapper()
{
    ts_SyncStatus = nil;
}

//========================================================================================
// HttpLnkServerAPIWrapper::ToWireURI
//========================================================================================
URI HttpLnkServerAPIWrapper::ToWireURI(const URI& inURI)
{
    URI wire(inURI);
    const WideString scheme(inURI.GetComponent(URI::kScheme));

    if (scheme == WideString(kHttpLnkSchemeSecure))
        wire.SetComponent(URI::kScheme, kHTTPSAssetURIScheme);
    else
        wire.SetComponent(URI::kScheme, kHTTPAssetURIScheme);

    return wire;
}

//========================================================================================
// HttpLnkServerAPIWrapper::FindHeader
//========================================================================================
bool16 HttpLnkServerAPIWrapper::FindHeader(const HTTPLinkSubsystemTypes::Headers& headers,
                                           const char* lowercaseName,
                                           std::string& outValue)
{
    for (HTTPLinkSubsystemTypes::HeadersConstIterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::string key;
        StringUtils::ConvertWideStringToUTF8(it->first, key);

        // HTTP header names are case-insensitive; normalise before comparing.
        for (std::string::iterator c = key.begin(); c != key.end(); ++c)
            *c = static_cast<char>(tolower(static_cast<unsigned char>(*c)));

        if (key == lowercaseName)
        {
            StringUtils::ConvertWideStringToUTF8(it->second, outValue);
            return kTrue;
        }
    }
    return kFalse;
}

//========================================================================================
// HttpLnkServerAPIWrapper::MetadataFromResponse
//========================================================================================
IHTTPLinkResourceServerAPIWrapper::AssetMetadataPtr
HttpLnkServerAPIWrapper::MetadataFromResponse(const URI& assetURI, const IDHTTPWrapper::Response& response)
{
    AssetMetadataPtr metadata(new AssetMetadata());
    metadata->fSize = 0;

    const HTTPLinkSubsystemTypes::Headers headers = response.GetHeaders();

    // Prefer a strong validator. ETag is the best change signal a plain web
    // server offers; Last-Modified is the fallback. One of these must vary when
    // the asset changes, or InDesign can never report the link as out of date.
    std::string etag;
    std::string lastModified;
    FindHeader(headers, kHDR_ETAG, etag);
    FindHeader(headers, kHDR_LASTMODIFIED, lastModified);

    metadata->fUniqueID    = !etag.empty() ? etag : lastModified;
    metadata->fVersionData = lastModified;

    // Size: with a satisfied range request Content-Length is 1, so the real
    // length lives in Content-Range ("bytes 0-0/12345"). Fall back to
    // Content-Length when the server ignored the range.
    std::string contentRange;
    if (FindHeader(headers, kHDR_CONTENTRANGE, contentRange))
    {
        const std::string::size_type slash = contentRange.rfind('/');
        if (slash != std::string::npos && slash + 1 < contentRange.length())
        {
            const std::string total = contentRange.substr(slash + 1);
            if (total != "*")
                metadata->fSize = static_cast<uint64>(::strtoull(total.c_str(), nil, 10));
        }
    }
    else
    {
        std::string contentLength;
        if (FindHeader(headers, kHDR_CONTENTLENGTH, contentLength))
            metadata->fSize = static_cast<uint64>(::strtoull(contentLength.c_str(), nil, 10));
    }

    std::string path;
    StringUtils::ConvertWideStringToUTF8(assetURI.GetComponent(URI::kPath), path);
    metadata->fPath = path;

    return metadata;
}

//========================================================================================
// HttpLnkServerAPIWrapper::StatusFromResponse
//========================================================================================
IHTTPLinkResourceServerAPIWrapper::AssetStatusWithMetadataPtr
HttpLnkServerAPIWrapper::StatusFromResponse(const URI& assetURI, const IDHTTPWrapper::Response& response)
{
    AssetStatusWithMetadataPtr statusPtr(new AssetStatusWithMetadata());

    // A network failure is emphatically NOT "missing" -- a dropped VPN must not
    // mark every link in the document as broken. kInaccessible leaves the link
    // alone and lets a later probe recover.
    if (response.IsNetworkConnectivityError())
    {
        statusPtr->fAssetStatus = AssetStatusWithMetadata::kInaccessible;
        return statusPtr;
    }

    const int32 status = response.GetStatus();

    if (status == 200 || status == 206)
    {
        statusPtr->fAssetStatus     = AssetStatusWithMetadata::kAvailable;
        statusPtr->fAssetMetadataPtr = MetadataFromResponse(assetURI, response);
    }
    else if (status == 404 || status == 410)
    {
        // The server answered and said the asset is gone. That is a real miss.
        statusPtr->fAssetStatus = AssetStatusWithMetadata::kMissing;
    }
    else
    {
        // 401/403/5xx and anything else: reachable but we cannot vouch for it.
        statusPtr->fAssetStatus = AssetStatusWithMetadata::kInaccessible;
    }

    return statusPtr;
}

//========================================================================================
// HttpLnkServerAPIWrapper::GetRequestHeaders
//========================================================================================
bool16 HttpLnkServerAPIWrapper::GetRequestHeaders(HTTPLinkSubsystemTypes::Headers& headers,
                                                  IHTTPLinkResourceConnection* connection) const
{
    // Anonymous access is legitimate here: most public URLs need no credentials
    // at all. Only attach an Authorization header when a token was configured.
    if (connection == nil)
        return kTrue;

    std::stringstream loginCredentials;
    if (!connection->GetLoginCredentials(loginCredentials))
        return kTrue;

    std::string token;
    try
    {
        JSON loginJson;
        loginJson.read_json(loginCredentials);
        if (loginJson.checkKey(token_key))
            token = loginJson.GetString(token_key);
    }
    catch (...)
    {
        // Malformed credentials are not fatal; fall through to anonymous.
        token.clear();
    }

    if (!token.empty())
    {
        HTTPLinkSubsystemTypes::HeaderValue authValue;
        StringUtils::ConvertUTF8ToWideString(token, authValue);
        headers.insert(std::make_pair(kHDR_AUTHORIZATION, authValue));
    }

    return kTrue;
}

//========================================================================================
// HttpLnkServerAPIWrapper::RequestAssetStatusWithMetadata  (synchronous)
//========================================================================================
IHTTPLinkResourceServerAPIWrapper::AssetStatusWithMetadataPtr
HttpLnkServerAPIWrapper::RequestAssetStatusWithMetadata(IHTTPLinkResourceConnection* connection, const URI& assetURI)
{
    HTTPLinkSubsystemTypes::Headers headers;
    GetRequestHeaders(headers, connection);
    headers.insert(std::make_pair(kHDR_RANGE, kRANGE_FIRSTBYTE));

    const URI wireURI = ToWireURI(assetURI);
    const IDHTTPWrapper::URL requestURL = wireURI.GetURIW();

    ts_SyncStatus = nil;

    IDHTTPWrapper::CompletionFunction completion =
        boost::bind<void>(&HttpLnkServerAPIWrapper::ProbeCallbackOnWorkerThread,
                          false /*isAsyncRequest*/,
                          RequestAssetStatusCompletionFunction(),
                          assetURI,
                          _1,
                          _2);

    IDHTTPWrapper::HTTP http;
    if (!http.GET(completion, requestURL, kStatusRequestTimeout_InMS, headers))
        return nil;

    AssetStatusWithMetadataPtr result = ts_SyncStatus;
    ts_SyncStatus = nil;
    return result;
}

//========================================================================================
// HttpLnkServerAPIWrapper::AsyncRequestAssetStatusWithMetadata
//========================================================================================
bool16 HttpLnkServerAPIWrapper::AsyncRequestAssetStatusWithMetadata(IHTTPLinkResourceConnection* connection,
                                                                    const URI& assetURI,
                                                                    RequestAssetStatusCompletionFunction completionFunction)
{
    HTTPLinkSubsystemTypes::Headers headers;
    GetRequestHeaders(headers, connection);
    headers.insert(std::make_pair(kHDR_RANGE, kRANGE_FIRSTBYTE));

    const URI wireURI = ToWireURI(assetURI);
    const IDHTTPWrapper::URL requestURL = wireURI.GetURIW();

    IDHTTPWrapper::CompletionFunction completion =
        boost::bind<void>(&HttpLnkServerAPIWrapper::ProbeCallbackOnWorkerThread,
                          true /*isAsyncRequest*/,
                          completionFunction,
                          assetURI,
                          _1,
                          _2);

    IDHTTPWrapper::HTTP http;
    return http.GET(completion, requestURL, 0 /*non-blocking*/, headers);
}

//========================================================================================
// HttpLnkServerAPIWrapper::AsyncRequestBatchAssetStatusWithMetadata
//
// There is no batch endpoint on a plain web server, so we fan out into
// individual probes and re-assemble. The links subsystem only calls this when
// IsBatchUpdateSupported() is true, which it is not for us, but the method
// is pure virtual, so it gets a correct implementation rather than a stub.
//========================================================================================
bool16 HttpLnkServerAPIWrapper::AsyncRequestBatchAssetStatusWithMetadata(IHTTPLinkResourceConnection* connection,
                                                                         const URIVector& assetURIVector,
                                                                         RequestBatchAssetStatusCompletionFunction completionFunction)
{
    if (assetURIVector.empty())
        return kFalse;

    BatchCollectorPtr collector(new BatchCollector());
    collector->fResults.reset(new AssetStatusWithMetadataPtrVector(assetURIVector.size()));
    collector->fRemaining  = assetURIVector.size();
    collector->fCompletion = completionFunction;

    bool16 allQueued = kTrue;
    for (size_t i = 0; i < assetURIVector.size(); ++i)
    {
        RequestAssetStatusCompletionFunction itemCompletion =
            boost::bind<void>(&HttpLnkServerAPIWrapper::BatchItemComplete, collector, i, _1);

        if (!AsyncRequestAssetStatusWithMetadata(connection, assetURIVector[i], itemCompletion))
        {
            // Could not even queue this one; record it as inaccessible so the
            // collector still completes rather than hanging forever.
            AssetStatusWithMetadataPtr failed(new AssetStatusWithMetadata());
            failed->fAssetStatus = AssetStatusWithMetadata::kInaccessible;
            BatchItemComplete(collector, i, failed);
            allQueued = kFalse;
        }
    }

    return allQueued;
}

//========================================================================================
// HttpLnkServerAPIWrapper::BatchItemComplete
//========================================================================================
void HttpLnkServerAPIWrapper::BatchItemComplete(BatchCollectorPtr collector,
                                                size_t index,
                                                AssetStatusWithMetadataPtr statusPtr)
{
    if (collector == nil || collector->fResults == nil)
        return;

    if (index < collector->fResults->size())
        (*collector->fResults)[index] = statusPtr;

    if (collector->fRemaining > 0)
        --collector->fRemaining;

    if (collector->fRemaining == 0 && collector->fCompletion)
        collector->fCompletion(collector->fResults);
}

//========================================================================================
// HttpLnkServerAPIWrapper::ProbeCallbackOnWorkerThread
//========================================================================================
void HttpLnkServerAPIWrapper::ProbeCallbackOnWorkerThread(const bool isAsyncRequest,
                                                          RequestAssetStatusCompletionFunction completionFunction,
                                                          URI assetURI,
                                                          IDHTTPWrapper::RequestID inRequestID,
                                                          IDHTTPWrapper::Response inResponse)
{
    AssetStatusWithMetadataPtr statusPtr = StatusFromResponse(assetURI, inResponse);

    if (isAsyncRequest)
    {
        // Hop back to the main thread: the InDesign object model is not
        // thread-safe and the completion function will touch link resources.
        IDHTTPWrapper::HTTP http;
        http.AsyncCallFromMainThread(boost::bind(&HttpLnkServerAPIWrapper::ProbeCallbackOnMainThread,
                                                 completionFunction,
                                                 inRequestID,
                                                 statusPtr));
    }
    else
    {
        ts_SyncStatus = statusPtr;
    }
}

//========================================================================================
// HttpLnkServerAPIWrapper::ProbeCallbackOnMainThread
//========================================================================================
void HttpLnkServerAPIWrapper::ProbeCallbackOnMainThread(RequestAssetStatusCompletionFunction completionFunction,
                                                        IDHTTPWrapper::RequestID inRequestID,
                                                        AssetStatusWithMetadataPtr statusPtr)
{
    if (completionFunction)
        completionFunction(statusPtr);

    inRequestID.DeleteRequest();
}

//========================================================================================
// HttpLnkServerAPIWrapper::GenerateSessionToken
//========================================================================================
bool16 HttpLnkServerAPIWrapper::GenerateSessionToken(IHTTPLinkResourceConnection* connection, std::string& token)
{
    if (connection == nil)
        return kFalse;

    // A generic web server has no login handshake. Whatever token the caller
    // configured on the connection (a bearer token, say) is used as-is; when
    // none was configured we still succeed, and requests go out anonymously.
    std::stringstream loginCredentials;
    if (connection->GetLoginCredentials(loginCredentials))
    {
        try
        {
            JSON loginJson;
            loginJson.read_json(loginCredentials);
            if (loginJson.checkKey(token_key))
                token = loginJson.GetString(token_key);
        }
        catch (...)
        {
            token.clear();
        }
    }

    return kTrue;
}

//========================================================================================
// HttpLnkServerAPIWrapper::IsServerAccessible
//========================================================================================
bool16 HttpLnkServerAPIWrapper::IsServerAccessible(IHTTPLinkResourceConnection* connection, bool16& serverAccessible)
{
    HTTPLinkSubsystemTypes::Headers headers;
    GetRequestHeaders(headers, connection);

    InterfacePtr<IHTTPServerInfo> httpServerInfo(connection, UseDefaultIID());
    if (httpServerInfo == nil)
        return kFalse;

    URI probeURI = ToWireURI(httpServerInfo->GetServerURI());
    const IDHTTPWrapper::URL requestURL = probeURI.GetURIW();

    ts_SyncServerAccessibleValid = kFalse;
    ts_SyncServerAccessible      = kFalse;

    IDHTTPWrapper::CompletionFunction completion =
        boost::bind<void>(&HttpLnkServerAPIWrapper::ServerAccessibleCallbackOnWorkerThread, _1, _2);

    IDHTTPWrapper::HTTP http;
    if (!http.GET(completion, requestURL, kStatusRequestTimeout_InMS, headers))
        return kFalse;

    if (!ts_SyncServerAccessibleValid)
        return kFalse;

    serverAccessible = ts_SyncServerAccessible;
    return kTrue;
}

//========================================================================================
// HttpLnkServerAPIWrapper::ServerAccessibleCallbackOnWorkerThread
//========================================================================================
void HttpLnkServerAPIWrapper::ServerAccessibleCallbackOnWorkerThread(IDHTTPWrapper::RequestID inRequestID,
                                                                      IDHTTPWrapper::Response inResponse)
{
    // Any answer at all, even a 404 for the origin root, proves the server
    // is reachable. Only a transport failure means "not accessible".
    ts_SyncServerAccessible      = inResponse.IsNetworkConnectivityError() ? kFalse : kTrue;
    ts_SyncServerAccessibleValid = kTrue;
}

//========================================================================================
// HttpLnkServerAPIWrapper::FetchAssetPathAndRequestHeadersForDownload
//========================================================================================
bool16 HttpLnkServerAPIWrapper::FetchAssetPathAndRequestHeadersForDownload(IHTTPLinkResourceConnection* connection,
                                                                           const URI& inURI,
                                                                           URI& assetURL,
                                                                           HTTPLinkSubsystemTypes::Headers& headers) const
{
    // Carry authority, path, query and fragment through verbatim. This is what
    // makes arbitrary hosts work, where the SDK sample pinned one server.
    assetURL = ToWireURI(inURI);

    return GetRequestHeaders(headers, connection);
}
