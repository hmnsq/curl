#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURLX_SNPRINTF_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURLX_SNPRINTF_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

/* Raw snprintf() for curlx */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
ਖਾਲੀ curlx_win32_snprintf(ਅੱਖਰ *buf, size_t maxlen, ਅਚਲ ਅੱਖਰ *fmt, ...)
  CURL_PRINTF(3, 4);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITHOUT_LIBCURL /* when built for the test servers */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNPRINTF curlx_win32_snprintf
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNPRINTF snprintf
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ /* !WITHOUT_LIBCURL */
#ਅਨ੍ਤਰ੍ਭਾਵ <curl/mprintf.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNPRINTF curl_msnprintf
#ਜੇ_ਅੰਤ /* WITHOUT_LIBCURL */
#ਜੇ_ਅੰਤ /* HEADER_CURLX_SNPRINTF_H */
