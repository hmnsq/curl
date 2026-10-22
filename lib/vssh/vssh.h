#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VSSH_VSSH_H
#నిర్వచించు శీర్షిక_CURL_VSSH_VSSH_H
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

#నిర్వచితమైతే USE_SSH

#అన్తర్భావ "urldata.h"

నిర్మాణం ssh_conn;

CURLcode Curl_getworkingpath(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి అక్షరం *homedir,
                             అక్షరం **మార్గం);

CURLcode Curl_get_pathname(స్థిరరాశి అక్షరం **cpp, అక్షరం **మార్గం, స్థిరరాశి అక్షరం *homedir);

CURLcode Curl_ssh_range(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం easy_pollset *ps);
CURLcode Curl_ssh_అమరిక_pkey(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ssh_conn *sshc);

#అయితే_ముగింపు /* USE_SSH */
#అయితే_ముగింపు /* HEADER_CURL_VSSH_VSSH_H */
