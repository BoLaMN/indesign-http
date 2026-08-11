# HttpLink

InDesign plug-ins that let you use `http://` and `https://` URLs as link paths.
Place a remote image, keep it in the Links panel, update it when the server copy
changes, export it at full resolution.

InDesign has supported non-file links since CS5 (the Links subsystem dispatches
on URI scheme) but Adobe only ships a handler for `file:`. This adds one for the
web.

Working on macOS, InDesign 21.5.1. Never compiled on Windows yet.

## Features

- **Web Links panel**: paste a URL, hit Place, draw the frame
- **Drag from a browser** onto the page
- **Relink from Web…** on the Links panel flyout and right-click menu
- Out-of-date detection from `ETag` / `Last-Modified`
- Extra Link Info rows: remote URL, status, version tag
- Scripting from ExtendScript, UXP and InDesign Server
- Full-resolution PDF/JPEG export

Links survive save and reopen: the URL is stored in the document and re-fetched.

## Install

Build (see [BUILD.md](BUILD.md)), then point InDesign at the output:

```bash
echo '=Path "<repo>/build/mac/release_cocoa64/SDK"' \
  > ~/Library/Preferences/Adobe\ InDesign/Version\ 21.0/en_US/PluginConfig.txt
```

Restart and check Help ▸ About Plug-ins.

## Scripting

Same code from `.jsx`, `.idjs` and Server:

```js
doc.placeFromURL("https://cdn.example.com/hero.jpg");

var link = doc.links.item(0);
link.relinkToURL("https://cdn.example.com/other.jpg");
link.isRemoteLink   // read-only
link.remoteURL      // read-only
```

More in [docs/SCRIPTING.md](docs/SCRIPTING.md).

## Notes

Links are stored internally as `idhttp://` / `idhttps://` and mapped to the real
scheme when a request goes out. Claiming bare `http`/`https` isn't safe. Scheme
registration is global and first-come, so two plug-ins doing it would conflict.
You never see the internal form; the Links panel and scripting API both show the
real URL.

The **Link Info rows are off by default**. Links panel flyout ▸ Panel Options…,
then tick them. Until you do they just don't appear, which looks like the plug-in
failing to register them.

## Layout

```
source/httplink/     model plug-in: links, HTTP, scripting, Link Info
source/httplinkui/   UI plug-in: panel, drag-drop, relink dialog
build/mac/           Xcode projects + build.sh
build/win/           generated VS projects + genproj.py
sdk/                 InDesign SDK, not committed (see BUILD.md)
```

Server never loads a UI plug-in, so everything functional lives in the model one.

## Known gaps

- Windows is generated but never compiled; expect real errors first time
- Plug-in prefix IDs are placeholders; shipping needs numbers from Adobe
  Developer Support, which has lead time
- Japanese strings are still `[JP]` placeholders from the SDK sample
- No auth UI. A bearer token can be set programmatically; public URLs are the
  tested path
- The status probe blocks the main thread, capped at 4s per unreachable origin

## Docs

[BUILD.md](BUILD.md) · [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · [docs/SCRIPTING.md](docs/SCRIPTING.md)

## Licence

Derived from the `CustomHttpLink` / `CustomHttpLinkUI` samples in the Adobe
InDesign SDK, under the Adobe SDK licence. The SDK is not redistributed here.
