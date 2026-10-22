#अगर_अघोषित HEADER_CURLX_SNPRINTF_H
#घोषणा HEADER_CURLX_SNPRINTF_H
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
#समावेश "curl_setup.h"

/* Raw snprintf() for curlx */

#अगर_घोषित _WIN32
व्योम curlx_win32_snprintf(अक्षर *buf, माप_प्रकार maxlen, स्थिर अक्षर *fmt, ...)
  CURL_PRINTF(3, 4);
#पूर्ण_अगर

#अगर_घोषित WITHOUT_LIBCURL /* when built for the test servers */
#अगर_घोषित _WIN32
#घोषणा SNPRINTF curlx_win32_snprintf
#अन्यथा
#घोषणा SNPRINTF snprintf
#पूर्ण_अगर
#अन्यथा /* !WITHOUT_LIBCURL */
#समावेश <curl/mprintf.h>
#घोषणा SNPRINTF curl_msnprintf
#पूर्ण_अगर /* WITHOUT_LIBCURL */
#पूर्ण_अगर /* HEADER_CURLX_SNPRINTF_H */
