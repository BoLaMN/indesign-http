//========================================================================================
//
//  HttpLinkUIDDTargetFlavorHelper.cpp
//
//  Drag a URL out of a browser onto the layout.
//
//  Registered through kLayoutDDTargHelperProviderImpl, which extends what the
//  layout accepts rather than binding a target to one of our own panels. A
//  successful drop loads the place gun, so you draw the frame yourself.
//
//========================================================================================

#include "VCPlugInHeaders.h"

#include "CDragDropTargetFlavorHelper.h"
#include "CAlert.h"
#include "CmdUtils.h"
#include "DataObjectIterator.h"
#include "ErrorUtils.h"
#include "ICommand.h"
#include "IControlView.h"
#include "IDocument.h"
#include "IImportExportFacade.h"
#include "ILayoutCmdData.h"
#include "ILayoutControlData.h"
#include "ILayoutUIUtils.h"
#include "ILinkFacade.h"
#include "IPasteboardUtils.h"
#include "IPlaceGun.h"
#include "ISpread.h"
#include "IPMDataObject.h"
#include "IPMStream.h"
#include "PMFlavorTypes.h"
#include <cstdarg>
#include "PMString.h"
#include "URI.h"
#include "StringUtils.h"
#include "Utils.h"

#include "HttpLinkUIID.h"
#include "HttpLinkUIURLUtils.h"

#include <cstdio>

// ---------------------------------------------------------------------------
// Drag-and-drop diagnostics.
//
// Drags can't be scripted, so this is the only way to see why a drop misbehaved.
// One line per drag query to /tmp/httplink-dnd.log. Off by default.
// ---------------------------------------------------------------------------
#define kHttpLnkDnDLogging 0

#if kHttpLnkDnDLogging
static void DnDLog(const char* fmt, ...)
{
    FILE* f = std::fopen("/tmp/httplink-dnd.log", "a");
    if (f == nil)
        return;
    va_list args;
    va_start(args, fmt);
    std::vfprintf(f, fmt, args);
    va_end(args);
    std::fputc('\n', f);
    std::fclose(f);
}
#else
static void DnDLog(const char*, ...) {}
#endif

/** Extends the layout's drop handling to accept URLs dragged in from outside.
    @ingroup httplinkui.sdk
*/
class HttpLinkUIDDTargetFlavorHelper : public CDragDropTargetFlavorHelper
{
public:
    HttpLinkUIDDTargetFlavorHelper(IPMUnknown* boss);
    virtual ~HttpLinkUIDDTargetFlavorHelper();

    virtual DragDrop::TargetResponse CouldAcceptTypes(const IDragDropTarget* target,
                                                      DataObjectIterator* dataIter,
                                                      const IDragDropSource* fromSource,
                                                      const IDragDropController* controller) const;

    virtual ErrorCode ProcessDragDropCommand(IDragDropTarget* target,
                                             IDragDropController* controller,
                                             DragDrop::eCommandType action);

    DECLARE_HELPER_METHODS()

private:
    /** Browsers advertise a dragged link in more than one way, and which ones
        appear depends on the browser and on whether you grabbed a link or an
        image. Try the dedicated URL flavour first and fall back to plain text,
        which is the one every browser provides.
    */
    static bool16 FindUsableFlavor(DataObjectIterator* dataIter,
                                   ExternalPMFlavor& outExternal,
                                   PMFlavor& outInternal);


    /** True if the URL's path ends in something InDesign can actually place.

        Dropping a page URL rather than an asset URL is easy to do by accident --
        dragging a link gives you the page, not the picture -- and trying to place
        an HTML document is not a no-op: the status probe fetches it, the import
        providers reject it, and the user gets a stalled application for their
        trouble. Declining up front keeps the failure cheap and lets InDesign's own
        drop handling have the drag instead.
    */
    static bool16 LooksPlaceable(const std::string& path);

    /** Reads the raw bytes of a flavour straight off a drag data object. */
    static bool16 ReadFlavorAsString(IPMDataObject* item, ExternalPMFlavor flavor, PMString& out);

