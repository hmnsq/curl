#నిర్వచితం_కాకపోతే శీర్షిక_CURLX_WINAPI_H
#నిర్వచించు శీర్షిక_CURLX_WINAPI_H
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

#నిర్వచితమైతే _WIN32
#నిర్వచించు WINAPI_ERROR_LEN 100
స్థిరరాశి అక్షరం *curlx_get_winapi_error(DWORD err, అక్షరం *buf, size_t buflen);
స్థిరరాశి అక్షరం *curlx_winapi_strerror(DWORD err, అక్షరం *buf, size_t buflen);
CURLcode curlx_win32_random(గుర్తులేని అక్షరం *entropy, size_t నిడివి);
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURLX_WINAPI_H */
