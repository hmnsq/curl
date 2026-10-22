#جيڪڏهن_وصف_نه_ٿيل سرو_FAKE_ADDRINFO_H
#وصف_ڪريو سرو_FAKE_ADDRINFO_H
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

#جيڪڏهن_وصف_ٿيل USE_ARES
#ضمن <ares.h>
#جيڪڏهن_پورو

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_نسخو >= 0x011a00) /* >= 1.26.0 */
#وصف_ڪريو USE_FAKE_GETADDRINFO 1
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_FAKE_GETADDRINFO

#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#  include <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#جيڪڏهن_پورو

فراغ r_freeaddrinfo(بنية addrinfo *cahead);
صحيح r_getaddrinfo(ثابت حرف *ڳنڍ,
                  ثابت حرف *service,
                  ثابت بنية addrinfo *hints,
                  بنية addrinfo **res);
#جيڪڏهن_پورو /* USE_FAKE_GETADDRINFO */

#جيڪڏهن_پورو /* HEADER_FAKE_ADDRINFO_H */
