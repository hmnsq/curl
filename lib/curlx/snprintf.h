#నిర్వచితం_కాకపోతే శీర్షిక_CURLX_SNPRINTF_H
#నిర్వచించు శీర్షిక_CURLX_SNPRINTF_H
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

/* Raw snprintf() for curlx */

#నిర్వచితమైతే _WIN32
శూన్య curlx_win32_snprintf(అక్షరం *buf, size_t maxlen, స్థిరరాశి అక్షరం *fmt, ...)
  CURL_PRINTF(3, 4);
#అయితే_ముగింపు

#నిర్వచితమైతే WITHOUT_LIBCURL /* when built for the test servers */
#నిర్వచితమైతే _WIN32
#నిర్వచించు SNPRINTF curlx_win32_snprintf
#లేకపోతే
#నిర్వచించు SNPRINTF snprintf
#అయితే_ముగింపు
#లేకపోతే /* !WITHOUT_LIBCURL */
#అన్తర్భావ <curl/mprintf.h>
#నిర్వచించు SNPRINTF curl_msnprintf
#అయితే_ముగింపు /* WITHOUT_LIBCURL */
#అయితే_ముగింపు /* HEADER_CURLX_SNPRINTF_H */
