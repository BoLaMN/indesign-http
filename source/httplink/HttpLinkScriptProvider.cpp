//========================================================================================
//
//  HttpLinkScriptProvider.cpp
//
//  Scripting surface: doc.placeFromURL(), link.relinkToURL(), link.isRemoteLink
//  and link.remoteURL.
//
//  Lives in the model plug-in so ExtendScript, UXP and InDesign Server all reach
//  it. The scripting DOM is the only C++/UXP bridge, and Server never loads a
//  UI plug-in. Scripts pass ordinary http(s) URLs; the idhttp scheme stays
//  internal.
//
//========================================================================================

#include "VCPlugInHeaders.h"

#include "CScriptProvider.h"
#include "IDocument.h"
#include "IImportExportFacade.h"
#include "ILink.h"
#include "ILinkFacade.h"
#include "ILinkManager.h"
#include "IHierarchy.h"
#include "ILinkResource.h"
#include "IPlaceGun.h"
#include "IScript.h"
#include "IScriptRequestData.h"
#include "IScriptErrorUtils.h"
#include "IScriptUtils.h"
#include "ISpread.h"
#include "IStringData.h"
#include "URI.h"
#include "StringUtils.h"
#include "Utils.h"

#include "HttpLinkID.h"


/** Adds the HttpLink methods and properties to the Application and Link objects.
    @ingroup httplink.sdk
*/
class HttpLnkScriptProvider : public CScriptProvider
{
public:
    HttpLnkScriptProvider(IPMUnknown* boss) : CScriptProvider(boss) {}
    virtual ~HttpLnkScriptProvider() {}

    virtual ErrorCode HandleMethod(ScriptID methodID, IScriptRequestData* data, IScript* parent);
    virtual ErrorCode AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* parent);

private:
    ErrorCode PlaceFromURL(IScriptRequestData* data, IScript* parent);
    ErrorCode RelinkToURL(IScriptRequestData* data, IScript* parent);

    /** Reads the required "url" parameter. */
    static bool16 GetURLParam(IScriptRequestData* data, PMString& outURL);

    /** http(s) URL -> our private scheme. Shared shape with the UI plug-in's
        BuildResourceURI; kept here so Server, which never loads the UI plug-in,
        still gets the conversion.
    */
    static bool16 BuildResourceURI(const PMString& typed, URI& outURI);

    /** The reverse: our stored URI -> the real URL to show a script. */
    static PMString ToDisplayURL(const URI& uri);

    /** True if this URI belongs to us. */
    static bool16 IsOurs(const URI& uri);

    /** The ILink behind the Link script object the call was made on. */
    static ILink* QueryLinkFromScript(IScript* parent);

    /** The link for a placed item, searching the item then its descendants. */
    static UID FindLinkForItem(ILinkManager* linkMgr, IDataBase* db, UID itemUID, int depth = 0);
};

CREATE_PMINTERFACE(HttpLnkScriptProvider, kHttpLnkScriptProviderImpl)

//========================================================================================
// Helpers
//========================================================================================
bool16 HttpLnkScriptProvider::IsOurs(const URI& uri)
{
    const WideString scheme(uri.GetComponent(URI::kScheme));
    return scheme == WideString(kHttpLnkScheme) || scheme == WideString(kHttpLnkSchemeSecure);
}

bool16 HttpLnkScriptProvider::BuildResourceURI(const PMString& typed, URI& outURI)
{
    std::string url = typed.GetUTF8String();

    const std::string ws(" \t\r\n");
    const std::string::size_type first = url.find_first_not_of(ws);
    if (first == std::string::npos)
        return kFalse;
    const std::string::size_type last = url.find_last_not_of(ws);
    url = url.substr(first, last - first + 1);

    const char* scheme = nil;
    if (url.compare(0, 8, "https://") == 0)      { scheme = kHttpLnkSchemeSecure; url.erase(0, 8); }
    else if (url.compare(0, 7, "http://") == 0)  { scheme = kHttpLnkScheme;       url.erase(0, 7); }
    else if (url.compare(0, 10, "idhttps://") == 0 || url.compare(0, 9, "idhttp://") == 0)
    {
        // Already one of ours: accept verbatim so round-tripping a stored URI works.
        outURI = URI(url.c_str());
        return kTrue;
    }
    else
    {
        // No scheme: assume https rather than quietly downgrading to plaintext.
        scheme = kHttpLnkSchemeSecure;
    }

    const std::string::size_type slash = url.find('/');
    std::string authority = (slash == std::string::npos) ? url : url.substr(0, slash);
    std::string path      = (slash == std::string::npos) ? std::string("/") : url.substr(slash);
    if (authority.empty())
        return kFalse;

    std::string query;
    const std::string::size_type qmark = path.find('?');
    if (qmark != std::string::npos)
    {
        query = path.substr(qmark + 1);
        path  = path.substr(0, qmark);
    }

    outURI.SetComponent(URI::kScheme,    WideString(scheme));
    outURI.SetComponent(URI::kAuthority, WideString(authority.c_str()));
    outURI.SetComponent(URI::kPath,      WideString(path.c_str()));
    if (!query.empty())
        outURI.SetComponent(URI::kQuery, WideString(query.c_str()));

    return kTrue;
}

