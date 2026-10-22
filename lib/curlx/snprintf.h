#اگر_غیر_تعریف_شدہ سرخی_CURLX_SNPRINTF_H
#تعریف_کرو سرخی_CURLX_SNPRINTF_H
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
#شامل "curl_setup.h"

/* Raw snprintf() for curlx */

#اگر_تعریف_شدہ _WIN32
خالی curlx_win32_snprintf(حرف *buf, size_t maxlen, ثابت حرف *fmt, ...)
  CURL_PRINTF(3, 4);
#اگر_ختم

#اگر_تعریف_شدہ WITHOUT_LIBCURL /* when built for the test servers */
#اگر_تعریف_شدہ _WIN32
#تعریف_کرو SNPRINTF curlx_win32_snprintf
#ورنہ
#تعریف_کرو SNPRINTF snprintf
#اگر_ختم
#ورنہ /* !WITHOUT_LIBCURL */
#شامل <curl/mprintf.h>
#تعریف_کرو SNPRINTF curl_msnprintf
#اگر_ختم /* WITHOUT_LIBCURL */
#اگر_ختم /* HEADER_CURLX_SNPRINTF_H */