    /** The URL seen during drag tracking.
        InternalizeDrag cannot help us here: nothing in InDesign claims
        kURLExternalFlavor, so every conversion attempt failed and the drop did
        nothing. Instead we read the URL directly from the drag data object while
        we still have the iterator -- CouldAcceptTypes is the only place it is
        handed to us -- and use it when the drop arrives. Mutable because
        CouldAcceptTypes is const by contract.
    */
    mutable PMString fPendingURL;
};

CREATE_PMINTERFACE(HttpLinkUIDDTargetFlavorHelper, kHttpLinkUIDDTargetFlavorHelperImpl)
DEFINE_HELPER_METHODS(HttpLinkUIDDTargetFlavorHelper)

HttpLinkUIDDTargetFlavorHelper::HttpLinkUIDDTargetFlavorHelper(IPMUnknown* boss)
: CDragDropTargetFlavorHelper(boss), HELPER_METHODS_INIT(boss)
{
}

HttpLinkUIDDTargetFlavorHelper::~HttpLinkUIDDTargetFlavorHelper()
{
}

//========================================================================================
// HttpLinkUIDDTargetFlavorHelper::FindUsableFlavor
//========================================================================================
bool16 HttpLinkUIDDTargetFlavorHelper::FindUsableFlavor(DataObjectIterator* dataIter,
                                                        ExternalPMFlavor& outExternal,
                                                        PMFlavor& outInternal)
{
    if (dataIter == nil)
        return kFalse;

    // Ordered best-first: a real URL flavour carries exactly the link, whereas
    // text might be anything the user happened to drag.
    const ExternalPMFlavor externals[] = {
        kURLExternalFlavor,
        kUNICODETEXTExternalFlavor,
        kTEXTExternalFlavor,
    };
    const PMFlavor internals[] = {
        kPMURLFlavor,
        kTEXTFlavor,
        kTEXTFlavor,
    };

    for (int i = 0; i < 3; ++i)
    {
        const DataExchangeResponse response =
            dataIter->FlavorExistsWithPriorityInAllObjects(externals[i]);
        if (response.CanDo())
        {
            outExternal = externals[i];
            outInternal = internals[i];
            return kTrue;
        }
    }
    return kFalse;
}

//========================================================================================
// HttpLinkUIDDTargetFlavorHelper::LooksPlaceable
//========================================================================================
bool16 HttpLinkUIDDTargetFlavorHelper::LooksPlaceable(const std::string& path)
{
    const std::string::size_type dot = path.rfind('.');
    if (dot == std::string::npos || dot + 1 >= path.length())
        return kFalse;

    std::string ext = path.substr(dot + 1);
    for (std::string::iterator c = ext.begin(); c != ext.end(); ++c)
        *c = static_cast<char>(tolower(static_cast<unsigned char>(*c)));
    if (ext.empty())
        return kFalse;

    // Parsed from the same macros the panel displays (HttpLinkUIID.h), so the
    // list shown to the user is by construction the list actually enforced.
    static const char* kGroups[] = {
        kHttpLnkExtsRaster,
        kHttpLnkExtsVector,
        kHttpLnkExtsInDesign,
        kHttpLnkExtsText,
        nil
    };

    const std::string needle = " " + ext + " ";
    for (int i = 0; kGroups[i] != nil; ++i)
    {
        const std::string haystack = std::string(" ") + kGroups[i] + " ";
        if (haystack.find(needle) != std::string::npos)
            return kTrue;
    }
    return kFalse;
}