PMString HttpLnkScriptProvider::ToDisplayURL(const URI& uri)
{
    URI wire(uri);
    wire.SetComponent(URI::kScheme,
        (uri.GetComponent(URI::kScheme) == WideString(kHttpLnkSchemeSecure))
            ? kHTTPSAssetURIScheme : kHTTPAssetURIScheme);

    PMString out;
    out.SetUTF8String(wire.GetURI());
    out.SetTranslatable(kFalse);
    return out;
}

bool16 HttpLnkScriptProvider::GetURLParam(IScriptRequestData* data, PMString& outURL)
{
    ScriptData scriptData;
    if (data->ExtractRequestData(p_HttpLnkURLParam, scriptData) != kSuccess)
        return kFalse;
    return scriptData.GetPMString(outURL) == kSuccess;
}

UID HttpLnkScriptProvider::FindLinkForItem(ILinkManager* linkMgr, IDataBase* db, UID itemUID, int depth)
{
    if (linkMgr == nil || itemUID == kInvalidUID || depth > 4)
        return kInvalidUID;

    ILinkManager::QueryResult links;
    if (linkMgr->QueryLinksByObjectUID(itemUID, links) > 0 && !links.empty())
        return links.front();

    InterfacePtr<IHierarchy> hierarchy(db, itemUID, UseDefaultIID());
    if (hierarchy == nil)
        return kInvalidUID;

    for (int32 i = 0; i < hierarchy->GetChildCount(); ++i)
    {
        const UID found = FindLinkForItem(linkMgr, db, hierarchy->GetChildUID(i), depth + 1);
        if (found != kInvalidUID)
            return found;
    }
    return kInvalidUID;
}

ILink* HttpLnkScriptProvider::QueryLinkFromScript(IScript* parent)
{
    if (parent == nil)
        return nil;
    return (ILink*)parent->QueryInterface(IID_ILINK);
}

//========================================================================================
// HttpLnkScriptProvider::HandleMethod
//========================================================================================
ErrorCode HttpLnkScriptProvider::HandleMethod(ScriptID methodID, IScriptRequestData* data, IScript* parent)
{
    switch (methodID.Get())
    {
        case e_HttpLnkPlaceFromURL:  return PlaceFromURL(data, parent);
        case e_HttpLnkRelinkToURL:   return RelinkToURL(data, parent);
        default:                     return CScriptProvider::HandleMethod(methodID, data, parent);
    }
}

//========================================================================================
// HttpLnkScriptProvider::AccessProperty
//========================================================================================
ErrorCode HttpLnkScriptProvider::AccessProperty(ScriptID propID, IScriptRequestData* data, IScript* parent)
{
    switch (propID.Get())
    {
        case p_HttpLnkIsRemoteLink:
        case p_HttpLnkRemoteURL:
        {
            if (!data->IsPropertyGet())
            {
                // Both are read-only; the URL is changed through relinkToURL so
                // the relink actually happens rather than just rewriting a string.
                return Utils<IScriptErrorUtils>()->SetInvalidParameterErrorData(data, propID);
            }

            InterfacePtr<ILink> link(QueryLinkFromScript(parent));
            if (link == nil)
                return kFailure;

            InterfacePtr<ILinkResource> resource(
                ::GetDataBase(link), link->GetResource(), UseDefaultIID());
            if (resource == nil)
                return kFailure;

            const URI uri = resource->GetURI();
            ScriptData out;
            if (propID.Get() == p_HttpLnkIsRemoteLink)
                out.SetBoolean(IsOurs(uri));
            else
                out.SetPMString(IsOurs(uri) ? ToDisplayURL(uri) : PMString(""));

            data->AppendReturnData(parent, propID, out);
            return kSuccess;
        }
        default:
            return CScriptProvider::AccessProperty(propID, data, parent);
    }
}

