#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_FAKE_ADDRINFO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_FAKE_ADDRINFO_H
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
#ਅਨ੍ਤਰ੍ਭਾਵ <ares.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_ਸੰਸਕਰਣ >= 0x011a00) /* >= 1.26.0 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_FAKE_GETADDRINFO 1
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FAKE_GETADDRINFO

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#  include <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#ਜੇ_ਅੰਤ

ਖਾਲੀ r_freeaddrinfo(ਬਣਤਰ addrinfo *cahead);
ਪੂਰਨ r_getaddrinfo(ਅਚਲ ਅੱਖਰ *ਗੰਢ,
                  ਅਚਲ ਅੱਖਰ *service,
                  ਅਚਲ ਬਣਤਰ addrinfo *hints,
                  ਬਣਤਰ addrinfo **res);
#ਜੇ_ਅੰਤ /* USE_FAKE_GETADDRINFO */

#ਜੇ_ਅੰਤ /* HEADER_FAKE_ADDRINFO_H */
