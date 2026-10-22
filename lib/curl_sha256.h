#ifndef HEADER_CURL_SHA256_H
#define HEADER_CURL_SHA256_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Florin Petriuc, <petriuc.florin@gmail.com>
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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#അന്തര്ഭാവ "curl_hmac.h"

ബാഹ്യ സ്ഥിരം ഘടന HMAC_params Curl_HMAC_SHA256;

#ifndef CURL_SHA256_DIGEST_LENGTH
#define CURL_SHA256_DIGEST_LENGTH 32 /* fixed size */
#endif

CURLcode Curl_sha256it(unsigned അക്ഷരം *output, സ്ഥിരം unsigned അക്ഷരം *നിവേശം,
                       size_t len);

#endif

#endif /* HEADER_CURL_SHA256_H */
