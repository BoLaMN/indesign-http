//========================================================================================
//
//  HttpLinkUIRelinkDialogController.cpp
//
//  IDialogController for the "Relink from Web" modal dialog.
//
//  The action component stashes the target links and a seed URL via
//  HttpLinkUISetRelinkDialogData() and opens the dialog; this controller seeds the
//  field (InitializeDialogFields), refuses to close on a URL we cannot place
//  (ValidateDialogFields), and performs the relink on OK (ApplyDialogFields).
//
//========================================================================================

#include "VCPlugInHeaders.h"

// Interface includes:
#include "ILinkFacade.h"

// General includes:
#include "CAlert.h"
#include "CDialogController.h"
#include "UIDList.h"
#include "URI.h"
#include "Utils.h"

// Project includes:
#include "HttpLinkUIID.h"
#include "HttpLinkUIRelinkDialog.h"
#include "HttpLinkUIURLUtils.h"

// UI runs on the app's main thread only, so plain statics carry the state from
// the menu action into the (modal) dialog safely.
static UIDList  gRelinkTargets;
static PMString gRelinkInitialURL;

void HttpLinkUISetRelinkDialogData(const UIDList& targets, const PMString& initialURL)
{
    gRelinkTargets    = targets;
    gRelinkInitialURL = initialURL;
}

/** Implements IDialogController for the Relink from Web dialog.

    @ingroup httplinkui.sdk
*/
class HttpLinkUIRelinkDialogController : public CDialogController
{
public:
    /** Constructor.
        @param boss interface ptr from boss object on which this interface is aggregated.
    */
    HttpLinkUIRelinkDialogController(IPMUnknown* boss) : CDialogController(boss) {}

    /** Seeds the URL field with the selected link's current URL (or "https://"). */
    virtual void InitializeDialogFields(IActiveContext* dlgContext);

    /** Keeps the dialog open until the field holds a URL we can place. */
    virtual WidgetID ValidateDialogFields(IActiveContext* myContext);

    /** Relinks the stashed target links to the typed URL. */
    virtual void ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(HttpLinkUIRelinkDialogController, kHttpLinkUIRelinkDialogControllerImpl)

/* InitializeDialogFields
*/
void HttpLinkUIRelinkDialogController::InitializeDialogFields(IActiveContext* dlgContext)
{
    CDialogController::InitializeDialogFields(dlgContext);

    PMString seed = gRelinkInitialURL;
    if (seed.IsEmpty())
        seed.SetCString("https://");
    seed.SetTranslatable(kFalse);
    this->SetTextControlData(kHttpLinkUIRelinkURLEditWidgetID, seed);
}

/* ValidateDialogFields
*/
WidgetID HttpLinkUIRelinkDialogController::ValidateDialogFields(IActiveContext* myContext)
{
    #pragma unused(myContext)
    const PMString url = this->GetTextControlData(kHttpLinkUIRelinkURLEditWidgetID);
    URI ignored;
    if (!HttpLinkUIBuildResourceURI(url, ignored))
    {
        CAlert::InformationAlert(PMString("Enter an http:// or https:// URL."));
        return kHttpLinkUIRelinkURLEditWidgetID;
    }
    return kNoInvalidWidgets;
}

/* ApplyDialogFields
*/
void HttpLinkUIRelinkDialogController::ApplyDialogFields(IActiveContext* myContext, const WidgetID& widgetId)
{
    #pragma unused(myContext, widgetId)

    const PMString url = this->GetTextControlData(kHttpLinkUIRelinkURLEditWidgetID);
    URI resourceURI;
    if (!HttpLinkUIBuildResourceURI(url, resourceURI))
        return; // Validate already vetoed this; belt and braces.

    for (int32 i = 0; i < gRelinkTargets.Length(); ++i)
    {
        const UIDRef linkRef = gRelinkTargets.GetRef(i);
        if (!Utils<Facade::ILinkFacade>()->CanRelinkLink(linkRef))
            continue;

        UID newLinkUID(kInvalidUID);
        ErrorCode err = Utils<Facade::ILinkFacade>()->RelinkLink(linkRef, resourceURI, kMinimalUI, newLinkUID);
        if (err == kSuccess)
        {
            // Relink alone repoints without fetching; update so the user sees the
            // new asset rather than a stale one. Follow the replacement UID if
            // relink minted a new link object.
            const UIDRef updatedRef = (newLinkUID != kInvalidUID)
                ? UIDRef(linkRef.GetDataBase(), newLinkUID) : linkRef;
            UID afterUID(kInvalidUID);
            Utils<Facade::ILinkFacade>()->UpdateLink(updatedRef, true, kMinimalUI, afterUID);
        }
    }
}

// End, HttpLinkUIRelinkDialogController.cpp.
