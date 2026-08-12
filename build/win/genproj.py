#!/usr/bin/env python3
"""Generate build/win/prj/HttpLink.sdk.vcxproj from the SDK's CustomHttpLink
project.

Kept as a script rather than a hand-edited checked-in file because the SDK's
project changes between drops; re-running this against a new SDK is safer than
maintaining a divergent copy by hand.

Run from the repo root:  python3 build/win/genproj.py
"""
import os
import re
import sys
import uuid

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SDK_PRJ = os.path.join(REPO, "sdk", "build", "win", "prj")
OUT_PRJ = os.path.join(REPO, "build", "win", "prj")

# Stable GUIDs so regenerating doesn't churn solution files.
GUIDS = {
    "HttpLink": "{7A2C1E90-4B31-4E5B-9E2D-1C6F0A5D7B01}",
    "HttpLinkUI": "{7A2C1E90-4B31-4E5B-9E2D-1C6F0A5D7B02}",
}

# Sample file name -> our file name. The model plug-in renamed its sources when
# it was forked, so the project's ClCompile/None entries must follow.
RENAMES = [
    ("CusHttpLnkLinkResourceFactory", "HttpLinkResourceFactory"),
    ("CusHttpLnkLinkResourceHandler", "HttpLinkResourceHandler"),
    ("CusHttpLnkLinkResourceHelperHandler", "HttpLinkResourceHelperHandler"),
    ("CusHttpLnkLinkResourceConnection", "HttpLinkResourceConnection"),
    ("CusHttpLnkLinkResourceStateBatchUpdater", "HttpLinkResourceStateBatchUpdater"),
    ("CusHttpLnkLinkResourceStateUpdater", "HttpLinkResourceStateUpdater"),
    ("CusHttpLnkResourceServerAPIWrapper", "HttpLinkServerAPIWrapper"),
]


def inject_sources(text, name, srcdir):
    """Replace the sample's file lists with this repo's actual sources.

    The sample project is only a *shape* to copy: its ItemGroups list the
    sample's files, so anything added since the fork was silently missing from
    the Windows build. That failed quietly -- the plug-in still compiled and
    linked, it just shipped without whatever those files implemented.

    Entries under $(id_sdk_dir) are SDK-owned (entrypoint, statics, precompiled
    header) and are left alone; only repo-relative ones are regenerated.
    """
    src_dir = os.path.join(REPO, "source", srcdir)
    win_dir = "..\\..\\..\\source\\" + srcdir

    names = sorted(os.listdir(src_dir))
    cpps = [n for n in names if n.endswith(".cpp")]
    hdrs = [n for n in names if n.endswith(".h")]
    # The main .fr is compiled by ODFRC via CustomBuild; the localised ones are
    # #included by it, so they ride along as None entries.
    loc_frs = [n for n in names if n.endswith(".fr") and n != name + ".fr"]

    def rewrite(tag, files, block_marker):
        # Drop every existing repo-relative entry for this tag...
        pattern = r'[ \t]*<%s Include="\.\.\\\.\.\\\.\.\\source\\[^"]*"\s*/>\r?\n' % tag
        stripped = re.sub(pattern, "", block_marker)
        # ...then insert the real list before the ItemGroup close.
        lines = "".join('    <%s Include="%s\\%s" />\n' % (tag, win_dir, f) for f in files)
        return stripped, lines

    # ClCompile: rebuild the repo-relative portion in place.
    def fix_group(m):
        body = m.group(0)
        if 'Include="..\\..\\..\\source\\' not in body:
            return body
        for tag, files in (("ClCompile", cpps), ("ClInclude", hdrs), ("None", loc_frs)):
            if "<%s Include=" % tag not in body:
                continue
            body, lines = rewrite(tag, files, body)
            body = body.replace("</ItemGroup>", lines + "  </ItemGroup>", 1)
        return body

    text = re.sub(r"  <ItemGroup>.*?</ItemGroup>", fix_group, text, flags=re.S)

    # The sample pointed ODFRC at its own sibling samples (customdatalink and
    # customdatalinkui) for extra includes; ours needs its own source dir.
    text = re.sub(r'-i "\$\(id_sdk_dir\)\\source\\sdksamples\\customdatalink(ui)?"',
                  '-i "%s"' % win_dir.replace("\\", "\\\\"), text)
    return text


