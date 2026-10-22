#که_نه_تعریف_شوی سرلیک_CURL_ADDRINFO_H
#تعریف_کړه سرلیک_CURL_ADDRINFO_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#ضمن "curl_setup.h"

#که_تعریف_شوی HAVE_NETINET_دننه_H
#  include <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#  include <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#د_که_پای

#که_تعریف_شوی __VMS
#  include <in.h>
#  include <inet.h>
#د_که_پای

بنية Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

بنية Curl_addrinfo {
  صحيح                   ai_flags;
  صحيح                   ai_family;
  صحيح                   ai_socktype;
  صحيح                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  حرف                 *ai_canonname;
  بنية sockaddr      *ai_addr;
  بنية Curl_addrinfo *ai_بل;
};

فراغ Curl_freeaddrinfo(بنية Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
بنية Curl_addrinfo *Curl_addrinfo_get(بنية Curl_addrinfo *ai,
                                        صحيح ai_family,
                                        بینښه صحيح n);

#که_تعریف_شوی HAVE_GETADDRINFO
صحيح Curl_getaddrinfo_ex(ثابت حرف *nodename,
                        ثابت حرف *servname,
                        ثابت بنية addrinfo *hints,
                        بنية Curl_addrinfo **پایله);
#د_که_پای

#إذا !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
بنية Curl_addrinfo *Curl_he2ai(ثابت بنية hostent *he, صحيح پورټ);
#د_که_پای

bool Curl_is_ipv4addr(ثابت حرف *پته);
bool Curl_is_ipaddr(ثابت حرف *پته);
bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          بنية Curl_str *host, بنية Curl_str *zone);

CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t پورټ,
                       بنية Curl_addrinfo **addrp);

#که_تعریف_شوی USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ثابت حرف *لار, bool abstract,
                        بنية Curl_addrinfo **paddr);
#د_که_پای

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
فراغ curl_dbg_freeaddrinfo(بنية addrinfo *freethis, صحيح کرښه,
                           ثابت حرف *سرچینه);
#د_که_پای

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
صحيح curl_dbg_getaddrinfo(ثابت حرف *hostname, ثابت حرف *service,
                         ثابت بنية addrinfo *hints,
                         بنية addrinfo **پایله, صحيح کرښه,
                         ثابت حرف *سرچینه);
#د_که_پای

#که_تعریف_شوی HAVE_GETADDRINFO
#که_تعریف_شوی USE_RESOLVE_ON_IPS
فراغ Curl_addrinfo_سېټ_پورټ(بنية Curl_addrinfo *addrinfo, صحيح پورټ);
#وإلا
#تعریف_کړه Curl_addrinfo_سېټ_پورټ(x, y)
#د_که_پای
#د_که_پای

#د_که_پای /* HEADER_CURL_ADDRINFO_H */
