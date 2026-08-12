# Building

```bash
build/mac/build.sh            # both plug-ins, Release + Debug
build/mac/build.sh Release    # or one config
```

Use the script, not bare `xcodebuild`. See [Why the script](#why-the-script).

## You need

- Xcode (26.6 works; the SDK docs ask for 16.2 but nothing is pinned)
- The InDesign 2026 Plug-in SDK, unzipped to `<repo>/sdk`
- InDesign 21.4 or newer to load the result

The SDK is licence-restricted and about 4 GB, so it isn't committed. Get it from
<https://developer.adobe.com/console/downloads>. `server_sdk/` is optional; it's
client-side sample code and isn't needed to build.

If you keep the SDK elsewhere, change `ID_SDK_ROOT` in
`build/mac/prj/_shared_build_settings/plugin.sdk.xcconfig`.

## Output

`build/mac/release_cocoa64/SDK/*.InDesignPlugin`, universal (arm64 + x86_64).
Debug plug-ins only load into a debug InDesign.

## Why the script

The two projects share one output directory, which makes per-project `xcodebuild`
calls quietly destructive:

- `clean` on either deletes the other's bundle. InDesign then starts fine and the
  panel appears, but with the model plug-in gone nothing handles the scheme and
  every link shows `?`.
- Worse, a plain `build` afterwards can relink the binary while thinking the
  ODFRC resource step is up to date, producing a bundle with no `idrc_*`
  resources. InDesign rejects that with "does not recognize … as a valid
  plug-in", which looks like a code fault but is stale build state.

The script cleans everything, builds everything, then checks each bundle actually
has compiled resources before it says ok.

## Out-of-tree builds

The SDK's shared xcconfigs assume your project lives inside the SDK and derive
paths from `ID_PRJ_DIR`. Ours doesn't, so the files in
`build/mac/prj/_shared_build_settings/` include the SDK's originals and re-point
`ID_SDK_ROOT`. xcconfig references resolve lazily so inherited paths follow.

Three link-time search paths (`ID_DYLIBS_RELDIR`, `ID_STATICLIBS_RELDIR`,
`ID_FRAMEWORKS_RELDIR`) come off the project dir rather than `ID_SDK_ROOT`, so
they're overridden explicitly, since `InDesignModel.framework` and
`libPublicPlugIn.a` live in the SDK. Build output still lands in this repo.

## Windows

```bash
python3 build/win/genproj.py
```

Generates both `.vcxproj` files from the SDK's samples, supplying the `id_sdk_dir`
macro the SDK leaves undefined and putting `devtools\bin` on `ExecutablePath` so
ODFRC resolves without editing VC++ Directories by hand.

Compiles green in CI. Four `.pln` files come out, both plug-ins in both
configurations, each with its compiled resources.

Two things are not obvious and will bite a local build:

- **Pass the SDK paths as MSBuild globals.** The SDK's property sheets set
  `ID_SDK_DIR`, `BOOST_HEADER_SEARCH_PATH`, `DVA_ROOT`, `DVA_SHARED` and
  `ID_DVA` relative to the SDK's own `prj` directory, assuming the project lives
  inside the SDK tree. Property names are case-insensitive and an imported sheet
  outranks a `PropertyGroup` in your project, so only a command-line global wins.
- **The SDK caps out at MSVC 17.12.** `source/precomp/msvc/xlocnum_hack.h`
  patches a copy of the MSVC `<xlocnum>` header and hard-errors on any
  `_MSC_VER` it hasn't been updated for; the newest it knows is 1942. Anything
  newer needs an older toolset pinned with `/p:VCToolsVersion` (and a matching
  `/p:PlatformToolset`). CI picks the newest installed toolset the SDK accepts.

`.github/workflows/windows.yml` does all of this; copy the msbuild invocation
from there for local builds.

The SDK ships x64 only (`build/win` has `debugx64`/`releasex64` and the sample
projects declare no ARM64 platform), so no ARM64 slice despite InDesign 21.9
running natively on Windows ARM.

Re-run `genproj.py` after adding or renaming sources. It reads the source
directory, so new files are picked up automatically.

### CI

`.github/workflows/windows.yml` builds both plug-ins with MSBuild and is green.
The only thing GitHub's runners don't provide is the SDK, and that is a licensing
problem rather than a technical one.

The Windows build only needs about 400 MB of the SDK (`build/win`,
`devtools/bin`, `source/{open,precomp,public,sdksamples}`, `external`), not the
full 4 GB, so size is not the obstacle either.

**This repo is public, so the SDK must not be cached, uploaded as an artifact or
committed.** Two ways to give a runner access:

- **Self-hosted runner.** Register a Windows machine that already has the SDK
  unpacked, set the `SDK_PATH` repository variable, and switch `runs-on` to
  self-hosted. The SDK never leaves your control. Best fit for a public repo.
- **GitHub-hosted runner + private storage.** Put a zip somewhere
  access-controlled and set the `SDK_ZIP_URL` secret to a pre-signed URL. This
  does hand a copy to GitHub's infrastructure, so check it against the SDK
  licence first.

With neither configured the job stops early and says so rather than failing with
a confusing compile error. It deliberately doesn't upload the built `.pln` as an
artifact, since artifacts on a public repo are public.

### Cross-compiling from macOS

Not practical. The SDK ships the Windows import libs and `Odfrc.exe`, but MSVC
v143, the CRT/STL headers and the Windows SDK are Windows-only and
licence-restricted. `clang-cl` is ABI-compatible and `xwin` can fetch the headers,
but you'd have to re-express the build outside MSBuild, work around 22 sources
relying on MSVC precompiled headers, and run `Odfrc.exe` under Wine. Use a
Windows box, a VM, or CI. CI needs a self-hosted runner or a cached copy of the
SDK; it's too big and too licence-restricted to check out.

## InDesign Server

No extra work. The model plug-in declares
`{ kInDesignProduct, kInCopyProduct, kInDesignServerProduct }` and links
`InDesignModel.framework`, so the same binary loads under Server. Keep link and
scripting logic there; the UI plug-in is desktop-only and Server won't load it.

## Version lock

InDesign gates plug-ins on the dot release, not just the major version.

This SDK targets 21.4. A plug-in built from it won't load into 21.0.1: you get a
startup alert saying it requires 21.4. It does load into 21.5.1, so newer
applications are fine.

The gate isn't cosmetic: dot releases change ABI, which is why plug-ins built
against 21.0 are reported to crash 21.1. Match the SDK to the installed
application rather than forcing the declared version.

## Testing

Point InDesign at the build folder rather than copying:

```bash
echo '=Path "<repo>/build/mac/release_cocoa64/SDK"' \
  > ~/Library/Preferences/Adobe\ InDesign/Version\ 21.0/en_US/PluginConfig.txt
```

Restart InDesign, check Help ▸ About Plug-ins. Rename that file to disable.

Verified on 21.5.1 (macOS 26.6, Apple Silicon): both schemes resolve against
arbitrary hosts, 404 maps to missing, PDF export embeds the full-resolution asset,
and a saved document reopens with its links intact.
