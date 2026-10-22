#جے_بنا_تعریف سرخی_CURLX_SNPRINTF_H
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
#ضمن "curl_setup.h"

/* Raw snprintf() for curlx */

#جے_تعریف_شدہ _WIN32
فراغ curlx_win32_snprintf(حرف *buf, size_t maxlen, ثابت حرف *fmt, ...)
  CURL_PRINTF(3, 4);
#جے_انت

#جے_تعریف_شدہ WITHOUT_LIBCURL /* when built for the test servers */
#جے_تعریف_شدہ _WIN32
#تعریف_کرو SNPRINTF curlx_win32_snprintf
#وإلا
#تعریف_کرو SNPRINTF snprintf
#جے_انت
#وإلا /* !WITHOUT_LIBCURL */
#ضمن <curl/mprintf.h>
#تعریف_کرو SNPRINTF curl_msnprintf
#جے_انت /* WITHOUT_LIBCURL */
#جے_انت /* HEADER_CURLX_SNPRINTF_H */
