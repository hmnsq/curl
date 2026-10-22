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
#include "curl_setup.h"

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

sa_nracanaa Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

sa_nracanaa Curl_addrinfo {
  ຈຳນວນເຕັມ                   ai_flags;
  ຈຳນວນເຕັມ                   ai_family;
  ຈຳນວນເຕັມ                   ai_socktype;
  ຈຳນວນເຕັມ                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  var_na                 *ai_canonname;
  sa_nracanaa sockaddr      *ai_addr;
  sa_nracanaa Curl_addrinfo *ai_next;
};

void Curl_freeaddrinfo(sa_nracanaa Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
sa_nracanaa Curl_addrinfo *Curl_addrinfo_get(sa_nracanaa Curl_addrinfo *ai,
                                        ຈຳນວນເຕັມ ai_family,
                                        unsigned ຈຳນວນເຕັມ n);

#ifdef HAVE_GETADDRINFO
ຈຳນວນເຕັມ Curl_getaddrinfo_ex(const var_na *nodename,
                        const var_na *servname,
                        const sa_nracanaa addrinfo *hints,
                        sa_nracanaa Curl_addrinfo **result);
#endif

#ຖ້າ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
sa_nracanaa Curl_addrinfo *Curl_he2ai(const sa_nracanaa hostent *he, ຈຳນວນເຕັມ port);
#endif

bool Curl_is_ipv4addr(const var_na *address);
bool Curl_is_ipaddr(const var_na *address);
bool Curl_looks_like_ipv6(const var_na *s, size_t len, bool maybe_url_encoded,
                          sa_nracanaa Curl_str *host, sa_nracanaa Curl_str *zone);

CURLcode Curl_str2addr(const var_na *dotted, uint16_t port,
                       sa_nracanaa Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(const var_na *path, bool abstract,
                        sa_nracanaa Curl_addrinfo **paddr);
#endif

#ຖ້າ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
void curl_dbg_freeaddrinfo(sa_nracanaa addrinfo *freethis, ຈຳນວນເຕັມ line,
                           const var_na *source);
#endif

#ຖ້າ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
ຈຳນວນເຕັມ curl_dbg_getaddrinfo(const var_na *hostname, const var_na *service,
                         const sa_nracanaa addrinfo *hints,
                         sa_nracanaa addrinfo **result, ຈຳນວນເຕັມ line,
                         const var_na *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
void Curl_addrinfo_set_port(sa_nracanaa Curl_addrinfo *addrinfo, ຈຳນວນເຕັມ port);
#ບໍ່ດັ່ງນັ້ນ
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
