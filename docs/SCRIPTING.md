# Scripting

Works the same from ExtendScript (`.jsx`), UXP (`.idjs`, UXP panels) and
InDesign Server.

That's not a coincidence. The scripting DOM is the only bridge InDesign offers.
There's no C++ ↔ UXP calling convention, so one `IScriptProvider` in the model
plug-in serves all three. Putting it in the UI plug-in would have cut out Server,
which never loads one.

## Document

### `doc.placeFromURL(url)`

Loads the place gun. The user (or the next scripted click) places it.

```js
var doc = app.documents.add();
doc.placeFromURL("https://cdn.example.com/hero.jpg");
```

A bare host is assumed `https` rather than downgraded to plaintext. Failures
raise; there's no return value.

It's on Document rather than Application so the target is explicit and it works
headless. `ILayoutUIUtils::GetFrontDocument()` would have been easier and would
have broken Server.

## Link

### `link.relinkToURL(url)`

Repoints the link and updates it.

```js
doc.links.item(0).relinkToURL("https://cdn.example.com/other.jpg");
```

The stock `Link.reinitLink(uri)` repoints *without* fetching, so the document
keeps showing the old bytes until something else triggers an update.
`relinkToURL` does both, and follows the replacement link UID InDesign hands back
when relinking creates a new link object.

### `link.isRemoteLink` → Boolean, read-only
### `link.remoteURL` → String, read-only

The real `http(s)` URL, empty for links that aren't ours. Read-only on purpose:
changing the URL is what `relinkToURL()` is for, so the relink actually happens
instead of a string being rewritten under a stale asset.

## Stock DOM

Still useful alongside:

| | |
|---|---|
| `link.linkResourceURI` | stored URI, shows the internal `idhttp(s)://` form |
| `link.status` | `LinkStatus.NORMAL`, `LINK_OUT_OF_DATE`, `LINK_MISSING`, … |
| `link.update()` | force a re-fetch |

Note `linkResourceURI` gives you the internal scheme and `remoteURL` the real one.
Prefer `remoteURL` unless you specifically want the stored form.

### Status is async

After `relinkToURL()` or `update()` the probe runs in the background, so reading
`status` immediately can catch `LINK_OUT_OF_DATE` before it settles to `NORMAL`.

```js
link.relinkToURL(url);
$.sleep(3000);
if (doc.links.item(0).status == LinkStatus.NORMAL) { /* ... */ }
```

Poll with a timeout in production rather than sleeping blind.

## Examples

Move every web link to a new CDN:

```js
var doc = app.activeDocument;
for (var i = 0; i < doc.links.length; i++) {
    var link = doc.links.item(i);
    if (!link.isRemoteLink) continue;
    var moved = link.remoteURL.replace("//old-cdn.example.com/",
                                       "//new-cdn.example.com/");
    if (moved != link.remoteURL) link.relinkToURL(moved);
}
```

Place a batch:

```js
var urls = ["https://cdn.example.com/a.jpg", "https://cdn.example.com/b.jpg"];
var doc = app.documents.add();
for (var i = 0; i < urls.length; i++) {
    doc.placeFromURL(urls[i]);
    doc.pages.item(0).place();
}
```

## Server

Nothing extra to do. The model plug-in declares `kInDesignServerProduct` and
links `InDesignModel.framework`, so the same binary loads and the whole API above
is there.

Two things to remember headless: nothing prompts, failures raise; and the status
probe blocks the calling thread up to 4s per unreachable origin, which matters
when a render slot is held open.
