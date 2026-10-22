#ifndef HEADER_CURL_ADDRINFO_H
#define HEADER_CURL_ADDRINFO_H
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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#  include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#endif

#ifdef __VMS
#  include <in.h>
#  include <inet.h>
#endif

সংরচনা Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

সংরচনা Curl_addrinfo {
  পূর্ণাঙ্ক                   ai_flags;
  পূর্ণাঙ্ক                   ai_family;
  পূর্ণাঙ্ক                   ai_socktype;
  পূর্ণাঙ্ক                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  বর্ণ                 *ai_canonname;
  সংরচনা sockaddr      *ai_addr;
  সংরচনা Curl_addrinfo *ai_next;
};

শূন্য Curl_freeaddrinfo(সংরচনা Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
সংরচনা Curl_addrinfo *Curl_addrinfo_get(সংরচনা Curl_addrinfo *ai,
                                        পূর্ণাঙ্ক ai_family,
                                        unsigned পূর্ণাঙ্ক n);

#ifdef HAVE_GETADDRINFO
পূর্ণাঙ্ক Curl_getaddrinfo_ex(ধ্রুব বর্ণ *nodename,
                        ধ্রুব বর্ণ *servname,
                        ধ্রুব সংরচনা addrinfo *hints,
                        সংরচনা Curl_addrinfo **result);
#endif

#যদি !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
সংরচনা Curl_addrinfo *Curl_he2ai(ধ্রুব সংরচনা hostent *he, পূর্ণাঙ্ক port);
#endif

bool Curl_is_ipv4addr(ধ্রুব বর্ণ *address);
bool Curl_is_ipaddr(ধ্রুব বর্ণ *address);
bool Curl_looks_like_ipv6(ধ্রুব বর্ণ *s, size_t len, bool maybe_url_encoded,
                          সংরচনা Curl_str *host, সংরচনা Curl_str *zone);

CURLcode Curl_str2addr(ধ্রুব বর্ণ *dotted, uint16_t port,
                       সংরচনা Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ধ্রুব বর্ণ *path, bool abstract,
                        সংরচনা Curl_addrinfo **paddr);
#endif

#যদি defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
শূন্য curl_dbg_freeaddrinfo(সংরচনা addrinfo *freethis, পূর্ণাঙ্ক সারি,
                           ধ্রুব বর্ণ *source);
#endif

#যদি defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
পূর্ণাঙ্ক curl_dbg_getaddrinfo(ধ্রুব বর্ণ *hostname, ধ্রুব বর্ণ *service,
                         ধ্রুব সংরচনা addrinfo *hints,
                         সংরচনা addrinfo **result, পূর্ণাঙ্ক সারি,
                         ধ্রুব বর্ণ *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
শূন্য Curl_addrinfo_set_port(সংরচনা Curl_addrinfo *addrinfo, পূর্ণাঙ্ক port);
#অন্যথা
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
