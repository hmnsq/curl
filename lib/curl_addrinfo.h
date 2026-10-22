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

struct Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

struct Curl_addrinfo {
  整数                   ai_flags;
  整数                   ai_family;
  整数                   ai_socktype;
  整数                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  char                 *ai_canonname;
  struct sockaddr      *ai_addr;
  struct Curl_addrinfo *ai_next;
};

void Curl_freeaddrinfo(struct Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
struct Curl_addrinfo *Curl_addrinfo_get(struct Curl_addrinfo *ai,
                                        整数 ai_family,
                                        unsigned 整数 n);

#ifdef HAVE_GETADDRINFO
整数 Curl_getaddrinfo_ex(const char *nodename,
                        const char *servname,
                        const struct addrinfo *hints,
                        struct Curl_addrinfo **result);
#endif

#如果 !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
struct Curl_addrinfo *Curl_he2ai(const struct hostent *he, 整数 port);
#endif

bool Curl_is_ipv4addr(const char *address);
bool Curl_is_ipaddr(const char *address);
bool Curl_looks_like_ipv6(const char *s, size_t len, bool maybe_url_encoded,
                          struct Curl_str *host, struct Curl_str *zone);

CURLcode Curl_str2addr(const char *dotted, uint16_t port,
                       struct Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(const char *path, bool abstract,
                        struct Curl_addrinfo **paddr);
#endif

#如果 defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
void curl_dbg_freeaddrinfo(struct addrinfo *freethis, 整数 line,
                           const char *source);
#endif

#如果 defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
整数 curl_dbg_getaddrinfo(const char *hostname, const char *service,
                         const struct addrinfo *hints,
                         struct addrinfo **result, 整数 line,
                         const char *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
void Curl_addrinfo_set_port(struct Curl_addrinfo *addrinfo, 整数 port);
#否则
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
