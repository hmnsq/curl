#यदि_अपरिभाषितम् शीर्षकम्_CURLX_WINAPI_H
#परिभाषय शीर्षकम्_CURLX_WINAPI_H
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

#यदि_परिभाषितम् _WIN32
#परिभाषय WINAPI_ERROR_LEN 100
ध्रुव वर्ण *curlx_get_winapi_error(DWORD err, वर्ण *buf, size_t buflen);
ध्रुव वर्ण *curlx_winapi_strerror(DWORD err, वर्ण *buf, size_t buflen);
CURLcode curlx_win32_random(अचिह्नित वर्ण *entropy, size_t दीर्घता);
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURLX_WINAPI_H */
