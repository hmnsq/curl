#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SHA256_H
#నిర్వచించు శీర్షిక_CURL_SHA256_H
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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#అన్తర్భావ "curl_hmac.h"

బాహ్య స్థిరరాశి నిర్మాణం HMAC_params Curl_HMAC_SHA256;

#నిర్వచితం_కాకపోతే CURL_SHA256_DIGEST_నిడివి
#నిర్వచించు CURL_SHA256_DIGEST_నిడివి 32 /* fixed size */
#అయితే_ముగింపు

CURLcode Curl_sha256it(గుర్తులేని అక్షరం *output, స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                       size_t len);

#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_SHA256_H */
