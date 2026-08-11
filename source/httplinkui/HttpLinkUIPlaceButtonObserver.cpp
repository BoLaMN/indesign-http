//========================================================================================
//
//  HttpLinkUIPlaceButtonObserver.cpp
//
//  Watches the panel's Place button. On click it reads the URL the user typed,
//  rewrites it onto our private scheme, and loads the place gun -- so placing a
//  remote asset feels exactly like File > Place, cursor and all.
//
//  Derived from the CustomHttpLink SDK sample, which offered a dropdown of
//  filenames on one hardcoded host; this takes an arbitrary URL instead.
//
//========================================================================================

#include "VCPlugInHeaders.h"

#include "CObserver.h"
#include "CAlert.h"
#include "ErrorUtils.h"
#include "EventUtilities.h"
#include "ILayoutUIUtils.h"
#include "ILinkFacade.h"
#include "IPasteboardUtils.h"
#include "IActiveContext.h"
#include "IControlView.h"
#include "IImportExportFacade.h"
#include "IPanelControlData.h"
#include "ISpread.h"
#include "ISubject.h"
#include "ITextControlData.h"
#include "IWidgetParent.h"
#include "PMString.h"
#include "URI.h"

#include "HttpLinkUIID.h"
#include "HttpLinkUIURLUtils.h"

/** Responds to clicks on the panel's Place button.
    @ingroup httplinkui.sdk
*/
class HttpLinkUIPlaceButtonObserver : public CObserver
{
public:
    HttpLinkUIPlaceButtonObserver(IPMUnknown* boss);
    virtual ~HttpLinkUIPlaceButtonObserver() {}

    virtual void AutoAttach();
    virtual void AutoDetach();
    virtual void Update(const ClassID& theChange, ISubject* theSubject,
                        const PMIID& protocol, void* changedBy);

private:
};

CREATE_PMINTERFACE(HttpLinkUIPlaceButtonObserver, kHttpLinkUIPlaceButtonObserverImpl)

HttpLinkUIPlaceButtonObserver::HttpLinkUIPlaceButtonObserver(IPMUnknown* boss)
: CObserver(boss)
{
}

void HttpLinkUIPlaceButtonObserver::AutoAttach()
{
    InterfacePtr<ISubject> subject(this, IID_ISUBJECT);
    if (subject && !subject->IsAttached(this, IID_IBOOLEANCONTROLDATA))
        subject->AttachObserver(this, IID_IBOOLEANCONTROLDATA);
}

void HttpLinkUIPlaceButtonObserver::AutoDetach()
{
    InterfacePtr<ISubject> subject(this, IID_ISUBJECT);
    if (subject && subject->IsAttached(this, IID_IBOOLEANCONTROLDATA))
        subject->DetachObserver(this, IID_IBOOLEANCONTROLDATA);
}

//========================================================================================
// HttpLinkUIPlaceButtonObserver::Update
//========================================================================================
void HttpLinkUIPlaceButtonObserver::Update(const ClassID& theChange, ISubject* theSubject,
                                           const PMIID& protocol, void* changedBy)
{
    if (theChange != kTrueStateMessage)
        return;

    InterfacePtr<IControlView> view(this, UseDefaultIID());
    if (view == nil || view->GetWidgetID() != kPlaceURLButtonWidgetID)
        return;

    // Read the URL out of the sibling edit box.
    InterfacePtr<IWidgetParent> parentHolder(this, UseDefaultIID());
    if (parentHolder == nil)
        return;
    InterfacePtr<IPanelControlData> panelData(parentHolder->GetParent(), UseDefaultIID());
    if (panelData == nil)
        return;

    IControlView* urlField = panelData->FindWidget(kURLEditWidgetID);
    InterfacePtr<ITextControlData> urlText(urlField, UseDefaultIID());
    if (urlText == nil)
        return;

    URI resourceURI;
    if (!HttpLinkUIBuildResourceURI(urlText->GetString(), resourceURI))
    {
        CAlert::InformationAlert(PMString("Enter an http:// or https:// URL to place."));
        return;
    }

    // Work out where the place gun should drop.
    IActiveContext* ac = GetExecutionContextSession()->GetActiveContext();
    IControlView* layoutView = ac ? ac->GetContextView() : nil;
    if (layoutView == nil)
    {
        CAlert::InformationAlert(PMString("Open a document before placing a URL."));
        return;
    }

    const GSysPoint globalPoint = ::GetGlobalMouseLocation();
    const PMPoint converted = Utils<ILayoutUIUtils>()->ComputePasteboardPoint(globalPoint, layoutView);
    InterfacePtr<ISpread> spread(Utils<IPasteboardUtils>()->QueryNearestSpread(layoutView, converted));
    if (spread == nil)
        return;

    IDataBase* db = ::GetDataBase(spread);

    // Full-resolution originals: we deliberately do not request FPO renditions
    // the way the SDK sample does, because a generic web server has only the one
    // asset -- there is no proxy to fall back to.
    Utils<Facade::ILinkFacade>()->SetHTTPLinksDefaultRenditionType(ILinkResourceRenditionData::kOriginal);

    const ErrorCode err = Utils<Facade::IImportExportFacade>()->ImportAndLoadPlaceGun(
        db, resourceURI, kMinimalUI, kFalse, kFalse, kFalse,
        UID(kInvalidUID), IPlaceGun::kAddToFront);

    if (err != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(err);
}
