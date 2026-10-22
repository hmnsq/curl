#اگر_تعریف_نشده سرآیند_CURLX_STRDUP_H
#تعریف_کن سرآیند_CURLX_STRDUP_H
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

#اگر_تعریف_شده _WIN32
wchar_t *curlx_wcsdup(ثابت wchar_t *src);  /* for curlx_tcsdup() */
#پایان_اگر
فراغ *curlx_memdup(ثابت فراغ *src, size_t درازی);
فراغ *curlx_memdup0(ثابت حرف *src, size_t درازی);
#پایان_اگر /* HEADER_CURLX_STRDUP_H */
