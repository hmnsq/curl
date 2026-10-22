#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SOCKADDR_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SOCKADDR_H
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

ਬਣਤਰ Curl_sockaddr_storage {
  ਸੰਘ {
    ਬਣਤਰ sockaddr sa;
    ਬਣਤਰ sockaddr_in sa_ਵਿੱਚ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਬਣਤਰ sockaddr_in6 sa_in6;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRUCT_SOCKADDR_STORAGE
    ਬਣਤਰ sockaddr_storage sa_stor;
#ਨਹੀਂਤਾਂ
    ਅੱਖਰ cbuf[256];   /* this should be big enough to fit a lot */
#ਜੇ_ਅੰਤ
  } ਬਫਰ;
};

/*
 * The Curl_sockaddr_ex structure is libcurl's external API curl_sockaddr
 * structure with enough space available to directly hold any
 * protocol-specific address structures. The variable declared here will be
 * used to pass / receive data to/from the fopensocket callback if this has
 * been set, before that, it is initialized from parameters.
 */
ਬਣਤਰ Curl_sockaddr_ex {
  ਪੂਰਨ family;
  ਪੂਰਨ socktype;
  ਪੂਰਨ protocol;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ addrlen;
  ਸੰਘ {
    ਬਣਤਰ sockaddr sa;
    ਬਣਤਰ Curl_sockaddr_storage buf;
  } addr;
};

#ਜੇ_ਅੰਤ /* HEADER_CURL_SOCKADDR_H */
