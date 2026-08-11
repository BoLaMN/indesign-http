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
#include "CPMUnknown.h"

#include "IDataBase.h"
#include "ILinkResourceHandler.h"
#include "ILinkResourceStateUpdater.h"
#include "IHTTPLinkSubsystemObjectFactory.h"
#include "IHTTPLinkResourceConnection.h"
#include "IHTTPLinkResourceServerAPIWrapper.h"

#include "HttpLinkID.h"

//========================================================================================
// Class HttpLnkLinkResourceFactory
//========================================================================================
/** Implementation of IHTTPLinkSubsystemObjectFactory to instantiate link resource related classes,
	
	@ingroup httplink.sdk
*/
class HttpLnkLinkResourceFactory : public CPMUnknown<IHTTPLinkSubsystemObjectFactory>
{
public:
    typedef CPMUnknown<IHTTPLinkSubsystemObjectFactory> inherited;

	/**
     Constructor.
     @param boss IN interface ptr from boss object on which this interface is aggregated.
	 */
	HttpLnkLinkResourceFactory(IPMUnknown* boss);
    
	/**
     Destructor.
	 */
	virtual ~HttpLnkLinkResourceFactory();

    /**
     Gets the URI scheme supported by the resource connection.
     @param schemes [OUT] all the registered schemes
     */ 
	virtual void GetSchemes(K2Vector<WideString>& schemes) const;
	
    /**
     Returns the link resource handler that will be used by a link resource
     object to act on an resource whose URI scheme matches one of the
     supported schemes. Caller assumes responsibility for releasing the returned object.
     @param uri    [IN] URI of the link resource the handler will be used for.
     @param db     [IN] Database that contains the link resource.
     @return HTTP Link resource handler. \\
     */
	virtual ILinkResourceHandler* QueryHandler(const URI& uri, IDataBase* db) const;

    /**
     Returns the link resource state updater that will be used to obtain and
     update the state of a link resource whose URI scheme matches one of the
     supported schemes. Caller assumes responsibility for releasing the returned object.
     @param uri    [IN] URI of the link resource whose state will be updated.
     @param db     [IN] Database that contains the link resource.
     @return HTTP Link resource state updater. \\
     */
	virtual ILinkResourceStateUpdater* QueryStateUpdater(const URI& uri, IDataBase* db) const;

    /**
     Returns the http link resource connection that will be used by a http links subsystem to
     interact with the http resource connection whose URI scheme matches one of the
     supported scheme. Caller assumes responsibility for releasing the returned object.
     @param uri    [IN] URI of the link resource the handler will be used for.
     @param db     [IN] Database that contains the link resource.
     @return Http Link resource connection. \\
     */
    virtual IHTTPLinkResourceConnection* QueryHTTPLinkResourceConnection(const URI& uri, IDataBase* db) const;
    
    /**
     Returns the http link resource server API wrapper that will be used by a http links subsystem to
     interact with the http resource server whose URI scheme matches one of the
     supported scheme. Caller assumes responsibility for releasing the returned object.
     @param uri    [IN] URI of the link resource the handler will be used for.
     @param db     [IN] Database that contains the link resource.
     @return Http Link resource server API Wrapper. \\
     */
    virtual IHTTPLinkResourceServerAPIWrapper* QueryHTTPLinkResourceServerAPIWrapper(const URI& uriScheme, IDataBase* db) const;
    
private:
	// Prevent copy construction and assignment.
	HttpLnkLinkResourceFactory(const HttpLnkLinkResourceFactory&);
	HttpLnkLinkResourceFactory& operator=(const HttpLnkLinkResourceFactory&);
};

static const WideString k_HttpScheme(kHttpLnkScheme);
static const WideString k_HttpsScheme(kHttpLnkSchemeSecure);

CREATE_PMINTERFACE(HttpLnkLinkResourceFactory, kHttpLnkLinkResourceFactoryImpl)


//========================================================================================
//
// HttpLnkLinkResourceFactory Public Implementation
//
//========================================================================================

//========================================================================================
// Constructor
//========================================================================================
HttpLnkLinkResourceFactory::HttpLnkLinkResourceFactory(IPMUnknown* boss)
: inherited(boss)
{
}

//========================================================================================
// Destructor
//========================================================================================
HttpLnkLinkResourceFactory::~HttpLnkLinkResourceFactory()
{
}

//========================================================================================
// HttpLnkLinkResourceFactory::GetSchemes
//========================================================================================
void HttpLnkLinkResourceFactory::GetSchemes(K2Vector<WideString>& schemes) const
{
	// Register both halves of the private scheme pair. Each maps onto a real
	// wire scheme at request time (see HttpLnkServerAPIWrapper::ToWireURI).
	schemes.clear();
	schemes.push_back(k_HttpScheme);
	schemes.push_back(k_HttpsScheme);
}

//========================================================================================
// HttpLnkLinkResourceFactory::QueryHandler
//========================================================================================
ILinkResourceHandler* HttpLnkLinkResourceFactory::QueryHandler(const URI& uri, IDataBase* db) const
{
    return ::CreateObject2<ILinkResourceHandler>(kHttpLnkLinkResourceHandlerBoss);
}

//========================================================================================
// HttpLnkLinkResourceFactory::QueryStateUpdater
//========================================================================================
ILinkResourceStateUpdater* HttpLnkLinkResourceFactory::QueryStateUpdater(const URI& uri, IDataBase* db) const
{
    return ::CreateObject2<ILinkResourceStateUpdater>(kHttpLnkLinkResourceStateUpdaterBoss);
}


//========================================================================================
// HttpLnkLinkResourceFactory::QueryHTTPLinkResourceConnection
//========================================================================================
IHTTPLinkResourceConnection* HttpLnkLinkResourceFactory::QueryHTTPLinkResourceConnection(const URI& uri, IDataBase* db) const
{
    return ::CreateObject2<IHTTPLinkResourceConnection>(kHttpLnkLinkResourceHelperBoss);
}

//========================================================================================
// HttpLnkLinkResourceFactory::QueryHTTPLinkResourceServerAPIWrapper
//========================================================================================
IHTTPLinkResourceServerAPIWrapper* HttpLnkLinkResourceFactory::QueryHTTPLinkResourceServerAPIWrapper(const URI& uriScheme, IDataBase* db) const
{
    return ::CreateObject2<IHTTPLinkResourceServerAPIWrapper>(kHttpLnkLinkResourceServerHelperBoss);
}

