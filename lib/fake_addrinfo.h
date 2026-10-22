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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_ARES
#அந்தர்பாவ <ares.h>
#endif

#எனில் defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
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

வெறுமை r_freeaddrinfo(கட்டமைப்பு addrinfo *cahead);
முழு r_getaddrinfo(மாறிலி எழுத்து *முனை,
                  மாறிலி எழுத்து *service,
                  மாறிலி கட்டமைப்பு addrinfo *hints,
                  கட்டமைப்பு addrinfo **res);
#endif /* USE_FAKE_GETADDRINFO */

#endif /* HEADER_FAKE_ADDRINFO_H */
