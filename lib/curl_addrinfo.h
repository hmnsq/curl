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
#অন্তর্ভাব "curl_setup.h"

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

কাঠামো Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

কাঠামো Curl_addrinfo {
  পূর্ণসংখ্যা                   ai_flags;
  পূর্ণসংখ্যা                   ai_family;
  পূর্ণসংখ্যা                   ai_socktype;
  পূর্ণসংখ্যা                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  অক্ষর                 *ai_canonname;
  কাঠামো sockaddr      *ai_addr;
  কাঠামো Curl_addrinfo *ai_পরবর্তী;
};

শূন্য Curl_freeaddrinfo(কাঠামো Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
কাঠামো Curl_addrinfo *Curl_addrinfo_get(কাঠামো Curl_addrinfo *ai,
                                        পূর্ণসংখ্যা ai_family,
                                        unsigned পূর্ণসংখ্যা n);

#ifdef HAVE_GETADDRINFO
পূর্ণসংখ্যা Curl_getaddrinfo_ex(ধ্রুবক অক্ষর *nodename,
                        ধ্রুবক অক্ষর *servname,
                        ধ্রুবক কাঠামো addrinfo *hints,
                        কাঠামো Curl_addrinfo **ফলাফল);
#endif

#যদি !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
কাঠামো Curl_addrinfo *Curl_he2ai(ধ্রুবক কাঠামো hostent *he, পূর্ণসংখ্যা port);
#endif

bool Curl_is_ipv4addr(ধ্রুবক অক্ষর *address);
bool Curl_is_ipaddr(ধ্রুবক অক্ষর *address);
bool Curl_looks_like_ipv6(ধ্রুবক অক্ষর *s, size_t len, bool maybe_url_encoded,
                          কাঠামো Curl_str *host, কাঠামো Curl_str *zone);

CURLcode Curl_str2addr(ধ্রুবক অক্ষর *dotted, uint16_t port,
                       কাঠামো Curl_addrinfo **addrp);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ধ্রুবক অক্ষর *path, bool abstract,
                        কাঠামো Curl_addrinfo **paddr);
#endif

#যদি defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
শূন্য curl_dbg_freeaddrinfo(কাঠামো addrinfo *freethis, পূর্ণসংখ্যা সারি,
                           ধ্রুবক অক্ষর *source);
#endif

#যদি defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
পূর্ণসংখ্যা curl_dbg_getaddrinfo(ধ্রুবক অক্ষর *hostname, ধ্রুবক অক্ষর *service,
                         ধ্রুবক কাঠামো addrinfo *hints,
                         কাঠামো addrinfo **ফলাফল, পূর্ণসংখ্যা সারি,
                         ধ্রুবক অক্ষর *source);
#endif

#ifdef HAVE_GETADDRINFO
#ifdef USE_RESOLVE_ON_IPS
শূন্য Curl_addrinfo_set_port(কাঠামো Curl_addrinfo *addrinfo, পূর্ণসংখ্যা port);
#অন্যথা
#define Curl_addrinfo_set_port(x, y)
#endif
#endif

#endif /* HEADER_CURL_ADDRINFO_H */
