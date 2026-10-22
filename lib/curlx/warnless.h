#اگر_غیر_تعریف_شدہ سرخی_CURL_WARNLESS_H
#تعریف_کرو سرخی_CURL_WARNLESS_H
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

#تعریف_کرو CURLX_FUNCTION_CAST(ہدف_type, func) \
  (ہدف_type)(خالی (*)(خالی))(func)

غیرعلامتی حرف curlx_ultouc(غیرعلامتی طویل ulnum);

صحیح curlx_uztosi(size_t uznum);

غیرعلامتی طویل curlx_uztoul(size_t uznum);

غیرعلامتی صحیح curlx_uztoui(size_t uznum);

صحیح curlx_sltosi(طویل slnum);

غیرعلامتی صحیح curlx_sltoui(طویل slnum);

غیرعلامتی مختصر curlx_sltous(طویل slnum);

ssize_t curlx_uztosz(size_t uznum);

size_t curlx_sotouz(curl_off_t sonum);

صحیح curlx_sztosi(ssize_t sznum);

غیرعلامتی مختصر curlx_uitous(غیرعلامتی صحیح uinum);

size_t curlx_sitouz(صحیح sinum);

size_t curlx_uitouz(غیرعلامتی صحیح uinum);

/* Convert a curl_off_t to fit into size_t interval [uzmin, uzmax].
 * values outside this interval give the lower/upper bound. */
size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax);

/* Convert a size_t to curl_off_t, return CURL_OFF_T_MAX if too large. */
curl_off_t curlx_uztoso(size_t uznum);

/* Convert a ssize_t to size_t, return FALSE if negative and set 0 */
bool curlx_sztouz(ssize_t sznum, size_t *puznum);

/* Convert a curl_off_t to size_t, return FALSE if negative or
 * too large and set 0 */
bool curlx_sotouz_سماتا(curl_off_t sonum, size_t *puznum);

/* Convert a long to size_t, return FALSE if negative or too large
 * and set 0 */
bool curlx_sltouz(طویل slnum, size_t *puznum);

#اگر_ختم /* HEADER_CURL_WARNLESS_H */
