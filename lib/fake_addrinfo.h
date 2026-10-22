#که_نه_تعریف_شوی سرلیک_FAKE_ADDRINFO_H
#تعریف_کړه سرلیک_FAKE_ADDRINFO_H
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

#که_تعریف_شوی USE_ARES
#ضمن <ares.h>
#د_که_پای

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_نسخه >= 0x011a00) /* >= 1.26.0 */
#تعریف_کړه USE_FAKE_GETADDRINFO 1
#د_که_پای

#که_تعریف_شوی USE_FAKE_GETADDRINFO

#که_تعریف_شوی HAVE_NETDB_H
#  include <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#د_که_پای

فراغ r_freeaddrinfo(بنية addrinfo *cahead);
صحيح r_getaddrinfo(ثابت حرف *غوټه,
                  ثابت حرف *service,
                  ثابت بنية addrinfo *hints,
                  بنية addrinfo **res);
#د_که_پای /* USE_FAKE_GETADDRINFO */

#د_که_پای /* HEADER_FAKE_ADDRINFO_H */
