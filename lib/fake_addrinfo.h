#اگر_غیر_تعریف_شدہ سرخی_FAKE_ADDRINFO_H
#تعریف_کرو سرخی_FAKE_ADDRINFO_H
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

#اگر_تعریف_شدہ USE_ARES
#شامل <ares.h>
#اگر_ختم

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_نسخہ >= 0x011a00) /* >= 1.26.0 */
#تعریف_کرو USE_FAKE_GETADDRINFO 1
#اگر_ختم

#اگر_تعریف_شدہ USE_FAKE_GETADDRINFO

#اگر_تعریف_شدہ HAVE_NETDB_H
#  include <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#اگر_ختم

خالی r_freeaddrinfo(ساخت addrinfo *cahead);
صحیح r_getaddrinfo(ثابت حرف *گرہ,
                  ثابت حرف *service,
                  ثابت ساخت addrinfo *hints,
                  ساخت addrinfo **res);
#اگر_ختم /* USE_FAKE_GETADDRINFO */

#اگر_ختم /* HEADER_FAKE_ADDRINFO_H */
