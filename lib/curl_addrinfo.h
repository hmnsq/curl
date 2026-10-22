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
#അന്തര്ഭാവ "curl_setup.h"

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

ഘടന Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

ഘടന Curl_addrinfo {
  പൂർണ്ണസംഖ്യ                   ai_flags;
  പൂർണ്ണസംഖ്യ                   ai_family;
  പൂർണ്ണസംഖ്യ                   ai_socktype;
  പൂർണ്ണസംഖ്യ                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  അക്ഷരം                 *ai_canonname;
  ഘടന sockaddr      *ai_addr;
  ഘടന Curl_addrinfo *ai_next;
};

ശൂന്യം Curl_freeaddrinfo(ഘടന Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
ഘടന Curl_addrinfo *Curl_addrinfo_get(ഘടന Curl_addrinfo *ai,
                                        പൂർണ്ണസംഖ്യ ai_family,
                                        unsigned പൂർണ്ണസംഖ്യ n);

#ifdef HAVE_GETADDRINFO
പൂർണ്ണസംഖ്യ Curl_getaddrinfo_ex(സ്ഥിരം അക്ഷരം *nodename,
                        സ്ഥിരം അക്ഷരം *servname,
                        സ്ഥിരം ഘടന addrinfo *hints,
                        ഘടന Curl_addrinfo **result);
#endif

#എങ്കിൽ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ഘടന Curl_addrinfo *Curl_he2ai(സ്ഥിരം ഘടന hostent *he, പൂർണ്ണസംഖ്യ port);
#endif

bool Curl_is_ipv4addr(സ്ഥിരം അക്ഷരം *address);
bool Curl_is_ipaddr(സ്ഥിരം അക്ഷരം *address);
bool Curl_looks_like_ipv6(സ്ഥിരം അക്ഷരം *s, size_t len, bool maybe_url_encoded,
                          ഘടന Curl_str *host, ഘടന Curl_str *zone);

CURLcode Curl_str2addr(സ്ഥിരം അക്ഷരം *dotted, uint16_t port,
                       ഘടന Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(സ്ഥിരം അക്ഷരം *path, bool abstract,
                        ഘടന Curl_addrinfo **paddr);
#endif

#എങ്കിൽ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
ശൂന്യം curl_dbg_freeaddrinfo(ഘടന addrinfo *freethis, പൂർണ്ണസംഖ്യ വരി,
                           സ്ഥിരം അക്ഷരം *source);
#endif

#എങ്കിൽ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
പൂർണ്ണസംഖ്യ curl_dbg_getaddrinfo(സ്ഥിരം അക്ഷരം *hostname, സ്ഥിരം അക്ഷരം *service,
                         സ്ഥിരം ഘടന addrinfo *hints,
                         ഘടന addrinfo **result, പൂർണ്ണസംഖ്യ വരി,
                         സ്ഥിരം അക്ഷരം *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
ശൂന്യം Curl_addrinfo_set_port(ഘടന Curl_addrinfo *addrinfo, പൂർണ്ണസംഖ്യ port);
#അല്ലെങ്കിൽ
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
