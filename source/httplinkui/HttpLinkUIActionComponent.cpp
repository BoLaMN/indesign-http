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

#include "VCPlugInHeaders.h"

// Interface includes:
#include "IActionStateList.h"
#include "IActiveContext.h"
#include "IApplication.h"
#include "IDialog.h"
#include "IDialogMgr.h"
#include "ILink.h"
#include "ILinkFacade.h"
#include "ILinkResource.h"
#include "ILinksUIUtils.h"
#include "ISession.h"

// General includes:
#include "CActionComponent.h"
#include "CAlert.h"
#include "CoreResTypes.h"
#include "LocaleSetting.h"
#include "RsrcSpec.h"
#include "UIDList.h"
#include "URI.h"

// Project includes:
#include "HttpLinkUIID.h"
#include "HttpLinkUIRelinkDialog.h"



/** Implements IActionComponent;
    performs the actions that are executed when the plug-in's
	menu items are selected.

	@ingroup httplinkui.sdk
*/
class HttpLinkUIActionComponent : public CActionComponent
{
public:
/**
 Constructor.
 @param boss interface ptr from boss object on which this interface is aggregated.
 */
		HttpLinkUIActionComponent(IPMUnknown* boss);

		/** The action component should perform the requested action.
			This is where the menu item's action is taken.
			When a menu item is selected, the Menu Manager determines
			which plug-in is responsible for it, and calls its DoAction
			with the ID for the menu item chosen.

            @param ac active context
			@param actionID identifies the menu item that was selected.
			@param mousePoint contains the global mouse location at time of event causing action (e.g. context menus). kInvalidMousePoint if not relevant.
			@param widget contains the widget that invoked this action. May be nil. 
			*/
		virtual void DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint, IPMUnknown* widget);
        virtual void UpdateActionStates(IActiveContext* ac, IActionStateList *listToUpdate, GSysPoint mousePoint, IPMUnknown* widget);

	private:
		/** Encapsulates functionality for the about menu item. */
		void DoAbout();

		/** Relinks the link(s) selected in the Links panel to a web URL. */
		void DoRelinkFromWeb(IPMUnknown* widget);

		/** The links this menu action applies to: the row that was right-clicked
		    if there is one, otherwise the panel selection. */
		UIDList GetLinksForMenuAction(IPMUnknown* widget) const;

		/** The URL to seed the dialog's field with: the first target link's
		    current URL if it is already one of ours, otherwise "https://". */
		PMString GetInitialURLForDialog(const UIDList& targetLinks) const;
};


/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(HttpLinkUIActionComponent, kHttpLinkUIActionComponentImpl)

/* HttpLinkUIActionComponent Constructor
*/
HttpLinkUIActionComponent::HttpLinkUIActionComponent(IPMUnknown* boss)
: CActionComponent(boss)
{
}

/* DoAction
*/
void HttpLinkUIActionComponent::DoAction(IActiveContext* ac, ActionID actionID, GSysPoint mousePoint, IPMUnknown* widget)
{
    switch (actionID.Get())
	{
        case kHttpLinkUIRelinkFromWebActionID:
        {
            this->DoRelinkFromWeb(widget);
            break;
        }

        // Show about dialog
		case kHttpLinkUIPopupAboutThisActionID:
		case kHttpLinkUIAboutActionID:
		{
			this->DoAbout();
			break;
		}
	}
}


void HttpLinkUIActionComponent::UpdateActionStates(IActiveContext* ac, IActionStateList *listToUpdate, GSysPoint mousePoint, IPMUnknown* widget)
{
    const int32 nCount = listToUpdate->Length();
    for (int32 i = 0; i < nCount; i++)
    {
        if (listToUpdate->GetNthAction(i).Get() != kHttpLinkUIRelinkFromWebActionID)
            continue;

        // Enabled only when something relinkable is actually selected, so the
        // item greys out rather than failing after the fact.
        UIDList targetLinks = GetLinksForMenuAction(widget);
        for (int32 n = targetLinks.Length() - 1; n >= 0; --n)
        {
            if (Utils<Facade::ILinkFacade>()->CanRelinkLink(targetLinks.GetRef(n)))
            {
                listToUpdate->SetNthActionState(i, kEnabledAction);
                break;
            }
        }
    }
}

