#अगर_अघोषित HEADER_CURLX_WINAPI_H
#घोषणा HEADER_CURLX_WINAPI_H
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

#अगर_घोषित _WIN32
#घोषणा WINAPI_ERROR_LEN 100
स्थिर अक्षर *curlx_get_winapi_error(DWORD err, अक्षर *buf, माप_प्रकार buflen);
स्थिर अक्षर *curlx_winapi_strerror(DWORD err, अक्षर *buf, माप_प्रकार buflen);
CURLcode curlx_win32_random(अचिन्हित अक्षर *entropy, माप_प्रकार लंबाई);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURLX_WINAPI_H */
