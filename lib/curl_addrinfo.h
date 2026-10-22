#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ADDRINFO_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ADDRINFO_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#  include <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#  include <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#  include <in.h>
#  include <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

ರಚನೆ Curl_addrinfo {
  ಪೂರ್ಣಾಂಕ                   ai_flags;
  ಪೂರ್ಣಾಂಕ                   ai_family;
  ಪೂರ್ಣಾಂಕ                   ai_socktype;
  ಪೂರ್ಣಾಂಕ                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  ಅಕ್ಷರ                 *ai_canonname;
  ರಚನೆ sockaddr      *ai_addr;
  ರಚನೆ Curl_addrinfo *ai_ಮುಂದಿನ;
};

ಶೂನ್ಯ Curl_freeaddrinfo(ರಚನೆ Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
ರಚನೆ Curl_addrinfo *Curl_addrinfo_get(ರಚನೆ Curl_addrinfo *ai,
                                        ಪೂರ್ಣಾಂಕ ai_family,
                                        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETADDRINFO
ಪೂರ್ಣಾಂಕ Curl_getaddrinfo_ex(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nodename,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *servname,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *hints,
                        ರಚನೆ Curl_addrinfo **ಫಲಿತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ರಚನೆ Curl_addrinfo *Curl_he2ai(ಸ್ಥಿರಾಂಕ ರಚನೆ hostent *he, ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್);
#ಆದರೆ_ಅಂತ್ಯ

bool Curl_is_ipv4addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಳಾಸ);
bool Curl_is_ipaddr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಳಾಸ);
bool Curl_looks_like_ipv6(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, bool maybe_url_encoded,
                          ರಚನೆ Curl_str *host, ರಚನೆ Curl_str *zone);

CURLcode Curl_str2addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dotted, uint16_t ಪೋರ್ಟ್,
                       ರಚನೆ Curl_addrinfo **addrp);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool abstract,
                        ರಚನೆ Curl_addrinfo **paddr);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
ಶೂನ್ಯ curl_dbg_freeaddrinfo(ರಚನೆ addrinfo *freethis, ಪೂರ್ಣಾಂಕ ಸಾಲು,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
ಪೂರ್ಣಾಂಕ curl_dbg_getaddrinfo(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service,
                         ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *hints,
                         ರಚನೆ addrinfo **ಫಲಿತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸಾಲು,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETADDRINFO
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLVE_ON_IPS
ಶೂನ್ಯ Curl_addrinfo_ಗಣ_ಪೋರ್ಟ್(ರಚನೆ Curl_addrinfo *addrinfo, ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_addrinfo_ಗಣ_ಪೋರ್ಟ್(x, y)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_ADDRINFO_H */
