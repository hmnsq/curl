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
#அந்தர்பாவ "curl_setup.h"

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

கட்டமைப்பு Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

கட்டமைப்பு Curl_addrinfo {
  முழு                   ai_flags;
  முழு                   ai_family;
  முழு                   ai_socktype;
  முழு                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  எழுத்து                 *ai_canonname;
  கட்டமைப்பு sockaddr      *ai_addr;
  கட்டமைப்பு Curl_addrinfo *ai_next;
};

வெறுமை Curl_freeaddrinfo(கட்டமைப்பு Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
கட்டமைப்பு Curl_addrinfo *Curl_addrinfo_get(கட்டமைப்பு Curl_addrinfo *ai,
                                        முழு ai_family,
                                        unsigned முழு n);

#ifdef HAVE_GETADDRINFO
முழு Curl_getaddrinfo_ex(மாறிலி எழுத்து *nodename,
                        மாறிலி எழுத்து *servname,
                        மாறிலி கட்டமைப்பு addrinfo *hints,
                        கட்டமைப்பு Curl_addrinfo **result);
#endif

#எனில் !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
கட்டமைப்பு Curl_addrinfo *Curl_he2ai(மாறிலி கட்டமைப்பு hostent *he, முழு port);
#endif

bool Curl_is_ipv4addr(மாறிலி எழுத்து *address);
bool Curl_is_ipaddr(மாறிலி எழுத்து *address);
bool Curl_looks_like_ipv6(மாறிலி எழுத்து *s, size_t len, bool maybe_url_encoded,
                          கட்டமைப்பு Curl_str *host, கட்டமைப்பு Curl_str *zone);

CURLcode Curl_str2addr(மாறிலி எழுத்து *dotted, uint16_t port,
                       கட்டமைப்பு Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(மாறிலி எழுத்து *path, bool abstract,
                        கட்டமைப்பு Curl_addrinfo **paddr);
#endif

#எனில் defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
வெறுமை curl_dbg_freeaddrinfo(கட்டமைப்பு addrinfo *freethis, முழு வரி,
                           மாறிலி எழுத்து *source);
#endif

#எனில் defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
முழு curl_dbg_getaddrinfo(மாறிலி எழுத்து *hostname, மாறிலி எழுத்து *service,
                         மாறிலி கட்டமைப்பு addrinfo *hints,
                         கட்டமைப்பு addrinfo **result, முழு வரி,
                         மாறிலி எழுத்து *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
வெறுமை Curl_addrinfo_set_port(கட்டமைப்பு Curl_addrinfo *addrinfo, முழு port);
#இல்லையெனில்
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
