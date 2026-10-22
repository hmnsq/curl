#اگر_تعریف_نشده سرآیند_CURL_ADDRINFO_H
#تعریف_کن سرآیند_CURL_ADDRINFO_H
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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#  include <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#  include <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#پایان_اگر

#اگر_تعریف_شده __VMS
#  include <in.h>
#  include <inet.h>
#پایان_اگر

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
  بنية Curl_addrinfo *ai_بعدی;
};

فراغ Curl_freeaddrinfo(بنية Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
بنية Curl_addrinfo *Curl_addrinfo_get(بنية Curl_addrinfo *ai,
                                        صحيح ai_family,
                                        بیعلامت صحيح n);

#اگر_تعریف_شده HAVE_GETADDRINFO
صحيح Curl_getaddrinfo_ex(ثابت حرف *nodename,
                        ثابت حرف *servname,
                        ثابت بنية addrinfo *hints,
                        بنية Curl_addrinfo **نتیجه);
#پایان_اگر

#إذا !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
بنية Curl_addrinfo *Curl_he2ai(ثابت بنية hostent *he, صحيح پورت);
#پایان_اگر

bool Curl_is_ipv4addr(ثابت حرف *آدرس);
bool Curl_is_ipaddr(ثابت حرف *آدرس);
bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          بنية Curl_str *host, بنية Curl_str *zone);

CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t پورت,
                       بنية Curl_addrinfo **addrp);

#اگر_تعریف_شده USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ثابت حرف *مسیر, bool abstract,
                        بنية Curl_addrinfo **paddr);
#پایان_اگر

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
فراغ curl_dbg_freeaddrinfo(بنية addrinfo *freethis, صحيح سطر,
                           ثابت حرف *منبع);
#پایان_اگر

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
صحيح curl_dbg_getaddrinfo(ثابت حرف *hostname, ثابت حرف *service,
                         ثابت بنية addrinfo *hints,
                         بنية addrinfo **نتیجه, صحيح سطر,
                         ثابت حرف *منبع);
#پایان_اگر

#اگر_تعریف_شده HAVE_GETADDRINFO
#اگر_تعریف_شده USE_RESOLVE_ON_IPS
فراغ Curl_addrinfo_مجموعه_پورت(بنية Curl_addrinfo *addrinfo, صحيح پورت);
#وإلا
#تعریف_کن Curl_addrinfo_مجموعه_پورت(x, y)
#پایان_اگر
#پایان_اگر

#پایان_اگر /* HEADER_CURL_ADDRINFO_H */
