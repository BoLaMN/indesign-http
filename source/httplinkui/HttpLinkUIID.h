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


#ifndef __HttpLinkUIID_h__
#define __HttpLinkUIID_h__

#include "SDKDef.h"

// Company:
#define kHttpLinkUICompanyKey	kSDKDefPlugInCompanyKey		// Company name used internally for menu paths and the like. Must be globally unique, only A-Z, 0-9, space and "_".
#define kHttpLinkUICompanyValue	kSDKDefPlugInCompanyValue	// Company name displayed externally.

// Plug-in:
#define kHttpLinkUIPluginName	"HttpLinkUI"			// Name of this plug-in.
// PLACEHOLDER -- see HttpLinkID.h. One block above the model plug-in's.
#define kHttpLinkUIPrefixNumber	0x1DE100
#define kHttpLinkUIVersion		kSDKDefPluginVersionString						// Version of this plug-in (for the About Box).
#define kHttpLinkUIAuthor		"Adobe Developer Technologies"					// Author of this plug-in (for the About Box).

// Plug-in Prefix: (please change kHttpLinkUIPrefixNumber above to modify the prefix.)
#define kHttpLinkUIPrefix		RezLong(kHttpLinkUIPrefixNumber)				// The unique numeric prefix for all object model IDs for this plug-in.
#define kHttpLinkUIStringPrefix	SDK_DEF_STRINGIZE(kHttpLinkUIPrefixNumber)	// The string equivalent of the unique prefix number for  this plug-in.

// Missing plug-in: (see ExtraPluginInfo resource)
#define kHttpLinkUIMissingPluginURLValue		kSDKDefPartnersStandardValue_enUS // URL displayed in Missing Plug-in dialog
#define kHttpLinkUIMissingPluginAlertValue	kSDKDefMissingPluginAlertValue // Message displayed in Missing Plug-in dialog - provide a string that instructs user how to solve their missing plug-in problem


// PluginID:
DECLARE_PMID(kPlugInIDSpace, kHttpLinkUIPluginID, kHttpLinkUIPrefix + 0)

// ClassIDs:
DECLARE_PMID(kClassIDSpace, kHttpLinkUIActionComponentBoss, kHttpLinkUIPrefix + 0)
DECLARE_PMID(kClassIDSpace, kHttpLinkUIPanelWidgetBoss, kHttpLinkUIPrefix + 1)
DECLARE_PMID(kClassIDSpace, kPlaceButtonBoss, kHttpLinkUIPrefix + 3)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 4)
DECLARE_PMID(kClassIDSpace, kHttpLinkUIRelinkDialogBoss, kHttpLinkUIPrefix + 5)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 6)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 7)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 8)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 9)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 10)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 11)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 12)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 13)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 14)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 15)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 16)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 17)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 18)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 19)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 20)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 21)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 22)
DECLARE_PMID(kClassIDSpace, kHttpLinkUIDropTargetProviderBoss, kHttpLinkUIPrefix + 23)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 24)
//DECLARE_PMID(kClassIDSpace, kHttpLinkUIBoss, kHttpLinkUIPrefix + 25)


// InterfaceIDs:
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 0)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 1)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 2)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 3)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 4)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 5)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 6)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 7)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 8)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 9)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 10)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 11)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 12)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 13)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 14)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 15)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 16)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 17)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 18)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 19)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 20)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 21)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 22)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 23)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 24)
//DECLARE_PMID(kInterfaceIDSpace, IID_ICUSTOMHTTPLINKUIINTERFACE, kHttpLinkUIPrefix + 25)


// ImplementationIDs:
DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIActionComponentImpl, kHttpLinkUIPrefix + 0 )
DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIPlaceButtonObserverImpl, kHttpLinkUIPrefix + 1)
DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIObserverImpl, kHttpLinkUIPrefix + 2)
DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIRelinkDialogControllerImpl, kHttpLinkUIPrefix + 3)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 4)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 5)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 6)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 7)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 8)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 9)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 10)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 11)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 12)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 13)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 14)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 15)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 16)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 17)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 18)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 19)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 20)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 21)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 22)
DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIDDTargetFlavorHelperImpl, kHttpLinkUIPrefix + 23)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 24)
//DECLARE_PMID(kImplementationIDSpace, kHttpLinkUIImpl, kHttpLinkUIPrefix + 25)


// ActionIDs:
DECLARE_PMID(kActionIDSpace, kHttpLinkUIAboutActionID, kHttpLinkUIPrefix + 0)
DECLARE_PMID(kActionIDSpace, kHttpLinkUIPanelWidgetActionID, kHttpLinkUIPrefix + 1)
DECLARE_PMID(kActionIDSpace, kHttpLinkUISeparator1ActionID, kHttpLinkUIPrefix + 2)
DECLARE_PMID(kActionIDSpace, kHttpLinkUIPopupAboutThisActionID, kHttpLinkUIPrefix + 3)
DECLARE_PMID(kActionIDSpace, kHttpLinkUIRelinkFromWebActionID, kHttpLinkUIPrefix + 4)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 11)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 12)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 13)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 14)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 15)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 16)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 17)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 18)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 19)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 20)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 21)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 22)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 23)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 24)
//DECLARE_PMID(kActionIDSpace, kHttpLinkUIActionID, kHttpLinkUIPrefix + 25)


