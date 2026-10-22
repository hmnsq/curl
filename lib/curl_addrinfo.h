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
#અન્તર્ભાવ "curl_setup.h"

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

સંરચના Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

સંરચના Curl_addrinfo {
  પૂર્ણાંક                   ai_flags;
  પૂર્ણાંક                   ai_family;
  પૂર્ણાંક                   ai_socktype;
  પૂર્ણાંક                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  અક્ષર                 *ai_canonname;
  સંરચના sockaddr      *ai_addr;
  સંરચના Curl_addrinfo *ai_next;
};

શૂન્ય Curl_freeaddrinfo(સંરચના Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
સંરચના Curl_addrinfo *Curl_addrinfo_get(સંરચના Curl_addrinfo *ai,
                                        પૂર્ણાંક ai_family,
                                        unsigned પૂર્ણાંક n);

#ifdef HAVE_GETADDRINFO
પૂર્ણાંક Curl_getaddrinfo_ex(અચલ અક્ષર *nodename,
                        અચલ અક્ષર *servname,
                        અચલ સંરચના addrinfo *hints,
                        સંરચના Curl_addrinfo **result);
#endif

#જો !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
સંરચના Curl_addrinfo *Curl_he2ai(અચલ સંરચના hostent *he, પૂર્ણાંક port);
#endif

bool Curl_is_ipv4addr(અચલ અક્ષર *address);
bool Curl_is_ipaddr(અચલ અક્ષર *address);
bool Curl_looks_like_ipv6(અચલ અક્ષર *s, size_t len, bool maybe_url_encoded,
                          સંરચના Curl_str *host, સંરચના Curl_str *zone);

CURLcode Curl_str2addr(અચલ અક્ષર *dotted, uint16_t port,
                       સંરચના Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(અચલ અક્ષર *path, bool abstract,
                        સંરચના Curl_addrinfo **paddr);
#endif

#જો defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
શૂન્ય curl_dbg_freeaddrinfo(સંરચના addrinfo *freethis, પૂર્ણાંક લીટી,
                           અચલ અક્ષર *source);
#endif

#જો defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
પૂર્ણાંક curl_dbg_getaddrinfo(અચલ અક્ષર *hostname, અચલ અક્ષર *service,
                         અચલ સંરચના addrinfo *hints,
                         સંરચના addrinfo **result, પૂર્ણાંક લીટી,
                         અચલ અક્ષર *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
શૂન્ય Curl_addrinfo_set_port(સંરચના Curl_addrinfo *addrinfo, પૂર્ણાંક port);
#નહીંતર
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
