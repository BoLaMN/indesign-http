//========================================================================================
//  
//  $File$
//  
//  Owner: swagarg
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


#ifndef __HttpLinkID_h__
#define __HttpLinkID_h__

#include "SDKDef.h"

// Company:
#define kHttpLinkCompanyKey	kSDKDefPlugInCompanyKey		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kHttpLinkCompanyValue	kSDKDefPlugInCompanyValue	// Company name displayed externally.

// Plug-in:
#define kHttpLinkPluginName	"HttpLink.sdk"			// Name of this plug-in.
// PLACEHOLDER -- needs a real prefix from Adobe Developer Support before
// shipping. Two plug-ins sharing a prefix collide in every ID space.
#define kHttpLinkPrefixNumber	0x1DE000
#define kHttpLinkVersion		kSDKDefPluginVersionString						// Version of this plug-in (for the About Box).
#define kHttpLinkAuthor		"Adobe Developer Technologies"					// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kHttpLinkPrefixNumber above to modify the prefix.)
#define kHttpLinkPrefix		RezLong(kHttpLinkPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kHttpLinkStringPrefix	SDK_DEF_STRINGIZE(kHttpLinkPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kHttpLinkMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS // URL displayed in Missing Plug-in dialog
#define kHttpLinkMissingPluginAlertValue	kSDKDefMissingPluginAlertValue // Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem


// login information keys
#define username_key  "username"
#define password_key  "password"
#define token_key  "token"


// PluginID:
DECLARE_PMID(kPlugInIDSpace, kHttpLinkPluginID, kHttpLinkPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kHttpLnkLinkResourceProviderBoss, kHttpLinkPrefix + 3)
DECLARE_PMID(kClassIDSpace, kHttpLnkLinkResourceHandlerBoss, kHttpLinkPrefix + 4)
DECLARE_PMID(kClassIDSpace, kHttpLnkLinkResourceStateUpdaterBoss, kHttpLinkPrefix + 5)
DECLARE_PMID(kClassIDSpace, kHttpLnkLinkResourceHelperBoss, kHttpLinkPrefix + 6)
DECLARE_PMID(kClassIDSpace, kHttpLnkLinkResourceServerHelperBoss, kHttpLinkPrefix + 7)
DECLARE_PMID(kClassIDSpace, kHttpLnkScriptProviderBoss, kHttpLinkPrefix + 8)
DECLARE_PMID(kClassIDSpace, kHttpLnkURLInfoProviderBoss, kHttpLinkPrefix + 9)
DECLARE_PMID(kClassIDSpace, kHttpLnkStatusInfoProviderBoss, kHttpLinkPrefix + 10)
DECLARE_PMID(kClassIDSpace, kHttpLnkStampInfoProviderBoss, kHttpLinkPrefix + 11)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 12)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 13)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 14)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 15)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 16)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 17)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 18)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 19)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 20)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 21)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 22)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 23)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 24)
//DECLARE_PMID(kClassIDSpace, kHttpLinkBoss, kHttpLinkPrefix + 25)


// InterfaceIDs: none defined; kHttpLinkPrefix + 0..25 reserved.


// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kHttpLnkLinkResourceFactoryImpl, kHttpLinkPrefix + 0)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkLinkResourceHandlerImpl, kHttpLinkPrefix + 1)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkLinkResourceStateUpdaterImpl, kHttpLinkPrefix + 2)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkLinkResourceConnectionImpl, kHttpLinkPrefix + 3)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkLinkResourceStateBatchUpdaterImpl, kHttpLinkPrefix + 4)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkResourceServerAPIWrapperImpl, kHttpLinkPrefix + 5)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkLinkResourceHelperHandlerImpl, kHttpLinkPrefix + 6)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkScriptProviderImpl, kHttpLinkPrefix + 8)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkURLInfoProviderImpl, kHttpLinkPrefix + 9)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkStatusInfoProviderImpl, kHttpLinkPrefix + 10)
DECLARE_PMID(kImplementationIDSpace, kHttpLnkStampInfoProviderImpl, kHttpLinkPrefix + 11)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 12)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 13)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 14)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 15)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 16)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 17)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 18)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 19)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 20)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 21)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 22)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 23)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 24)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkImpl, kHttpLinkPrefix + 25)


// ScriptInfo IDs -- the scripting surface, shared by ExtendScript, UXP and
// InDesign Server (all three drive the same DOM; there is no C++/UXP bridge).
DECLARE_PMID(kScriptInfoIDSpace, kHttpLnkPlaceFromURLMethodScriptElement,  kHttpLinkPrefix + 0)
DECLARE_PMID(kScriptInfoIDSpace, kHttpLnkRelinkToURLMethodScriptElement,   kHttpLinkPrefix + 1)
DECLARE_PMID(kScriptInfoIDSpace, kHttpLnkIsRemoteLinkPropertyScriptElement,kHttpLinkPrefix + 2)
DECLARE_PMID(kScriptInfoIDSpace, kHttpLnkRemoteURLPropertyScriptElement,   kHttpLinkPrefix + 3)
DECLARE_PMID(kScriptInfoIDSpace, kHttpLnkURLParamScriptElement,            kHttpLinkPrefix + 4)

// Four-char script IDs. Must be unique across the whole DOM.
enum HttpLnkScriptIDs
{
	e_HttpLnkPlaceFromURL = 'hlPU',
	e_HttpLnkRelinkToURL  = 'hlRU',
	p_HttpLnkIsRemoteLink = 'hlIR',
	p_HttpLnkRemoteURL    = 'hlRL',
	p_HttpLnkURLParam     = 'hlUP',
};

// ActionIDs:
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 5)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 6)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 7)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 8)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 9)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 10)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 11)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 12)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 13)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 14)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 15)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 16)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 17)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 18)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 19)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 20)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 21)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 22)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 23)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 24)
//DECLARE_PMID(kActionIDSpace, kHttpLinkActionID, kHttpLinkPrefix + 25)


// WidgetIDs: none defined (model plug-in); kHttpLinkPrefix + 2..25 reserved.


// Initial data format version numbers
#define kHttpLinkFirstMajorFormatNumber  RezLong(1)
#define kHttpLinkFirstMinorFormatNumber  RezLong(0)

// Data format version numbers for the PluginVersion resource 
#define kHttpLinkCurrentMajorFormatNumber kHttpLinkFirstMajorFormatNumber
#define kHttpLinkCurrentMinorFormatNumber kHttpLinkFirstMinorFormatNumber

// Private scheme pair, mapped back to http/https at request time (ToWireURI).
// Claiming bare http/https isn't safe: scheme registration is global and
// first-come, so two plug-ins doing it conflict and the loser's links break.
#define kHttpLnkScheme          "idhttp"
#define kHttpLnkSchemeSecure    "idhttps"

#endif // __HttpLinkID_h__
