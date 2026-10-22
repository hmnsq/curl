#यदि_अपरिभाषितम् शीर्षकम्_CURL_WARNLESS_H
#परिभाषय शीर्षकम्_CURL_WARNLESS_H
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

#परिभाषय CURLX_FUNCTION_CAST(target_type, func) \
  (target_type)(शून्य (*)(शून्य))(func)

अचिह्नित वर्ण curlx_ultouc(अचिह्नित दीर्घ ulnum);

पूर्णाङ्क curlx_uztosi(size_t uznum);

अचिह्नित दीर्घ curlx_uztoul(size_t uznum);

अचिह्नित पूर्णाङ्क curlx_uztoui(size_t uznum);

पूर्णाङ्क curlx_sltosi(दीर्घ slnum);

अचिह्नित पूर्णाङ्क curlx_sltoui(दीर्घ slnum);

अचिह्नित ह्रस्व curlx_sltous(दीर्घ slnum);

ssize_t curlx_uztosz(size_t uznum);

size_t curlx_sotouz(curl_off_t sonum);

पूर्णाङ्क curlx_sztosi(ssize_t sznum);

अचिह्नित ह्रस्व curlx_uitous(अचिह्नित पूर्णाङ्क uinum);

size_t curlx_sitouz(पूर्णाङ्क sinum);

size_t curlx_uitouz(अचिह्नित पूर्णाङ्क uinum);

/* Convert a curl_off_t to fit into size_t interval [uzmin, uzmax].
 * values outside this interval give the lower/upper bound. */
size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax);

/* Convert a size_t to curl_off_t, return CURL_OFF_T_MAX if too large. */
curl_off_t curlx_uztoso(size_t uznum);

/* Convert a ssize_t to size_t, return FALSE if negative and set 0 */
bool curlx_sztouz(ssize_t sznum, size_t *puznum);

/* Convert a curl_off_t to size_t, return FALSE if negative or
 * too large and set 0 */
bool curlx_sotouz_माति(curl_off_t sonum, size_t *puznum);

/* Convert a long to size_t, return FALSE if negative or too large
 * and set 0 */
bool curlx_sltouz(दीर्घ slnum, size_t *puznum);

#यदि_अन्तः /* HEADER_CURL_WARNLESS_H */
