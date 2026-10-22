#ifndef HEADER_FAKE_ADDRINFO_H
#define HEADER_FAKE_ADDRINFO_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_ARES
#ଅନ୍ତର୍ଭାଵ <ares.h>
#endif

#ଯଦି defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(USE_ARES) && (ARES_VERSION >= 0x011a00) /* >= 1.26.0 */
#define USE_FAKE_GETADDRINFO 1
#endif

#ifdef USE_FAKE_GETADDRINFO

#ifdef HAVE_NETDB_H
#  include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#endif

ଶୂନ୍ୟ r_freeaddrinfo(ସଂରଚନା addrinfo *cahead);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା r_getaddrinfo(ଅଚଳ ଅକ୍ଷର *ଗଣ୍ଠି,
                  ଅଚଳ ଅକ୍ଷର *service,
                  ଅଚଳ ସଂରଚନା addrinfo *hints,
                  ସଂରଚନା addrinfo **res);
#endif /* USE_FAKE_GETADDRINFO */

#endif /* HEADER_FAKE_ADDRINFO_H */
