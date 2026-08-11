//========================================================================================
//
//  HttpLinkInfoProviders.cpp
//
//  Extra Link Info rows: Remote URL, Web status, Version tag.
//
//  One service-provider boss per row under kLinkInfoService. All return empty
//  for links that are not ours, so file links do not gain three blank rows.
//  Model-side because the interfaces are (LinksID.h, not LinksUIID.h).
//
//  Note these rows are opt-in per user: Links panel flyout > Panel Options.
//
//========================================================================================

#include "VCPlugInHeaders.h"

#include "CLinkInfoProvider.h"
#include "ILink.h"
#include "ILinkResource.h"
#include "URI.h"
#include "StringUtils.h"
#include <string>

#include "HttpLinkID.h"

//========================================================================================
// Shared behaviour
//========================================================================================
/** Common ground for our Link Info rows: identifying our own links and turning
    the stored URI back into something a person recognises.
*/
class HttpLnkInfoProviderBase : public CLinkInfoProvider
{
public:
    HttpLnkInfoProviderBase(IPMUnknown* boss) : CLinkInfoProvider(boss) {}

    virtual PMString GetUpdatedInfoForLink(const ILink* link,
                                           const ILinkResource* linkResource,
                                           bool shortForm) const
    {
        if (linkResource == nil)
            return PMString("");

        const URI uri = linkResource->GetURI();
        if (!IsOurs(uri))
        {
            // Not an HttpLink resource: stay silent rather than showing an empty
            // labelled row for every ordinary file link in the document.
            return PMString("");
        }
        return GetValue(link, linkResource, uri, shortForm);
    }

    /** These rows describe live network state, so let the panel refresh them. */
    virtual bool16 IsLinkInfoDynamic() const { return kTrue; }

    virtual int32 GetDefaultColumnWidth() const { return 160; }

protected:
    virtual PMString GetValue(const ILink* link,
                              const ILinkResource* linkResource,
                              const URI& uri,
                              bool shortForm) const = 0;

    static bool16 IsOurs(const URI& uri)
    {
        const WideString scheme(uri.GetComponent(URI::kScheme));
        return scheme == WideString(kHttpLnkScheme) || scheme == WideString(kHttpLnkSchemeSecure);
    }

    /** Our private scheme back to the URL the user actually typed. */
    static PMString ToDisplayURL(const URI& uri)
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
};

//========================================================================================
// Remote URL
//========================================================================================
/** @ingroup httplink.sdk */
class HttpLnkURLInfoProvider : public HttpLnkInfoProviderBase
{
public:
    HttpLnkURLInfoProvider(IPMUnknown* boss) : HttpLnkInfoProviderBase(boss) {}

    virtual PMString GetInfoDescriptionString() const
    {
        PMString s("Remote URL");
        s.SetTranslatable(kFalse);
        return s;
    }

    virtual int32 GetDefaultColumnWidth() const { return 260; }

protected:
    virtual PMString GetValue(const ILink*, const ILinkResource*, const URI& uri, bool) const
    {
        return ToDisplayURL(uri);
    }
};

CREATE_PMINTERFACE(HttpLnkURLInfoProvider, kHttpLnkURLInfoProviderImpl)

//========================================================================================
// Web status
//========================================================================================
/** @ingroup httplink.sdk */
class HttpLnkStatusInfoProvider : public HttpLnkInfoProviderBase
{
public:
    HttpLnkStatusInfoProvider(IPMUnknown* boss) : HttpLnkInfoProviderBase(boss) {}

    virtual PMString GetInfoDescriptionString() const
    {
        PMString s("Web status");
        s.SetTranslatable(kFalse);
        return s;
    }

    virtual int32 GetDefaultColumnWidth() const { return 110; }

protected:
    virtual PMString GetValue(const ILink*, const ILinkResource* linkResource, const URI&, bool) const
    {
        PMString out;
        switch (linkResource->GetState())
        {
            case ILinkResource::kAvailable:
                out = PMString("Available");
                break;
            case ILinkResource::kMissing:
                // Deliberately distinguished from "unreachable": missing means the
                // server answered and said the asset is gone (404/410).
                out = PMString("Missing on server");
                break;
            case ILinkResource::kPending:
                out = PMString("Downloading");
                break;
            case ILinkResource::kInaccessible:
                // Network trouble, auth failure or a 5xx -- the asset may well be
                // fine, we just could not confirm it.
                out = PMString("Unreachable");
                break;
            default:
                out = PMString("Unknown");
                break;
        }
        out.SetTranslatable(kFalse);
        return out;
    }
};

CREATE_PMINTERFACE(HttpLnkStatusInfoProvider, kHttpLnkStatusInfoProviderImpl)

//========================================================================================
// Version tag
//========================================================================================
/** @ingroup httplink.sdk */
class HttpLnkStampInfoProvider : public HttpLnkInfoProviderBase
{
public:
    HttpLnkStampInfoProvider(IPMUnknown* boss) : HttpLnkInfoProviderBase(boss) {}

    virtual PMString GetInfoDescriptionString() const
    {
        PMString s("Version tag");
        s.SetTranslatable(kFalse);
        return s;
    }

protected:
    virtual PMString GetValue(const ILink*, const ILinkResource* linkResource, const URI&, bool) const
    {
        // The stamp is built by the base state updater from the asset's ETag,
        // falling back to Last-Modified. It is what "out of date" is decided on,
        // so showing it makes an otherwise invisible mechanism inspectable when a
        // link updates (or stubbornly doesn't).
        const WideString stamp = linkResource->GetStamp();
        std::string stampUTF8;
        StringUtils::ConvertWideStringToUTF8(stamp, stampUTF8);
        PMString out;
        out.SetUTF8String(stampUTF8);
        out.SetTranslatable(kFalse);
        return out;
    }
};

CREATE_PMINTERFACE(HttpLnkStampInfoProvider, kHttpLnkStampInfoProviderImpl)
