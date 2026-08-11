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

#include "HttpLinkID.h"
#include "HTTPAssetLinkResourceStateBatchUpdater.h"

class HttpLnkLinkResourceStateBatchUpdater : HTTPAssetLinkResourceStateBatchUpdater
{
public:
	typedef HTTPAssetLinkResourceStateBatchUpdater inherited;
	HttpLnkLinkResourceStateBatchUpdater(IPMUnknown* boss);
	~HttpLnkLinkResourceStateBatchUpdater();
	
};

CREATE_PMINTERFACE(HttpLnkLinkResourceStateBatchUpdater, kHttpLnkLinkResourceStateBatchUpdaterImpl)

HttpLnkLinkResourceStateBatchUpdater::HttpLnkLinkResourceStateBatchUpdater(IPMUnknown* boss)
: inherited(boss)
{
}

HttpLnkLinkResourceStateBatchUpdater::~HttpLnkLinkResourceStateBatchUpdater()
{
}

