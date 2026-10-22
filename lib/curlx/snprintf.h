#جيڪڏهن_وصف_نه_ٿيل سرو_CURLX_SNPRINTF_H
#وصف_ڪريو سرو_CURLX_SNPRINTF_H
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

#جيڪڏهن_وصف_ٿيل _WIN32
فراغ curlx_win32_snprintf(حرف *buf, size_t maxlen, ثابت حرف *fmt, ...)
  CURL_PRINTF(3, 4);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل WITHOUT_LIBCURL /* when built for the test servers */
#جيڪڏهن_وصف_ٿيل _WIN32
#وصف_ڪريو SNPRINTF curlx_win32_snprintf
#وإلا
#وصف_ڪريو SNPRINTF snprintf
#جيڪڏهن_پورو
#وإلا /* !WITHOUT_LIBCURL */
#ضمن <curl/mprintf.h>
#وصف_ڪريو SNPRINTF curl_msnprintf
#جيڪڏهن_پورو /* WITHOUT_LIBCURL */
#جيڪڏهن_پورو /* HEADER_CURLX_SNPRINTF_H */
