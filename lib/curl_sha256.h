#यदि_अपरिभाषितम् शीर्षकम्_CURL_SHA256_H
#परिभाषय शीर्षकम्_CURL_SHA256_H
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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#अन्तर्भाव "curl_hmac.h"

बाह्य ध्रुव संरचना HMAC_params Curl_HMAC_SHA256;

#यदि_अपरिभाषितम् CURL_SHA256_DIGEST_दीर्घता
#परिभाषय CURL_SHA256_DIGEST_दीर्घता 32 /* fixed size */
#यदि_अन्तः

CURLcode Curl_sha256it(अचिह्नित वर्ण *output, ध्रुव अचिह्नित वर्ण *निवेशः,
                       size_t len);

#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_SHA256_H */