def transform(text, name, srcdir, resprefix):
    # 1. Longest-first so CusHttpLnkLinkResourceHelperHandler isn't clipped by
    #    the shorter CusHttpLnkLinkResourceHandler rule.
    for old, new in sorted(RENAMES, key=lambda p: -len(p[0])):
        text = text.replace(old, new)

    # 2. Our sources live in the repo, not in the SDK's sdksamples tree.
    text = text.replace(r"..\..\..\source\sdksamples\CustomHttpLinkUI",
                        r"..\..\..\source\httplinkui")
    text = text.replace(r"..\..\..\source\sdksamples\CustomHttpLink",
                        "\\".join([r"..\..\..\source", srcdir]))
    # Longest first, and NOT via `name`: for the UI job name is "HttpLinkUI", so
    # replacing "CustomHttpLink" with it turned a surviving "CustomHttpLinkUI"
    # into "HttpLinkUIUI" -- which is how the UI project ended up referencing
    # files that do not exist. The mapping is the same for both jobs, because
    # each project also references the other by name.
    text = text.replace("CustomHttpLinkUI", "HttpLinkUI")
    text = text.replace("CustomHttpLink", "HttpLink")

    # 3. Everything else under source\ belongs to the SDK. Rewrite the remaining
    #    ..\..\..\source\ prefixes (common helpers, open headers, customdatalink
    #    include path) to go through $(id_sdk_dir).
    text = re.sub(r"\.\.\\\.\.\\\.\.\\source\\(?!httplink)",
                  r"$(id_sdk_dir)\\source\\", text)

    # 4. Property sheets and response files live in the SDK's prj directory, not
    #    ours. Left bare they resolve against our project dir and MSBuild fails
    #    with MSB4019 before compiling anything.
    text = text.replace("@SDKCPPOptions.rsp", r"@$(id_sdk_dir)\build\win\prj\SDKCPPOptions.rsp")
    text = text.replace('@"SDKODFRCOptions.rsp"', r'@"$(id_sdk_dir)\build\win\prj\SDKODFRCOptions.rsp"')
    text = re.sub(r'<Import Project="([A-Za-z0-9_]+\.sdk\.props)"',
                  r'<Import Project="$(id_sdk_dir)\\build\\win\\prj\\\1"', text)

    # 5. Resource merge scripts: give our plug-in its own resource prefix so two
    #    plug-ins building side by side don't stomp each other's intermediates.
    text = text.replace("CusDtLnk", resprefix)

    # 6. Define id_sdk_dir. The SDK leaves this macro undefined and expects the
    #    consumer to supply it; ours points at <repo>/sdk to match the Mac side.
    #    It goes in Globals, not UserMacros: MSBuild evaluates in document order
    #    and the property sheets that use it are imported before UserMacros, so
    #    defining it there leaves it empty and the import fails with MSB4019.
    text = text.replace(
        "  <PropertyGroup Label=\"Globals\">",
        "  <PropertyGroup Label=\"Globals\">\n"
        "    <!-- The InDesign SDK, relative to this project. Matches ID_SDK_ROOT\n"
        "         in build/mac/prj/_shared_build_settings/plugin.sdk.xcconfig. -->\n"
        "    <id_sdk_dir>..\\..\\..\\sdk</id_sdk_dir>")

    # 7. ODFRC (the resource compiler) lives in the SDK's devtools. The SDK's
    #    getting-started guide tells you to add this to Executable Directories by
    #    hand; baking it into the project removes that manual step.
    text = text.replace(
        "  <ImportGroup Label=\"ExtensionSettings\">",
        "  <PropertyGroup>\n"
        "    <!-- ODFRC and friends, so the resource compile step resolves without\n"
        "         editing VC++ Directories by hand. -->\n"
        "    <ExecutablePath>$(id_sdk_dir)\\devtools\\bin;$(ExecutablePath)</ExecutablePath>\n"
        "  </PropertyGroup>\n"
        "  <ImportGroup Label=\"ExtensionSettings\">")

    # 8. Stable project GUID.
    text = re.sub(r"<ProjectGuid>\{[^}]*\}</ProjectGuid>",
                  "<ProjectGuid>%s</ProjectGuid>" % GUIDS[name], text)

    return text


def main():
    if not os.path.isdir(SDK_PRJ):
        sys.exit("SDK not found at %s -- see docs/build.md" % SDK_PRJ)
    os.makedirs(OUT_PRJ, exist_ok=True)

    jobs = [
        ("CustomHttpLink", "HttpLink", "httplink", "HttpLnk"),
        ("CustomHttpLinkUI", "HttpLinkUI", "httplinkui", "HttpLnkUI"),
    ]
    for sample, name, srcdir, resprefix in jobs:
        for ext in (".sdk.vcxproj", ".sdk.vcxproj.filters"):
            src = os.path.join(SDK_PRJ, sample + ext)
            if not os.path.exists(src):
                print("skip (absent): %s" % os.path.basename(src))
                continue
            with open(src, encoding="utf-8") as fh:
                text = fh.read()
            out = os.path.join(OUT_PRJ, name + ext)
            result = transform(text, name, srcdir, resprefix)
            if ext == ".sdk.vcxproj":
                result = inject_sources(result, name, srcdir)
            with open(out, "w", encoding="utf-8") as fh:
                fh.write(result)
            print("wrote %s" % os.path.relpath(out, REPO))


if __name__ == "__main__":
    main()
