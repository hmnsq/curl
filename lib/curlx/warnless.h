#నిర్వచితం_కాకపోతే శీర్షిక_CURL_WARNLESS_H
#నిర్వచించు శీర్షిక_CURL_WARNLESS_H
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

#నిర్వచించు CURLX_FUNCTION_CAST(లక్ష్యం_type, func) \
  (లక్ష్యం_type)(శూన్య (*)(శూన్య))(func)

గుర్తులేని అక్షరం curlx_ultouc(గుర్తులేని దీర్ఘ ulnum);

పూర్ణాంకం curlx_uztosi(size_t uznum);

గుర్తులేని దీర్ఘ curlx_uztoul(size_t uznum);

గుర్తులేని పూర్ణాంకం curlx_uztoui(size_t uznum);

పూర్ణాంకం curlx_sltosi(దీర్ఘ slnum);

గుర్తులేని పూర్ణాంకం curlx_sltoui(దీర్ఘ slnum);

గుర్తులేని పొట్టి curlx_sltous(దీర్ఘ slnum);

ssize_t curlx_uztosz(size_t uznum);

size_t curlx_sotouz(curl_off_t sonum);

పూర్ణాంకం curlx_sztosi(ssize_t sznum);

గుర్తులేని పొట్టి curlx_uitous(గుర్తులేని పూర్ణాంకం uinum);

size_t curlx_sitouz(పూర్ణాంకం sinum);

size_t curlx_uitouz(గుర్తులేని పూర్ణాంకం uinum);

/* Convert a curl_off_t to fit into size_t interval [uzmin, uzmax].
 * values outside this interval give the lower/upper bound. */
size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax);

/* Convert a size_t to curl_off_t, return CURL_OFF_T_MAX if too large. */
curl_off_t curlx_uztoso(size_t uznum);

/* Convert a ssize_t to size_t, return FALSE if negative and set 0 */
bool curlx_sztouz(ssize_t sznum, size_t *puznum);

/* Convert a curl_off_t to size_t, return FALSE if negative or
 * too large and set 0 */
bool curlx_sotouz_సరిపోతుంది(curl_off_t sonum, size_t *puznum);

/* Convert a long to size_t, return FALSE if negative or too large
 * and set 0 */
bool curlx_sltouz(దీర్ఘ slnum, size_t *puznum);

#అయితే_ముగింపు /* HEADER_CURL_WARNLESS_H */
