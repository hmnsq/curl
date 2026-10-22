#اگر_تعریف_نشده سرآیند_FAKE_ADDRINFO_H
#تعریف_کن سرآیند_FAKE_ADDRINFO_H
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

#اگر_تعریف_شده USE_ARES
#ضمن <ares.h>
#پایان_اگر

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_نسخه >= 0x011a00) /* >= 1.26.0 */
#تعریف_کن USE_FAKE_GETADDRINFO 1
#پایان_اگر

#اگر_تعریف_شده USE_FAKE_GETADDRINFO

#اگر_تعریف_شده HAVE_NETDB_H
#  include <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#پایان_اگر

فراغ r_freeaddrinfo(بنية addrinfo *cahead);
صحيح r_getaddrinfo(ثابت حرف *گره,
                  ثابت حرف *service,
                  ثابت بنية addrinfo *hints,
                  بنية addrinfo **res);
#پایان_اگر /* USE_FAKE_GETADDRINFO */

#پایان_اگر /* HEADER_FAKE_ADDRINFO_H */
