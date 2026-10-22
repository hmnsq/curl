#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ADDRINFO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ADDRINFO_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#  include <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#  include <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#  include <in.h>
#  include <inet.h>
#ਜੇ_ਅੰਤ

ਬਣਤਰ Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

ਬਣਤਰ Curl_addrinfo {
  ਪੂਰਨ                   ai_flags;
  ਪੂਰਨ                   ai_family;
  ਪੂਰਨ                   ai_socktype;
  ਪੂਰਨ                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  ਅੱਖਰ                 *ai_canonname;
  ਬਣਤਰ sockaddr      *ai_addr;
  ਬਣਤਰ Curl_addrinfo *ai_ਅਗਲਾ;
};

ਖਾਲੀ Curl_freeaddrinfo(ਬਣਤਰ Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
ਬਣਤਰ Curl_addrinfo *Curl_addrinfo_get(ਬਣਤਰ Curl_addrinfo *ai,
                                        ਪੂਰਨ ai_family,
                                        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETADDRINFO
ਪੂਰਨ Curl_getaddrinfo_ex(ਅਚਲ ਅੱਖਰ *nodename,
                        ਅਚਲ ਅੱਖਰ *servname,
                        ਅਚਲ ਬਣਤਰ addrinfo *hints,
                        ਬਣਤਰ Curl_addrinfo **ਨਤੀਜਾ);
#ਜੇ_ਅੰਤ

#ਜੇ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ਬਣਤਰ Curl_addrinfo *Curl_he2ai(ਅਚਲ ਬਣਤਰ hostent *he, ਪੂਰਨ ਪੋਰਟ);
#ਜੇ_ਅੰਤ

bool Curl_is_ipv4addr(ਅਚਲ ਅੱਖਰ *ਪਤਾ);
bool Curl_is_ipaddr(ਅਚਲ ਅੱਖਰ *ਪਤਾ);
bool Curl_looks_like_ipv6(ਅਚਲ ਅੱਖਰ *s, size_t len, bool maybe_url_encoded,
                          ਬਣਤਰ Curl_str *host, ਬਣਤਰ Curl_str *zone);

CURLcode Curl_str2addr(ਅਚਲ ਅੱਖਰ *dotted, uint16_t ਪੋਰਟ,
                       ਬਣਤਰ Curl_addrinfo **addrp);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool abstract,
                        ਬਣਤਰ Curl_addrinfo **paddr);
#ਜੇ_ਅੰਤ

#ਜੇ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
ਖਾਲੀ curl_dbg_freeaddrinfo(ਬਣਤਰ addrinfo *freethis, ਪੂਰਨ ਸਤਰ,
                           ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
#ਜੇ_ਅੰਤ

#ਜੇ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
ਪੂਰਨ curl_dbg_getaddrinfo(ਅਚਲ ਅੱਖਰ *hostname, ਅਚਲ ਅੱਖਰ *service,
                         ਅਚਲ ਬਣਤਰ addrinfo *hints,
                         ਬਣਤਰ addrinfo **ਨਤੀਜਾ, ਪੂਰਨ ਸਤਰ,
                         ਅਚਲ ਅੱਖਰ *ਸਰੋਤ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETADDRINFO
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLVE_ON_IPS
ਖਾਲੀ Curl_addrinfo_ਸੈੱਟ_ਪੋਰਟ(ਬਣਤਰ Curl_addrinfo *addrinfo, ਪੂਰਨ ਪੋਰਟ);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_addrinfo_ਸੈੱਟ_ਪੋਰਟ(x, y)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_ADDRINFO_H */
