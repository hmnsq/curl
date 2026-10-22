#اگر_غیر_تعریف_شدہ سرخی_CURLX_WINAPI_H
#تعریف_کرو سرخی_CURLX_WINAPI_H
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

#اگر_تعریف_شدہ _WIN32
#تعریف_کرو WINAPI_ERROR_LEN 100
ثابت حرف *curlx_get_winapi_error(DWORD err, حرف *buf, size_t buflen);
ثابت حرف *curlx_winapi_strerror(DWORD err, حرف *buf, size_t buflen);
CURLcode curlx_win32_random(غیرعلامتی حرف *entropy, size_t لمبائی);
#اگر_ختم

#اگر_ختم /* HEADER_CURLX_WINAPI_H */
