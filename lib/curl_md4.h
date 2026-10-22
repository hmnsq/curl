#اگر_تعریف_نشده سرآیند_CURL_MD4_H
#تعریف_کن سرآیند_CURL_MD4_H
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

#اگر_تعریف_شده USE_CURL_NTLM_CORE

#تعریف_کن MD4_DIGEST_درازی 16

CURLcode Curl_md4it(بیعلامت حرف *output, ثابت بیعلامت حرف *درون_داد,
                    ثابت size_t len);

#پایان_اگر /* USE_CURL_NTLM_CORE */

#پایان_اگر /* HEADER_CURL_MD4_H */
