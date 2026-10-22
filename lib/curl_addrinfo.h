#اگر_غیر_تعریف_شدہ سرخی_CURL_ADDRINFO_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#  include <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#  include <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#اگر_ختم

#اگر_تعریف_شدہ __VMS
#  include <in.h>
#  include <inet.h>
#اگر_ختم

ساخت Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

ساخت Curl_addrinfo {
  صحیح                   ai_flags;
  صحیح                   ai_family;
  صحیح                   ai_socktype;
  صحیح                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  حرف                 *ai_canonname;
  ساخت sockaddr      *ai_addr;
  ساخت Curl_addrinfo *ai_اگلا;
};

خالی Curl_freeaddrinfo(ساخت Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
ساخت Curl_addrinfo *Curl_addrinfo_get(ساخت Curl_addrinfo *ai,
                                        صحیح ai_family,
                                        غیرعلامتی صحیح n);

#اگر_تعریف_شدہ HAVE_GETADDRINFO
صحیح Curl_getaddrinfo_ex(ثابت حرف *nodename,
                        ثابت حرف *servname,
                        ثابت ساخت addrinfo *hints,
                        ساخت Curl_addrinfo **نتیجہ);
#اگر_ختم

#اگر !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ساخت Curl_addrinfo *Curl_he2ai(ثابت ساخت hostent *he, صحیح پورٹ);
#اگر_ختم

bool Curl_is_ipv4addr(ثابت حرف *پتہ);
bool Curl_is_ipaddr(ثابت حرف *پتہ);
bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          ساخت Curl_str *host, ساخت Curl_str *zone);

CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t پورٹ,
                       ساخت Curl_addrinfo **addrp);

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ثابت حرف *راستہ, bool abstract,
                        ساخت Curl_addrinfo **paddr);
#اگر_ختم

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
خالی curl_dbg_freeaddrinfo(ساخت addrinfo *freethis, صحیح سطر,
                           ثابت حرف *ماخذ);
#اگر_ختم

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
صحیح curl_dbg_getaddrinfo(ثابت حرف *hostname, ثابت حرف *service,
                         ثابت ساخت addrinfo *hints,
                         ساخت addrinfo **نتیجہ, صحیح سطر,
                         ثابت حرف *ماخذ);
#اگر_ختم

#اگر_تعریف_شدہ HAVE_GETADDRINFO
#اگر_تعریف_شدہ USE_RESOLVE_ON_IPS
خالی Curl_addrinfo_سیٹ_پورٹ(ساخت Curl_addrinfo *addrinfo, صحیح پورٹ);
#ورنہ
#تعریف_کرو Curl_addrinfo_سیٹ_پورٹ(x, y)
#اگر_ختم
#اگر_ختم

#اگر_ختم /* HEADER_CURL_ADDRINFO_H */