//========================================================================================
// HttpLinkUIDDTargetFlavorHelper::ReadFlavorAsString
//========================================================================================
bool16 HttpLinkUIDDTargetFlavorHelper::ReadFlavorAsString(IPMDataObject* item,
                                                          ExternalPMFlavor flavor,
                                                          PMString& out)
{
    if (item == nil || !item->FlavorExists(flavor))
        return kFalse;

    const uint32 size = item->GetSizeOfFlavorData(flavor);
    if (size == 0 || size > 64 * 1024)   // a URL is never this large; refuse junk
        return kFalse;

    IPMStream* stream = item->GetStreamForReading(flavor);
    if (stream == nil)
        return kFalse;

    std::string buffer(size, '\0');
    const int32 read = stream->XferByte(reinterpret_cast<uchar*>(&buffer[0]),
                                        static_cast<int32>(size));
    item->FinishedWithStream(stream);
    if (read <= 0)
        return kFalse;
    buffer.resize(static_cast<size_t>(read));

    // Some sources hand over UTF-16; a NUL in the second byte is the giveaway.
    if (buffer.size() >= 2 && buffer[1] == '\0')
    {
        std::string narrowed;
        narrowed.reserve(buffer.size() / 2);
        for (size_t i = 0; i + 1 < buffer.size(); i += 2)
            narrowed.push_back(buffer[i]);
        buffer.swap(narrowed);
    }

    out.SetUTF8String(buffer);
    return !buffer.empty();
}

//========================================================================================
// HttpLinkUIDDTargetFlavorHelper::CouldAcceptTypes
//========================================================================================
DragDrop::TargetResponse
HttpLinkUIDDTargetFlavorHelper::CouldAcceptTypes(const IDragDropTarget* target,
                                                 DataObjectIterator* dataIter,
                                                 const IDragDropSource* fromSource,
                                                 const IDragDropController* controller) const
{
    // Report what actually arrived, so a failing drop can be diagnosed without
    // having to guess at which flavours the source application offered.
    if (dataIter != nil)
    {
        DnDLog("CouldAcceptTypes: fromSource=%s url=%d unicode=%d text=%d",
               fromSource ? "internal" : "external(nil)",
               (int)dataIter->FlavorExistsWithPriorityInAllObjects(kURLExternalFlavor).CanDo(),
               (int)dataIter->FlavorExistsWithPriorityInAllObjects(kUNICODETEXTExternalFlavor).CanDo(),
               (int)dataIter->FlavorExistsWithPriorityInAllObjects(kTEXTExternalFlavor).CanDo());
    }
    else
    {
        DnDLog("CouldAcceptTypes: fromSource=%s dataIter=nil",
               fromSource ? "internal" : "external(nil)");
    }

    // Only ever claim drags that originated OUTSIDE InDesign.
    //
    // This helper is consulted for every drag over the layout, including
    // InDesign's own internal page-item drags. Those advertise a text flavour
    // among others, which our text fallback below happily matched -- so we were
    // claiming internal drags and silently swallowing them. The visible symptom
    // was that a placed frame could be selected but not dragged: selection is
    // not a drag operation, so it still worked, while the move was intercepted.
    //
    // An in-process drag has a source object; a drag from another application
    // does not. Declining here hands internal drags straight back to InDesign.
    if (fromSource != nil)
    {
        DnDLog("  -> declined: internal drag");
        return DragDrop::kWontAcceptTargetResponse;
    }

    ExternalPMFlavor external;
    PMFlavor internal;
    if (FindUsableFlavor(dataIter, external, internal))
    {
        // Grab the URL now, while we still have the iterator.
        fPendingURL.Clear();
        for (IPMDataObject* item = dataIter->First(); item != nil; item = dataIter->Next())
        {
            if (ReadFlavorAsString(item, external, fPendingURL))
                break;
        }
        DnDLog("  -> ACCEPTED, captured=[%s]", fPendingURL.GetUTF8String().c_str());
        const DataExchangeResponse response =
            dataIter->FlavorExistsWithPriorityInAllObjects(external);
        return DragDrop::TargetResponse(response, DragDrop::kDropWillCopy);
    }
    DnDLog("  -> declined: no usable flavour");
    return DragDrop::kWontAcceptTargetResponse;
}

