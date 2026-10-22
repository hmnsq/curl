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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

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

ସଂରଚନା Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

ସଂରଚନା Curl_addrinfo {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା                   ai_flags;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା                   ai_family;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା                   ai_socktype;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  ଅକ୍ଷର                 *ai_canonname;
  ସଂରଚନା sockaddr      *ai_addr;
  ସଂରଚନା Curl_addrinfo *ai_next;
};

ଶୂନ୍ୟ Curl_freeaddrinfo(ସଂରଚନା Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
ସଂରଚନା Curl_addrinfo *Curl_addrinfo_get(ସଂରଚନା Curl_addrinfo *ai,
                                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family,
                                        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);

#ifdef HAVE_GETADDRINFO
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_getaddrinfo_ex(ଅଚଳ ଅକ୍ଷର *nodename,
                        ଅଚଳ ଅକ୍ଷର *servname,
                        ଅଚଳ ସଂରଚନା addrinfo *hints,
                        ସଂରଚନା Curl_addrinfo **result);
#endif

#ଯଦି !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ସଂରଚନା Curl_addrinfo *Curl_he2ai(ଅଚଳ ସଂରଚନା hostent *he, ପୂର୍ଣ୍ଣସଂଖ୍ୟା port);
#endif

bool Curl_is_ipv4addr(ଅଚଳ ଅକ୍ଷର *address);
bool Curl_is_ipaddr(ଅଚଳ ଅକ୍ଷର *address);
bool Curl_looks_like_ipv6(ଅଚଳ ଅକ୍ଷର *s, size_t len, bool maybe_url_encoded,
                          ସଂରଚନା Curl_str *host, ସଂରଚନା Curl_str *zone);

CURLcode Curl_str2addr(ଅଚଳ ଅକ୍ଷର *dotted, uint16_t port,
                       ସଂରଚନା Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ଅଚଳ ଅକ୍ଷର *path, bool abstract,
                        ସଂରଚନା Curl_addrinfo **paddr);
#endif

#ଯଦି defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
ଶୂନ୍ୟ curl_dbg_freeaddrinfo(ସଂରଚନା addrinfo *freethis, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                           ଅଚଳ ଅକ୍ଷର *source);
#endif

#ଯଦି defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_dbg_getaddrinfo(ଅଚଳ ଅକ୍ଷର *hostname, ଅଚଳ ଅକ୍ଷର *service,
                         ଅଚଳ ସଂରଚନା addrinfo *hints,
                         ସଂରଚନା addrinfo **result, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                         ଅଚଳ ଅକ୍ଷର *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
ଶୂନ୍ୟ Curl_addrinfo_set_port(ସଂରଚନା Curl_addrinfo *addrinfo, ପୂର୍ଣ୍ଣସଂଖ୍ୟା port);
#ଅନ୍ୟଥା
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