// WidgetIDs:
DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIPanelWidgetID, kHttpLinkUIPrefix + 0)
DECLARE_PMID(kWidgetIDSpace, kPlaceURLButtonWidgetID, kHttpLinkUIPrefix + 2)
DECLARE_PMID(kWidgetIDSpace, kURLEditWidgetID, kHttpLinkUIPrefix + 3)
DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIRelinkDialogWidgetID, kHttpLinkUIPrefix + 4)
DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIRelinkURLEditWidgetID, kHttpLinkUIPrefix + 5)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 6)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 7)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 8)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 9)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 10)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 11)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 12)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 13)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 14)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 15)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 16)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 17)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 18)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 19)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 20)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 21)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 22)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 23)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 24)
//DECLARE_PMID(kWidgetIDSpace, kHttpLinkUIWidgetID, kHttpLinkUIPrefix + 25)


// "About Plug-ins" sub-menu:
#define kHttpLinkUIAboutMenuKey			kHttpLinkUIStringPrefix "kHttpLinkUIAboutMenuKey"
#define kHttpLinkUIAboutMenuPath		kSDKDefStandardAboutMenuPath kHttpLinkUICompanyKey

// Menu item keys:

// Other StringKeys:
#define kHttpLinkUIAboutBoxStringKey	kHttpLinkUIStringPrefix "kHttpLinkUIAboutBoxStringKey"
#define kHttpLinkUIPanelTitleKey					kHttpLinkUIStringPrefix	"kHttpLinkUIPanelTitleKey"
#define kHttpLinkUIInternalPopupMenuNameKey kHttpLinkUIStringPrefix	"kHttpLinkUIInternalPopupMenuNameKey"
#define kHttpLinkUITargetMenuPath kHttpLinkUIInternalPopupMenuNameKey

// Menu item positions:

#define	kHttpLinkUISeparator1MenuItemPosition		10.0
#define kHttpLinkUIAboutThisMenuItemPosition		11.0


// Initial data format version numbers
#define kHttpLinkUIFirstMajorFormatNumber  RezLong(1)
#define kHttpLinkUIFirstMinorFormatNumber  RezLong(0)

// Data format version numbers for the PluginVersion resource 
#define kHttpLinkUICurrentMajorFormatNumber kHttpLinkUIFirstMajorFormatNumber
#define kHttpLinkUICurrentMinorFormatNumber kHttpLinkUIFirstMinorFormatNumber

// Extensions the drop handler accepts. LooksPlaceable() parses these at runtime
// and the panel builds its captions from them, so the list shown is the list
// enforced. Space separated, lower case.
// The raster list is long, so the panel shows it on two lines. It is split
// into two halves here and reassembled below, so C++ (LooksPlaceable) and the
// panel captions still share the exact same extension text.
#define kHttpLnkExtsRaster1   "jpg jpeg jpf jpx j2k png gif tif tiff bmp"
#define kHttpLnkExtsRaster2   "psd psb webp heic avif dcs pcx pct"
#define kHttpLnkExtsRaster    kHttpLnkExtsRaster1 " " kHttpLnkExtsRaster2
#define kHttpLnkExtsVector    "ai eps pdf svg emf wmf"
#define kHttpLnkExtsInDesign  "indd indt idms idml icml incx inma"
#define kHttpLnkExtsText      "txt text rtf doc docx xlsx"

// Links panel menu integration
#define kHttpLinkUIRelinkPopupMenuPath      "#LinksUIPanelMenu"        // panel flyout
#define kHttpLinkUIRelinkContextMenuPath    "RtMenuLinksUILinkItem"    // right-click on a link
#define kHttpLinkUIRelinkFromWebMenuKey     kHttpLinkUIStringPrefix "kHttpLinkUIRelinkFromWebMenuKey"
#define kHttpLinkUIRelinkDialogTitleKey     kHttpLinkUIStringPrefix "kHttpLinkUIRelinkDialogTitleKey"

// Panel caption keys
#define kHttpLinkUIDragHintKey      kHttpLinkUIStringPrefix "kHttpLinkUIDragHintKey"
#define kHttpLinkUIExtsTitleKey     kHttpLinkUIStringPrefix "kHttpLinkUIExtsTitleKey"
#define kHttpLinkUIExtsRasterKey    kHttpLinkUIStringPrefix "kHttpLinkUIExtsRasterKey"
#define kHttpLinkUIExtsRaster2Key   kHttpLinkUIStringPrefix "kHttpLinkUIExtsRaster2Key"
#define kHttpLinkUIExtsVectorKey    kHttpLinkUIStringPrefix "kHttpLinkUIExtsVectorKey"
#define kHttpLinkUIExtsInDesignKey  kHttpLinkUIStringPrefix "kHttpLinkUIExtsInDesignKey"
#define kHttpLinkUIExtsTextKey      kHttpLinkUIStringPrefix "kHttpLinkUIExtsTextKey"

// Owned by the model plug-in; duplicated so the UI can build a URI without a
// link-time dependency on it. Keep in step.
#define kHttpLnkScheme          "idhttp"
#define kHttpLnkSchemeSecure    "idhttps"

#endif // __HttpLinkUIID_h__
