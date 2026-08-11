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
#include "HTTPAssetLinkResourceStateUpdater.h"

// Project includes
#include "HttpLinkID.h"

class HttpLnkLinkResourceStateUpdater : HTTPAssetLinkResourceStateUpdater
{
public:
    typedef HTTPAssetLinkResourceStateUpdater inherited;
    
    HttpLnkLinkResourceStateUpdater(IPMUnknown* boss);
    ~HttpLnkLinkResourceStateUpdater();
};


CREATE_PMINTERFACE(HttpLnkLinkResourceStateUpdater, kHttpLnkLinkResourceStateUpdaterImpl)


//========================================================================================
// HttpLnkLinkResourceStateUpdater::Constructor
//========================================================================================
HttpLnkLinkResourceStateUpdater::HttpLnkLinkResourceStateUpdater(IPMUnknown* boss)
: inherited(boss)
{
}

//========================================================================================
// HttpLnkLinkResourceStateUpdater::Destructor
//========================================================================================
HttpLnkLinkResourceStateUpdater::~HttpLnkLinkResourceStateUpdater()
{
}

