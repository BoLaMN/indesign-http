//========================================================================================
//
//  $File$
//
//  Owner: Adobe Developer Technologies
//
//  $Author$
//
//  $DateTime$
//
//  $Revision$
//
//  $Change$
//
//  ADOBE CONFIDENTIAL
//  
//  Copyright 2019 Adobe
//  All Rights Reserved.
//
//  NOTICE: Adobe permits you to use, modify, and distribute this file in
//  accordance with the terms of the Adobe license agreement accompanying
//  it. If you have received this file from a source other than Adobe,
//  then your use, modification, or distribution of it requires the prior
//  written permission of Adobe. 
//
//========================================================================================

#include "VCPlugInHeaders.h"

#include "HttpLinkID.h"
#include "HTTPAssetLinkResourceHandler.h"

/**
 Provide methods to manipulate the resource
 
 @ingroup httplink.sdk
 */

class HttpLnkLinkResourceHandler : HTTPAssetLinkResourceHandler
{
public:
	typedef HTTPAssetLinkResourceHandler inherited;
	typedef object_type data_type;

	HttpLnkLinkResourceHandler(IPMUnknown* boss);
	virtual ~HttpLnkLinkResourceHandler();

	virtual WideString GetLongResourceName(const UIDRef& ref, const URI& uri, bool bUIName) const;

	virtual bool IsHTTPSSupportedURI(const URI& uri) const;

    virtual bool canGetInstanceName() const ;
    
    virtual ErrorCode GetInstanceName(PMString &uri) const ;
    
private:
	// Prevent copy construction and assignment.
	HttpLnkLinkResourceHandler(const HttpLnkLinkResourceHandler&);
	HttpLnkLinkResourceHandler& operator=(const HttpLnkLinkResourceHandler&);
};

CREATE_PMINTERFACE(HttpLnkLinkResourceHandler, kHttpLnkLinkResourceHandlerImpl)

/* Constructor
*/
HttpLnkLinkResourceHandler::HttpLnkLinkResourceHandler(IPMUnknown* boss)
: inherited(boss)
{
}

/* Destructor
*/
HttpLnkLinkResourceHandler::~HttpLnkLinkResourceHandler()
{
}

/* IsHTTPSSupportedURI
   Tells the subsystem whether this link must be fetched over TLS. Our scheme
   pair encodes that directly, so the answer is simply which half we hold.
*/
bool HttpLnkLinkResourceHandler::IsHTTPSSupportedURI(const URI& uri) const
{
	return uri.GetComponent(URI::kScheme) == WideString(kHttpLnkSchemeSecure);
}

/* GetLongResourceName
*/
WideString HttpLnkLinkResourceHandler::GetLongResourceName(const UIDRef& ref, const URI& uri, bool bUIName) const
{
    // Return resource name from cache, if already present
    if (bUIName && !fLongUINameCache.empty())
    {
        return fLongUINameCache;
    }
    if (!fLongNameCache.empty())
        return fLongNameCache;

    // else, Generate resource name using uri
    // Show the user the genuine URL (correct http/https), not our internal
    // scheme and not a scheme forced to http like the SDK sample does.
    URI assetURI(uri);
    assetURI.SetComponent(URI::kScheme,
        IsHTTPSSupportedURI(uri) ? kHTTPSAssetURIScheme : kHTTPAssetURIScheme);
    WideString wUri;
    URICommonUtils::URIEncodingToWideString(assetURI.GetURI(), wUri);
    fLongNameCache = wUri;
    
    // Currently, fLongUINameCache and fLongNameCache are same
    // These can be made different if we wish to show different path
    // in UI and different for internal use
    fLongUINameCache = fLongNameCache;

	return bUIName ? fLongUINameCache : fLongNameCache;
}

/*
 canGetInstanceName
 There is no single "server instance" for this plug-in -- every link carries its
 own origin -- so we decline to advertise one rather than lying about a fixed
 host the way a single-DAM connector would.
 */
bool HttpLnkLinkResourceHandler::canGetInstanceName() const
{
    return kFalse;
}

/*
 GetInstanceName
 */
ErrorCode HttpLnkLinkResourceHandler::GetInstanceName(PMString &uri) const
{
    return kFailure;
}
