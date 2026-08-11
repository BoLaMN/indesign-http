//========================================================================================
//
//  HttpLinkUIURLUtils.h
//
//  http(s) URL -> our private-scheme URI. Shared by the panel button, the drop
//  handler and Relink from Web, so all three agree on what a valid URL is.
//
//  The model plug-in keeps its own copy for the scripting API: Server never
//  loads this plug-in, so it cannot include this header. Change one, change both.
//
//========================================================================================

#pragma once
#ifndef __HttpLinkUIURLUtils__
#define __HttpLinkUIURLUtils__

#include "PMString.h"
#include "URI.h"

#include "HttpLinkUIID.h"

#include <string>

/** Rewrites an http(s) URL onto our private scheme.
    @param typed  [IN]  what the user typed, pasted or dragged
    @param outURI [OUT] the resulting idhttp/idhttps URI
    @return kFalse if this isn't a URL we can place
*/
inline bool16 HttpLinkUIBuildResourceURI(const PMString& typed, URI& outURI)
{
    std::string url = typed.GetUTF8String();

    // A dragged payload can carry several newline-separated lines (URL then
    // title); keep the first.
    const std::string::size_type nl = url.find_first_of("\r\n");
    if (nl != std::string::npos)
        url = url.substr(0, nl);

    const std::string ws(" \t\r\n");
    const std::string::size_type first = url.find_first_not_of(ws);
    if (first == std::string::npos)
        return kFalse;
    const std::string::size_type last = url.find_last_not_of(ws);
    url = url.substr(first, last - first + 1);

    const char* scheme = nil;
    if (url.compare(0, 8, "https://") == 0)      { scheme = kHttpLnkSchemeSecure; url.erase(0, 8); }
    else if (url.compare(0, 7, "http://") == 0)  { scheme = kHttpLnkScheme;       url.erase(0, 7); }
    else if (url.compare(0, 10, "idhttps://") == 0 || url.compare(0, 9, "idhttp://") == 0)
    {
        // Already one of our URIs -- accept verbatim. Without this, such a string
        // would fall into the bare-host branch below and be mangled into
        // "idhttps://idhttp://host/path", which passes validation (the authority
        // is non-empty) and silently creates a broken link.
        outURI = URI(url.c_str());
        return kTrue;
    }
    else
    {
        // Bare host: assume https rather than quietly downgrading to plaintext.
        scheme = kHttpLnkSchemeSecure;
    }

    const std::string::size_type slash = url.find('/');
    std::string authority = (slash == std::string::npos) ? url : url.substr(0, slash);
    std::string path      = (slash == std::string::npos) ? std::string("/") : url.substr(slash);
    if (authority.empty())
        return kFalse;

    // Query strings are part of an asset's identity on most CDNs (cache-busting,
    // signing), so they are carried rather than dropped.
    std::string query;
    const std::string::size_type qmark = path.find('?');
    if (qmark != std::string::npos)
    {
        query = path.substr(qmark + 1);
        path  = path.substr(0, qmark);
    }

    outURI.SetComponent(URI::kScheme,    WideString(scheme));
    outURI.SetComponent(URI::kAuthority, WideString(authority.c_str()));
    outURI.SetComponent(URI::kPath,      WideString(path.c_str()));
    if (!query.empty())
        outURI.SetComponent(URI::kQuery, WideString(query.c_str()));

    return kTrue;
}

#endif // __HttpLinkUIURLUtils__
