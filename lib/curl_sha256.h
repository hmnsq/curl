#अगर_अघोषित HEADER_CURL_SHA256_H
#घोषणा HEADER_CURL_SHA256_H
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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#समावेश "curl_hmac.h"

बाह्य स्थिर काष्ठा HMAC_params Curl_HMAC_SHA256;

#अगर_अघोषित CURL_SHA256_DIGEST_लंबाई
#घोषणा CURL_SHA256_DIGEST_लंबाई 32 /* fixed size */
#पूर्ण_अगर

CURLcode Curl_sha256it(अचिन्हित अक्षर *output, स्थिर अचिन्हित अक्षर *निवेश,
                       माप_प्रकार len);

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_SHA256_H */
