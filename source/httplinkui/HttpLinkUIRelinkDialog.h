//========================================================================================
//
//  HttpLinkUIRelinkDialog.h
//
//  The seam between the "Relink from Web..." menu action and its modal dialog.
//
//  The action component decides *which* links the command applies to (right-clicked
//  row vs. panel selection) before the dialog opens; the dialog controller does the
//  relinking when OK is pressed. This stashes the targets across that gap.
//
//========================================================================================

#pragma once
#ifndef __HttpLinkUIRelinkDialog__
#define __HttpLinkUIRelinkDialog__

#include "PMString.h"
#include "UIDList.h"

/** Records the links the next "Relink from Web" dialog should act on, plus the
    URL to pre-populate its field with. Call immediately before opening the dialog.
    @param targets    [IN] the links to relink on OK
    @param initialURL [IN] display-form URL ("https://...") to seed the field with
*/
void HttpLinkUISetRelinkDialogData(const UIDList& targets, const PMString& initialURL);

#endif // __HttpLinkUIRelinkDialog__
