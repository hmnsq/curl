#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MD4_H
#నిర్వచించు శీర్షిక_CURL_MD4_H
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

#నిర్వచితమైతే USE_CURL_NTLM_CORE

#నిర్వచించు MD4_DIGEST_నిడివి 16

CURLcode Curl_md4it(గుర్తులేని అక్షరం *output, స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                    స్థిరరాశి size_t len);

#అయితే_ముగింపు /* USE_CURL_NTLM_CORE */

#అయితే_ముగింపు /* HEADER_CURL_MD4_H */
