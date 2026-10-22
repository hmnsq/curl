#अगर_अघोषित HEADER_CURL_WARNLESS_H
#घोषणा HEADER_CURL_WARNLESS_H
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

#घोषणा CURLX_FUNCTION_CAST(target_type, func) \
  (target_type)(व्योम (*)(व्योम))(func)

अचिन्हित अक्षर curlx_ultouc(अचिन्हित दीर्घ ulnum);

पूर्णांक curlx_uztosi(माप_प्रकार uznum);

अचिन्हित दीर्घ curlx_uztoul(माप_प्रकार uznum);

अचिन्हित पूर्णांक curlx_uztoui(माप_प्रकार uznum);

पूर्णांक curlx_sltosi(दीर्घ slnum);

अचिन्हित पूर्णांक curlx_sltoui(दीर्घ slnum);

अचिन्हित लघु curlx_sltous(दीर्घ slnum);

ssize_t curlx_uztosz(माप_प्रकार uznum);

माप_प्रकार curlx_sotouz(curl_off_t sonum);

पूर्णांक curlx_sztosi(ssize_t sznum);

अचिन्हित लघु curlx_uitous(अचिन्हित पूर्णांक uinum);

माप_प्रकार curlx_sitouz(पूर्णांक sinum);

माप_प्रकार curlx_uitouz(अचिन्हित पूर्णांक uinum);

/* Convert a curl_off_t to fit into size_t interval [uzmin, uzmax].
 * values outside this interval give the lower/upper bound. */
माप_प्रकार curlx_sotouz_range(curl_off_t sonum, माप_प्रकार uzmin, माप_प्रकार uzmax);

/* Convert a size_t to curl_off_t, return CURL_OFF_T_MAX if too large. */
curl_off_t curlx_uztoso(माप_प्रकार uznum);

/* Convert a ssize_t to size_t, return FALSE if negative and set 0 */
bool curlx_sztouz(ssize_t sznum, माप_प्रकार *puznum);

/* Convert a curl_off_t to size_t, return FALSE if negative or
 * too large and set 0 */
bool curlx_sotouz_fits(curl_off_t sonum, माप_प्रकार *puznum);

/* Convert a long to size_t, return FALSE if negative or too large
 * and set 0 */
bool curlx_sltouz(दीर्घ slnum, माप_प्रकार *puznum);

#पूर्ण_अगर /* HEADER_CURL_WARNLESS_H */
