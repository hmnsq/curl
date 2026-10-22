#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HOSTCHECK_H
#నిర్వచించు శీర్షిక_CURL_HOSTCHECK_H
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

#ఒకవేళ defined(USE_OPENSSL) || defined(USE_SCHANNEL)

/* returns TRUE if there is a match */
bool Curl_cert_hostcheck(స్థిరరాశి అక్షరం *సరిపోలిక, size_t matchlen,
                         స్థిరరాశి అక్షరం *hostname, size_t hostlen);

#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_HOSTCHECK_H */