//========================================================================================
// HttpLnkScriptProvider::PlaceFromURL
//========================================================================================
ErrorCode HttpLnkScriptProvider::PlaceFromURL(IScriptRequestData* data, IScript* parent)
{
    PMString url;
    if (!GetURLParam(data, url))
        return Utils<IScriptErrorUtils>()->SetInvalidParameterErrorData(data, p_HttpLnkURLParam);

    URI resourceURI;
    if (!BuildResourceURI(url, resourceURI))
        return Utils<IScriptErrorUtils>()->SetInvalidParameterErrorData(data, p_HttpLnkURLParam);

    // The method is declared on the Document object, so the target is whichever
    // document the script called it on: explicit, and it works headless.
    InterfacePtr<IDocument> document(parent, UseDefaultIID());
    if (document == nil)
        return kFailure;

    IDataBase* db = ::GetDataBase(document);
    if (db == nil)
        return kFailure;

    Utils<Facade::ILinkFacade>()->SetHTTPLinksDefaultRenditionType(ILinkResourceRenditionData::kOriginal);

    const ErrorCode err = Utils<Facade::IImportExportFacade>()->ImportAndLoadPlaceGun(
        db, resourceURI, kSuppressUI, kFalse, kFalse, kFalse,
        UID(kInvalidUID), IPlaceGun::kAddToFront);
    if (err != kSuccess)
        return err;

    // Hand back the Link so callers can check status or relink without hunting
    // through doc.links. The item is in the place gun rather than on a page, but
    // the import has already created the resource and the link, so the Link
    // object is live: place it with page.place() when ready.
    InterfacePtr<IPlaceGun> placeGun(document, UseDefaultIID());
    if (placeGun == nil || !placeGun->IsLoaded())
        return kSuccess;

    const UID itemUID = placeGun->GetFirstPlaceGunItemUID();
    if (itemUID == kInvalidUID)
        return kSuccess;

    InterfacePtr<ILinkManager> linkMgr(db, db->GetRootUID(), UseDefaultIID());
    if (linkMgr == nil)
        return kSuccess;

    // The place gun holds the frame; the link hangs off the graphic inside it,
    // so check the item first and then walk down.
    const UID linkUID = FindLinkForItem(linkMgr, db, itemUID);
    if (linkUID == kInvalidUID)
        return kSuccess;

    InterfacePtr<IScript> linkScript(db, linkUID, UseDefaultIID());
    if (linkScript == nil)
        return kSuccess;

    ScriptData out;
    out.SetObject(linkScript);
    data->AppendReturnData(parent, e_HttpLnkPlaceFromURL, out);
    return kSuccess;
}

//========================================================================================
// HttpLnkScriptProvider::RelinkToURL
//========================================================================================
ErrorCode HttpLnkScriptProvider::RelinkToURL(IScriptRequestData* data, IScript* parent)
{
    PMString url;
    if (!GetURLParam(data, url))
        return Utils<IScriptErrorUtils>()->SetInvalidParameterErrorData(data, p_HttpLnkURLParam);

    URI resourceURI;
    if (!BuildResourceURI(url, resourceURI))
        return Utils<IScriptErrorUtils>()->SetInvalidParameterErrorData(data, p_HttpLnkURLParam);

    InterfacePtr<ILink> link(QueryLinkFromScript(parent));
    if (link == nil)
        return kFailure;

    const UIDRef linkRef = ::GetUIDRef(link);

    // Relink, then update: unlike the stock reinitLink, which leaves the link
    // pointing at the new URI without fetching anything, this leaves the caller
    // with a link that has actually been resolved.
    UID newLinkUID(kInvalidUID);
    ErrorCode err = Utils<Facade::ILinkFacade>()->RelinkLink(linkRef, resourceURI, kSuppressUI, newLinkUID);
    if (err == kSuccess)
    {
        // Relinking leaves the link pointing at the new URI without fetching, so
        // update the (possibly replaced) link to actually resolve it.
        const UIDRef updatedRef = (newLinkUID != kInvalidUID)
            ? UIDRef(linkRef.GetDataBase(), newLinkUID) : linkRef;
        UID afterUpdateUID(kInvalidUID);
        err = Utils<Facade::ILinkFacade>()->UpdateLink(updatedRef, true /*bForceUpdate*/,
                                                       kSuppressUI, afterUpdateUID);
    }

    return err;
}
