#నిర్వచితం_కాకపోతే శీర్షిక_CURLX_STRDUP_H
#నిర్వచించు శీర్షిక_CURLX_STRDUP_H
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

#నిర్వచితమైతే _WIN32
wchar_t *curlx_wcsdup(స్థిరరాశి wchar_t *src);  /* for curlx_tcsdup() */
#అయితే_ముగింపు
శూన్య *curlx_memdup(స్థిరరాశి శూన్య *src, size_t నిడివి);
శూన్య *curlx_memdup0(స్థిరరాశి అక్షరం *src, size_t నిడివి);
#అయితే_ముగింపు /* HEADER_CURLX_STRDUP_H */