/* GetLinksForMenuAction
*/
UIDList HttpLinkUIActionComponent::GetLinksForMenuAction(IPMUnknown* widget) const
{
    InterfacePtr<const ILink> theLink(Utils<ILinksUIUtils>()->QueryLinkFromLinksUIWidget(widget));
    if (theLink == nil)
        return Utils<ILinksUIUtils>()->GetLinksSelectedInPanel();
    return UIDList(::GetUIDRef(theLink));
}

/* GetInitialURLForDialog
*/
PMString HttpLinkUIActionComponent::GetInitialURLForDialog(const UIDList& targetLinks) const
{
    PMString seed("https://");
    seed.SetTranslatable(kFalse);
    if (targetLinks.Length() == 0)
        return seed;

    const UIDRef linkRef = targetLinks.GetRef(0);
    InterfacePtr<const ILink> link(linkRef, UseDefaultIID());
    if (link == nil)
        return seed;
    InterfacePtr<const ILinkResource> resource(
        UIDRef(linkRef.GetDataBase(), link->GetResource()), UseDefaultIID());
    if (resource == nil)
        return seed;

    // Only our own idhttp/idhttps links round-trip; a file link's path is no
    // use as a URL seed.
    const URI uri = resource->GetURI();
    const WideString scheme(uri.GetComponent(URI::kScheme));
    if (scheme != WideString(kHttpLnkScheme) && scheme != WideString(kHttpLnkSchemeSecure))
        return seed;

    URI display(uri);
    display.SetComponent(URI::kScheme,
        (scheme == WideString(kHttpLnkSchemeSecure)) ? kHTTPSAssetURIScheme : kHTTPAssetURIScheme);
    seed.SetUTF8String(display.GetURI());
    seed.SetTranslatable(kFalse);
    return seed;
}

/* DoRelinkFromWeb
*/
void HttpLinkUIActionComponent::DoRelinkFromWeb(IPMUnknown* widget)
{
    UIDList targetLinks = GetLinksForMenuAction(widget);
    if (targetLinks.Length() == 0)
    {
        CAlert::InformationAlert(PMString("Select one or more links first."));
        return;
    }

    do
    {
        InterfacePtr<IApplication> application(GetExecutionContextSession()->QueryApplication());
        if (application == nil)
        {
            ASSERT_FAIL("HttpLinkUIActionComponent::DoRelinkFromWeb: application invalid");
            break;
        }
        InterfacePtr<IDialogMgr> dialogMgr(application, UseDefaultIID());
        if (dialogMgr == nil)
        {
            ASSERT_FAIL("HttpLinkUIActionComponent::DoRelinkFromWeb: dialogMgr invalid");
            break;
        }

        RsrcSpec dialogSpec
        (
            LocaleSetting::GetLocale(),  // Locale index from PMLocaleIDs.h.
            kHttpLinkUIPluginID,         // Our Plug-in ID.
            kViewRsrcType,               // This is the kViewRsrcType.
            kSDKDefDialogResourceID,     // Resource ID for our dialog.
            kTrue                        // Initially visible.
        );

        IDialog* dialog = dialogMgr->CreateNewDialog(dialogSpec, IDialog::kMovableModal);
        if (dialog == nil)
        {
            ASSERT_FAIL("HttpLinkUIActionComponent::DoRelinkFromWeb: can't create dialog");
            break;
        }

        // The controller does the relinking on OK; hand it the targets first.
        HttpLinkUISetRelinkDialogData(targetLinks, GetInitialURLForDialog(targetLinks));
        dialog->Open();
    } while (false);
}


/* DoAbout
*/
void HttpLinkUIActionComponent::DoAbout()
{
	CAlert::ModalAlert
	(
		kHttpLinkUIAboutBoxStringKey,				// Alert string
		kOKString, 						// OK button
		kNullString, 						// No second button
		kNullString, 						// No third button
		1,							// Set OK button to default
		CAlert::eInformationIcon				// Information icon.
	);
}

