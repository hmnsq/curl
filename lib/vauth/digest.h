#నిర్వచితం_కాకపోతే శీర్షిక_CURL_DIGEST_H
#నిర్వచించు శీర్షిక_CURL_DIGEST_H
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

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH

#నిర్వచించు DIGEST_MAX_విలువ_నిడివి           256
#నిర్వచించు DIGEST_MAX_CONTENT_నిడివి         1024

నిర్మాణం Curl_str;

/* This is used to extract the realm from a challenge message */
bool Curl_auth_digest_get_pair(స్థిరరాశి అక్షరం *str, నిర్మాణం Curl_str *విలువ,
                               అక్షరం *content, స్థిరరాశి అక్షరం **endptr);

#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_DIGEST_H */