//========================================================================================
// HttpLinkUIDDTargetFlavorHelper::ProcessDragDropCommand
//========================================================================================
ErrorCode HttpLinkUIDDTargetFlavorHelper::ProcessDragDropCommand(IDragDropTarget* target,
                                                                 IDragDropController* controller,
                                                                 DragDrop::eCommandType action)
{
    DnDLog("ProcessDragDropCommand: action=%d", (int)action);
    if (action != DragDrop::kDropCommand)
        return kFailure;

    ErrorCode stat = kFailure;
    CmdUtils::SequencePtr sequence;
    sequence->SetName(PMString("Place dropped URL", PMString::kUnknownEncoding));

    do
    {
        InterfacePtr<IControlView> layoutView(target, UseDefaultIID());
        InterfacePtr<ILayoutControlData> layoutData(target, UseDefaultIID());
        if (layoutView == nil || layoutData == nil)
            break;

        // Work out which spread was dropped on, and switch to it if the drop
        // landed on a spread other than the active one.
        const GSysPoint where = controller->GetDragMouseLocation();
        const PMPoint currentPoint = Utils<ILayoutUIUtils>()->GlobalToPasteboard(layoutView, where);
        InterfacePtr<ISpread> targetSpread(Utils<IPasteboardUtils>()->QueryNearestSpread(layoutView, currentPoint));
        if (targetSpread == nil)
            break;

        const UIDRef targetSpreadUIDRef = ::GetUIDRef(targetSpread);
        if (targetSpreadUIDRef != layoutData->GetSpreadRef())
        {
            InterfacePtr<ICommand> setSpreadCmd(CmdUtils::CreateCommand(kSetSpreadCmdBoss));
            InterfacePtr<ILayoutCmdData> setSpreadLayoutCmdData(setSpreadCmd, UseDefaultIID());
            if (setSpreadCmd == nil || setSpreadLayoutCmdData == nil)
                break;
            setSpreadLayoutCmdData->Set(::GetUIDRef(layoutData->GetDocument()), layoutData);
            setSpreadCmd->SetItemList(UIDList(targetSpreadUIDRef));
            if (CmdUtils::ProcessCommand(setSpreadCmd) != kSuccess)
                break;
        }

        // Use the URL captured during tracking. InternalizeDrag is deliberately
        // not used: no InDesign handler claims kURLExternalFlavor, so it always
        // failed and the drop silently did nothing.
        PMString payload = fPendingURL;
        if (payload.IsEmpty())
        {
            // Fallback: the drag item is still reachable through the controller.
            InterfacePtr<IPMDataObject> item(controller->GetDragItem(0));
            if (item != nil)
                ReadFlavorAsString(item, kURLExternalFlavor, payload);
        }
        DnDLog("  payload=[%s]", payload.GetUTF8String().c_str());

        URI resourceURI;
        if (!HttpLinkUIBuildResourceURI(payload, resourceURI))
        {
            DnDLog("  -> not an http(s) URL, declining");
            break;
        }

        {
            std::string probePath;
            StringUtils::ConvertWideStringToUTF8(resourceURI.GetComponent(URI::kPath), probePath);
            if (!LooksPlaceable(probePath))
            {
                DnDLog("  -> path '%s' is not a placeable asset, declining", probePath.c_str());
                break;
            }
        }

        IDataBase* db = ::GetDataBase(targetSpread);
        UIDRef docUIDRef(db, db->GetRootUID());

        Utils<Facade::ILinkFacade>()->SetHTTPLinksDefaultRenditionType(ILinkResourceRenditionData::kOriginal);

        // Load the place gun instead of dropping immediately, so the user draws
        // the frame -- which is what makes this feel like a normal Place.
        stat = Utils<Facade::IImportExportFacade>()->ImportAndLoadPlaceGun(
            db, resourceURI, kMinimalUI, kFalse, kFalse, kFalse,
            UID(kInvalidUID), IPlaceGun::kAddToFront);

    } while (false);

    if (stat != kSuccess)
        ErrorUtils::PMSetGlobalErrorCode(stat);

    return stat;
}
