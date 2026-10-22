#నిర్వచితం_కాకపోతే శీర్షిక_CURL_PARSEDATE_H
#నిర్వచించు శీర్షిక_CURL_PARSEDATE_H
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
బాహ్య స్థిరరాశి అక్షరం * స్థిరరాశి Curl_wkday[7];
బాహ్య స్థిరరాశి అక్షరం * స్థిరరాశి Curl_month[12];

/* Curl_getdate_capped() differs from curl_getdate() in that this returns
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. */

పూర్ణాంకం Curl_getdate_capped(స్థిరరాశి అక్షరం *p, time_t *tp);

#అయితే_ముగింపు /* HEADER_CURL_PARSEDATE_H */
