#اگر_تعریف_نشده سرآیند_CURLX_SNPRINTF_H
#تعریف_کن سرآیند_CURLX_SNPRINTF_H
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
#ضمن "curl_setup.h"

/* Raw snprintf() for curlx */

#اگر_تعریف_شده _WIN32
فراغ curlx_win32_snprintf(حرف *buf, size_t maxlen, ثابت حرف *fmt, ...)
  CURL_PRINTF(3, 4);
#پایان_اگر

#اگر_تعریف_شده WITHOUT_LIBCURL /* when built for the test servers */
#اگر_تعریف_شده _WIN32
#تعریف_کن SNPRINTF curlx_win32_snprintf
#وإلا
#تعریف_کن SNPRINTF snprintf
#پایان_اگر
#وإلا /* !WITHOUT_LIBCURL */
#ضمن <curl/mprintf.h>
#تعریف_کن SNPRINTF curl_msnprintf
#پایان_اگر /* WITHOUT_LIBCURL */
#پایان_اگر /* HEADER_CURLX_SNPRINTF_H */
