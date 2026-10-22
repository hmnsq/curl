#که_نه_تعریف_شوی سرلیک_CURL_WARNLESS_H
#تعریف_کړه سرلیک_CURL_WARNLESS_H
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

#تعریف_کړه CURLX_FUNCTION_CAST(موخه_type, func) \
  (موخه_type)(فراغ (*)(فراغ))(func)

بینښه حرف curlx_ultouc(بینښه اوږد ulnum);

صحيح curlx_uztosi(size_t uznum);

بینښه اوږد curlx_uztoul(size_t uznum);

بینښه صحيح curlx_uztoui(size_t uznum);

صحيح curlx_sltosi(اوږد slnum);

بینښه صحيح curlx_sltoui(اوږد slnum);

بینښه لنډ curlx_sltous(اوږد slnum);

ssize_t curlx_uztosz(size_t uznum);

size_t curlx_sotouz(curl_off_t sonum);

صحيح curlx_sztosi(ssize_t sznum);

بینښه لنډ curlx_uitous(بینښه صحيح uinum);

size_t curlx_sitouz(صحيح sinum);

size_t curlx_uitouz(بینښه صحيح uinum);

/* Convert a curl_off_t to fit into size_t interval [uzmin, uzmax].
 * values outside this interval give the lower/upper bound. */
size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax);

/* Convert a size_t to curl_off_t, return CURL_OFF_T_MAX if too large. */
curl_off_t curlx_uztoso(size_t uznum);

/* Convert a ssize_t to size_t, return FALSE if negative and set 0 */
bool curlx_sztouz(ssize_t sznum, size_t *puznum);

/* Convert a curl_off_t to size_t, return FALSE if negative or
 * too large and set 0 */
bool curlx_sotouz_ځاییږي(curl_off_t sonum, size_t *puznum);

/* Convert a long to size_t, return FALSE if negative or too large
 * and set 0 */
bool curlx_sltouz(اوږد slnum, size_t *puznum);

#د_که_پای /* HEADER_CURL_WARNLESS_H */
