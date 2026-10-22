#నిర్వచితం_కాకపోతే శీర్షిక_CURL_ED25519_H
#నిర్వచించు శీర్షిక_CURL_ED25519_H
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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#నిర్వచించు CURL_ED25519_SIGLEN 64
#నిర్వచించు CURL_ED25519_KEYLEN 32

/* Sign with Ed25519 (RFC 8032).
 * key/keylen: raw 32-byte private seed
 * msg/msglen: data to sign
 * sig: output buffer (at least CURL_ED25519_SIGLEN bytes)
 * siglen: out - actual signature length on success
 * Returns CURLE_OK or CURLE_NOT_BUILT_IN if no backend supports Ed25519. */
CURLcode Curl_ed25519_sign(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, size_t keylen,
                           స్థిరరాశి గుర్తులేని అక్షరం *సందేశం, size_t msglen,
                           గుర్తులేని అక్షరం *sig, size_t *siglen);

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
#అయితే_ముగింపు /* HEADER_CURL_ED25519_H */
