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
#include "HTTPAssetLinkResourceHelperHandler.h"
#include "IActionManager.h"
#include "IApplication.h"
#include "LinksUIID.h"   // kReplaceFPOWithOriginalLinkActionID (SDK public header, not our UI plug-in)

/**
 Provide methods to manipulate the resource
 
 @ingroup httplink.sdk
 */

class HttpLnkLinkResourceHelperHandler : HTTPAssetLinkResourceHelperHandler
{
public:
	typedef HTTPAssetLinkResourceHelperHandler inherited;
	typedef object_type data_type;

	HttpLnkLinkResourceHelperHandler(IPMUnknown* boss);
	virtual ~HttpLnkLinkResourceHelperHandler();
    //replace fpo with original asset.
    virtual void ReplaceWithOriginalAsset() const;

};

CREATE_PMINTERFACE(HttpLnkLinkResourceHelperHandler, kHttpLnkLinkResourceHelperHandlerImpl)

/* Constructor
*/
HttpLnkLinkResourceHelperHandler::HttpLnkLinkResourceHelperHandler(IPMUnknown* boss)
: inherited(boss)
{
}

/* Destructor
*/
HttpLnkLinkResourceHelperHandler::~HttpLnkLinkResourceHelperHandler()
{
}


//Replace FPO with original Asset
void HttpLnkLinkResourceHelperHandler::ReplaceWithOriginalAsset() const
{
    InterfacePtr<const IApplication> app(GetExecutionContextSession()->QueryApplication());
    InterfacePtr<IActionManager> actionMgr(app->QueryActionManager());
    actionMgr->PerformAction(GetExecutionContextSession()->GetActiveContext(), kReplaceFPOWithOriginalLinkActionID);
}
