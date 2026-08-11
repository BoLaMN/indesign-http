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

// General includes:
#include "CObserver.h"
#include "ISubject.h"
#include "IControlView.h"
#include "IEVEUtils.h"
#include "Utils.h"

// Project includes:
#include "HttpLinkUIID.h"

/** Implementation of IObserver to respond to notification about changes
 to the Panel (kHttpLinkUIPanelWidgetBoss) subject

	@ingroup httplinkui.sdk

*/
class HttpLinkUIObserver : public CObserver
{
    typedef CObserver inherited;
    
public:
    /**
     Constructor.
     @param boss interface ptr from boss object on which this interface is aggregated.
    */
    HttpLinkUIObserver(IPMUnknown* boss);

    /**
     Destructor
     */
    virtual ~HttpLinkUIObserver() {}
    
    /**
     AutoAttach is only called for registered observers
     of widgets.  This method is called by the application
     core when the widget is shown.
     */
    virtual void AutoAttach();
    
    /**
     AutoDetach is only called for registered observers
     of widgets. Called when widget hidden.
     */
    virtual void AutoDetach();
    
    /**
     Update is called for all registered observers, and is
     the method through which changes are broadcast.
     @param theChange [IN] this is specified by the agent of change; it can be the class ID of the agent,
     or it may be some specialised message ID.
     @param theSubject [IN] provides a reference to the object which has changed; in this case, the button
     widget boss object that is being observed.
     @param protocol [IN] the protocol along which the change occurred.
     @param changedBy [IN] this can be used to provide additional information about the change or a reference
     to the boss object that caused the change.
     */
    virtual void Update(const ClassID& theChange, ISubject* theSubject, const PMIID &protocol, void* changedBy);
};

/* CREATE_PMINTERFACE
 Binds the C++ implementation class onto its
 ImplementationID making the C++ code callable by the
 application.
*/
CREATE_PMINTERFACE(HttpLinkUIObserver, kHttpLinkUIObserverImpl)

/* HttpLinkUIObserver Constructor
*/
HttpLinkUIObserver::HttpLinkUIObserver(IPMUnknown* boss)
: CObserver(boss)
{
}

/* AutoAttach
 */
void HttpLinkUIObserver::AutoAttach()
{
    InterfacePtr<IControlView> panelView(this, IID_ICONTROLVIEW);
    Utils<IEVEUtils>()->ApplyEveLayout(panelView);
    panelView->Invalidate();
    
    inherited::AutoAttach();
}

/* AutoDetach
 */
void HttpLinkUIObserver::AutoDetach()
{
   inherited::AutoDetach();
}

/* Update
 */
void HttpLinkUIObserver::Update(const ClassID &theChange, ISubject *theSubject, const PMIID &protocol, void *changedBy)
{
}
