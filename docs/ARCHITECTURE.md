# Architecture

`HttpLink` (model) does the links, HTTP, scripting and Link Info rows.
`HttpLinkUI` does the panel, drag-and-drop and the relink dialog.

Server never loads a UI plug-in, so anything functional has to live in the model
one. That constraint explains a few things below that otherwise look odd.

## The links subsystem

InDesign dispatches link resources on URI scheme. The 2026 SDK has a dedicated
HTTP subsystem on top of the generic one, which hands you caching, a download
manager, batch status and an HTTP client. Entry point is
`IHTTPLinkSubsystemObjectFactory`, registered as `kHTTPLinkResourceConnectionProvider`:

```
ILinkManager  →  kHTTPLinkResourceConnectionProvider
                   GetSchemes()          → ["idhttp", "idhttps"]
                   QueryHandler()        → ILinkResourceHandler
                   QueryStateUpdater()   → ILinkResourceStateUpdater
                   QueryHTTPLinkResourceConnection()
                   QueryHTTPLinkResourceServerAPIWrapper()
```

`IHTTPLinkResourceServerAPIWrapper` and `IHTTPLinkResourceConnection` are
mandatory; the rest have workable defaults.

| File | Does |
|---|---|
| `HttpLinkResourceFactory.cpp` | advertises schemes, vends the objects below |
| `HttpLinkServerAPIWrapper.cpp` | all the HTTP |
| `HttpLinkResourceConnection.cpp` | connection state, optional bearer token |
| `HttpLinkResourceHandler.cpp` | resource naming, https flag |
| `HttpLinkResourceStateUpdater.cpp` | thin subclass, base does the work |

## Private scheme

Links are stored as `idhttp://` / `idhttps://`, mapped to the real scheme in
`ToWireURI()` at request time.

Adobe's sample readme spells out why: scheme registration is global and
first-come, so two vendors both claiming `https` conflict and the loser's links
stop resolving. Users never see the internal form: `GetLongResourceName`
rewrites it for the panel and the scripting API exposes `remoteURL`. The stock
`link.linkResourceURI` is the one place it shows through.

## Status without downloading

A ranged `GET` (`Range: bytes=0-0`) gets us `ETag`, `Last-Modified` and
`Content-Range`. The ETag becomes the resource stamp the panel compares, which is
what drives the out-of-date badge, and why there's a **Version tag** Link Info
row, since otherwise that comparison is invisible when a link won't update.

Servers that ignore `Range` answer 200 with the whole body. Still correct, just
more bandwidth.

**Missing and unreachable are different.** `404`/`410` → `kMissing`; network
failure, `401`/`403`, `5xx` → `kInaccessible`. Collapsing them means a dropped
VPN marks every link in the document broken.

## IsConnected() returns true, always

Not laziness. The framework treats a disconnected connection as fatal:
`HTTPAssetLinkResourceStateUpdater::UpdateLinkResourceState` forces the resource
to `kInaccessible` and `HTTPAssetLinkResourceHandler` refuses to serve it.
Anonymous HTTP is sessionless (nothing to connect to or keep alive) so
reporting "not connected" fails every link before a request is even attempted.

This bit us: links came back unresolved after closing and reopening a document,
because nothing had called `Connect()` in the new session. A server that really
does reject us still gets the last word via the status probe seeing the 401.

## Panel

EVE layout. Two things worth not relearning:

- `kEllipsizeEnd` makes static text *shrinkable*. EVE collapses it to a sliver
  under pressure. Use `kDontEllipsize` plus explicit alignment.
- Panel size is cached in `~/Library/Caches/Adobe InDesign/Version 21.0/en_US/InDesign SavedData`.
  A changed default frame won't apply until that's cleared or the user resizes.

## Drag and drop

`HttpLinkUIDDTargetFlavorHelper` registers via `kLayoutDDTargHelperProviderImpl`,
which extends what the *layout* accepts rather than binding a target to our own
panel. Three things it has to get right, each a bug first:

1. **Decline drags with a source object.** The layout consults this for every
   drag including internal page-item moves, and those carry a text flavour the
   fallback matched. Claiming them made placed frames selectable but not
   draggable.
2. **Don't use `InternalizeDrag`.** Nothing in InDesign claims
   `kURLExternalFlavor`, so it always fails and the drop silently does nothing.
   The URL is read off the drag item with `GetStreamForReading()` during
   `CouldAcceptTypes()`, the only point we get the iterator, and stashed.
3. **Gate on file extension.** Dragging a link gives you the page URL, not the
   image, and placing an HTML document isn't a no-op: the probe fetches it and
   the import providers chew on it.

Logging behind `kHttpLnkDnDLogging`, off by default. Kept because all three of
those survived a round of plausible reasoning and were only found by logging.

## Link Info rows

Three service bosses under `kLinkInfoService`, one row each. `CLinkInfoProvider`
covers the defaults so each only answers what it's called and what it says.

They're in the model plug-in because the interfaces are model-side (`LinksID.h`,
not `LinksUIID.h`). All three return empty for links that aren't ours, so file
links don't gain three blank rows.

## Duplication that stays

The `http(s)` → `idhttp(s)` conversion exists in both
`httplinkui/HttpLinkUIURLUtils.h` and `httplink/HttpLinkScriptProvider.cpp`. The
model plug-in can't include a UI header, because under Server the UI plug-in isn't
there. That's the price of Server support.

It's already drifted once: the model copy compared 9 characters against the
10-character `"idhttps://"` so the passthrough never fired. **Change one, change
both.**
