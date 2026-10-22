#جے_بنا_تعریف سرخی_CURL_ADDRINFO_H
#تعریف_کرو سرخی_CURL_ADDRINFO_H
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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#  include <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#  include <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#جے_انت

#جے_تعریف_شدہ __VMS
#  include <in.h>
#  include <inet.h>
#جے_انت

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
  بنية Curl_addrinfo *ai_اگلا;
};

فراغ Curl_freeaddrinfo(بنية Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
بنية Curl_addrinfo *Curl_addrinfo_get(بنية Curl_addrinfo *ai,
                                        صحيح ai_family,
                                        بنانشانی صحيح n);

#جے_تعریف_شدہ HAVE_GETADDRINFO
صحيح Curl_getaddrinfo_ex(ثابت حرف *nodename,
                        ثابت حرف *servname,
                        ثابت بنية addrinfo *hints,
                        بنية Curl_addrinfo **نتیجہ);
#جے_انت

#إذا !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
بنية Curl_addrinfo *Curl_he2ai(ثابت بنية hostent *he, صحيح پورٹ);
#جے_انت

bool Curl_is_ipv4addr(ثابت حرف *پتہ);
bool Curl_is_ipaddr(ثابت حرف *پتہ);
bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          بنية Curl_str *host, بنية Curl_str *zone);

CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t پورٹ,
                       بنية Curl_addrinfo **addrp);

#جے_تعریف_شدہ USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ثابت حرف *رستہ, bool abstract,
                        بنية Curl_addrinfo **paddr);
#جے_انت

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
فراغ curl_dbg_freeaddrinfo(بنية addrinfo *freethis, صحيح سطر,
                           ثابت حرف *سروت);
#جے_انت

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
صحيح curl_dbg_getaddrinfo(ثابت حرف *hostname, ثابت حرف *service,
                         ثابت بنية addrinfo *hints,
                         بنية addrinfo **نتیجہ, صحيح سطر,
                         ثابت حرف *سروت);
#جے_انت

#جے_تعریف_شدہ HAVE_GETADDRINFO
#جے_تعریف_شدہ USE_RESOLVE_ON_IPS
فراغ Curl_addrinfo_سیٹ_پورٹ(بنية Curl_addrinfo *addrinfo, صحيح پورٹ);
#وإلا
#تعریف_کرو Curl_addrinfo_سیٹ_پورٹ(x, y)
#جے_انت
#جے_انت

#جے_انت /* HEADER_CURL_ADDRINFO_H */
