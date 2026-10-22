#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_ADDRINFO_H
#وصف_ڪريو سرو_CURL_ADDRINFO_H
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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#  include <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#  include <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __VMS
#  include <in.h>
#  include <inet.h>
#جيڪڏهن_پورو

بنية Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

بنية Curl_addrinfo {
  صحيح                   ai_flags;
  صحيح                   ai_family;
  صحيح                   ai_socktype;
  صحيح                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  حرف                 *ai_canonname;
  بنية sockaddr      *ai_addr;
  بنية Curl_addrinfo *ai_ايندڙ;
};

فراغ Curl_freeaddrinfo(بنية Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
بنية Curl_addrinfo *Curl_addrinfo_get(بنية Curl_addrinfo *ai,
                                        صحيح ai_family,
                                        بنانشان صحيح n);

#جيڪڏهن_وصف_ٿيل HAVE_GETADDRINFO
صحيح Curl_getaddrinfo_ex(ثابت حرف *nodename,
                        ثابت حرف *servname,
                        ثابت بنية addrinfo *hints,
                        بنية Curl_addrinfo **نتيجو);
#جيڪڏهن_پورو

#إذا !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
بنية Curl_addrinfo *Curl_he2ai(ثابت بنية hostent *he, صحيح پورٽ);
#جيڪڏهن_پورو

bool Curl_is_ipv4addr(ثابت حرف *پتو);
bool Curl_is_ipaddr(ثابت حرف *پتو);
bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          بنية Curl_str *host, بنية Curl_str *zone);

CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t پورٽ,
                       بنية Curl_addrinfo **addrp);

#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ثابت حرف *رستو, bool abstract,
                        بنية Curl_addrinfo **paddr);
#جيڪڏهن_پورو

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
فراغ curl_dbg_freeaddrinfo(بنية addrinfo *freethis, صحيح سٽ,
                           ثابت حرف *ذريعو);
#جيڪڏهن_پورو

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
صحيح curl_dbg_getaddrinfo(ثابت حرف *hostname, ثابت حرف *service,
                         ثابت بنية addrinfo *hints,
                         بنية addrinfo **نتيجو, صحيح سٽ,
                         ثابت حرف *ذريعو);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_GETADDRINFO
#جيڪڏهن_وصف_ٿيل USE_RESOLVE_ON_IPS
فراغ Curl_addrinfo_سيٽ_پورٽ(بنية Curl_addrinfo *addrinfo, صحيح پورٽ);
#وإلا
#وصف_ڪريو Curl_addrinfo_سيٽ_پورٽ(x, y)
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_ADDRINFO_H */
