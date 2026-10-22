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
#شامل "curl_setup.h"

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

ساختار Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

ساختار Curl_addrinfo {
  صحیح                   ai_flags;
  صحیح                   ai_family;
  صحیح                   ai_socktype;
  صحیح                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  نویسه                 *ai_canonname;
  ساختار sockaddr      *ai_addr;
  ساختار Curl_addrinfo *ai_next;
};

تهی Curl_freeaddrinfo(ساختار Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
ساختار Curl_addrinfo *Curl_addrinfo_get(ساختار Curl_addrinfo *ai,
                                        صحیح ai_family,
                                        unsigned صحیح n);

#ifdef HAVE_GETADDRINFO
صحیح Curl_getaddrinfo_ex(ثابت نویسه *nodename,
                        ثابت نویسه *servname,
                        ثابت ساختار addrinfo *hints,
                        ساختار Curl_addrinfo **result);
#endif

#اگر !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ساختار Curl_addrinfo *Curl_he2ai(ثابت ساختار hostent *he, صحیح port);
#endif

bool Curl_is_ipv4addr(ثابت نویسه *address);
bool Curl_is_ipaddr(ثابت نویسه *address);
bool Curl_looks_like_ipv6(ثابت نویسه *s, size_t len, bool maybe_url_encoded,
                          ساختار Curl_str *host, ساختار Curl_str *zone);

CURLcode Curl_str2addr(ثابت نویسه *dotted, uint16_t port,
                       ساختار Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ثابت نویسه *path, bool abstract,
                        ساختار Curl_addrinfo **paddr);
#endif

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
تهی curl_dbg_freeaddrinfo(ساختار addrinfo *freethis, صحیح سطر,
                           ثابت نویسه *source);
#endif

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
صحیح curl_dbg_getaddrinfo(ثابت نویسه *hostname, ثابت نویسه *service,
                         ثابت ساختار addrinfo *hints,
                         ساختار addrinfo **result, صحیح سطر,
                         ثابت نویسه *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
تهی Curl_addrinfo_set_port(ساختار Curl_addrinfo *addrinfo, صحیح port);
#وگرنه
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
