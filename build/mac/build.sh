#!/usr/bin/env bash
#
# Build both plug-ins on macOS.
#
# Use this rather than calling xcodebuild per project. The two projects share one
# output directory (build/mac/<config>_cocoa64/SDK), which makes per-project
# xcodebuild invocations dangerous in two specific ways:
#
#   1. `clean` on either project deletes the *other* plug-in's bundle from the
#      shared folder. InDesign then loads whichever half survived -- and with the
#      model plug-in missing, http links silently resolve to nothing.
#
#   2. Worse: after such a clean, a plain `build` can relink the binary while
#      considering the ODFRC resource step up to date, producing a bundle with no
#      idrc_* resources. InDesign rejects that with "does not recognize ... as a
#      valid plug-in" -- which looks like a code problem but is purely a stale
#      build-state problem.
#
# So: clean everything first, then build everything, and verify the resources
# actually landed.
#
# Usage:  build/mac/build.sh [Release|Debug|both]   (default: both)

set -euo pipefail

cd "$(dirname "$0")/prj"
OUT_ROOT="$(cd .. && pwd)"

PROJECTS=(HttpLink HttpLinkUI)
case "${1:-both}" in
    Release|release) CONFIGS=(Release) ;;
    Debug|debug)     CONFIGS=(Debug) ;;
    both|"")         CONFIGS=(Release Debug) ;;
    *) echo "usage: $0 [Release|Debug|both]" >&2; exit 2 ;;
esac

echo "==> cleaning"
for p in "${PROJECTS[@]}"; do
    for c in "${CONFIGS[@]}"; do
        xcodebuild -project "$p.xcodeproj" -target "${c}_Cocoa64" \
                   -configuration Default clean >/dev/null
    done
done

echo "==> building"
for p in "${PROJECTS[@]}"; do
    for c in "${CONFIGS[@]}"; do
        printf '    %-12s %-7s ... ' "$p" "$c"
        if xcodebuild -project "$p.xcodeproj" -target "${c}_Cocoa64" \
                      -configuration Default build 2>&1 | grep -q '^\*\* BUILD SUCCEEDED'; then
            echo ok
        else
            echo FAILED
            exit 1
        fi
    done
done

echo "==> verifying bundles"
status=0
for c in "${CONFIGS[@]}"; do
    lc=$(echo "$c" | tr '[:upper:]' '[:lower:]')
    dir="$OUT_ROOT/${lc}_cocoa64/SDK"
    for p in "${PROJECTS[@]}"; do
        bundle="$dir/$p.sdk.InDesignPlugin"
        if [[ ! -d "$bundle" ]]; then
            echo "    MISSING: $bundle"; status=1; continue
        fi
        # An InDesign plug-in without compiled resources loads as "not a valid
        # plug-in", so treat their absence as a build failure rather than
        # letting it reach InDesign.
        n=$(ls "$bundle/Versions/A/Resources" 2>/dev/null | grep -c '^idrc_' || true)
        if [[ "$n" -eq 0 ]]; then
            echo "    NO RESOURCES: $p ($c) -- ODFRC did not run"; status=1
        else
            arch=$(lipo -archs "$bundle/Versions/A/$p.sdk" 2>/dev/null || echo '?')
            echo "    ok: $p ($c) ${n} resource groups [$arch]"
        fi
    done
done

exit $status
