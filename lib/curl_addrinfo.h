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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

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

ಸಂರಚನಾ Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

ಸಂರಚನಾ Curl_addrinfo {
  ಪೂರ್ಣಾಙ್ಕ                   ai_flags;
  ಪೂರ್ಣಾಙ್ಕ                   ai_family;
  ಪೂರ್ಣಾಙ್ಕ                   ai_socktype;
  ಪೂರ್ಣಾಙ್ಕ                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  ವರ್ಣ                 *ai_canonname;
  ಸಂರಚನಾ sockaddr      *ai_addr;
  ಸಂರಚನಾ Curl_addrinfo *ai_next;
};

ಶೂನ್ಯ Curl_freeaddrinfo(ಸಂರಚನಾ Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
ಸಂರಚನಾ Curl_addrinfo *Curl_addrinfo_get(ಸಂರಚನಾ Curl_addrinfo *ai,
                                        ಪೂರ್ಣಾಙ್ಕ ai_family,
                                        unsigned ಪೂರ್ಣಾಙ್ಕ n);

#ifdef HAVE_GETADDRINFO
ಪೂರ್ಣಾಙ್ಕ Curl_getaddrinfo_ex(ಧ್ರುವ ವರ್ಣ *nodename,
                        ಧ್ರುವ ವರ್ಣ *servname,
                        ಧ್ರುವ ಸಂರಚನಾ addrinfo *hints,
                        ಸಂರಚನಾ Curl_addrinfo **result);
#endif

#ಯದಿ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ಸಂರಚನಾ Curl_addrinfo *Curl_he2ai(ಧ್ರುವ ಸಂರಚನಾ hostent *he, ಪೂರ್ಣಾಙ್ಕ port);
#endif

bool Curl_is_ipv4addr(ಧ್ರುವ ವರ್ಣ *address);
bool Curl_is_ipaddr(ಧ್ರುವ ವರ್ಣ *address);
bool Curl_looks_like_ipv6(ಧ್ರುವ ವರ್ಣ *s, size_t len, bool maybe_url_encoded,
                          ಸಂರಚನಾ Curl_str *host, ಸಂರಚನಾ Curl_str *zone);

CURLcode Curl_str2addr(ಧ್ರುವ ವರ್ಣ *dotted, uint16_t port,
                       ಸಂರಚನಾ Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ಧ್ರುವ ವರ್ಣ *path, bool abstract,
                        ಸಂರಚನಾ Curl_addrinfo **paddr);
#endif

#ಯದಿ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
ಶೂನ್ಯ curl_dbg_freeaddrinfo(ಸಂರಚನಾ addrinfo *freethis, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                           ಧ್ರುವ ವರ್ಣ *source);
#endif

#ಯದಿ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
ಪೂರ್ಣಾಙ್ಕ curl_dbg_getaddrinfo(ಧ್ರುವ ವರ್ಣ *hostname, ಧ್ರುವ ವರ್ಣ *service,
                         ಧ್ರುವ ಸಂರಚನಾ addrinfo *hints,
                         ಸಂರಚನಾ addrinfo **result, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                         ಧ್ರುವ ವರ್ಣ *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
ಶೂನ್ಯ Curl_addrinfo_set_port(ಸಂರಚನಾ Curl_addrinfo *addrinfo, ಪೂರ್ಣಾಙ್ಕ port);
#ಅನ್ಯಥಾ
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
