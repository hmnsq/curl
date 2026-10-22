#که_نه_تعریف_شوی سرلیک_CURLX_SNPRINTF_H
#تعریف_کړه سرلیک_CURLX_SNPRINTF_H
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

#که_تعریف_شوی _WIN32
فراغ curlx_win32_snprintf(حرف *buf, size_t maxlen, ثابت حرف *fmt, ...)
  CURL_PRINTF(3, 4);
#د_که_پای

#که_تعریف_شوی WITHOUT_LIBCURL /* when built for the test servers */
#که_تعریف_شوی _WIN32
#تعریف_کړه SNPRINTF curlx_win32_snprintf
#وإلا
#تعریف_کړه SNPRINTF snprintf
#د_که_پای
#وإلا /* !WITHOUT_LIBCURL */
#ضمن <curl/mprintf.h>
#تعریف_کړه SNPRINTF curl_msnprintf
#د_که_پای /* WITHOUT_LIBCURL */
#د_که_پای /* HEADER_CURLX_SNPRINTF_H */
