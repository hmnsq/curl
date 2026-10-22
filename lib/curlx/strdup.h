#ifndef HEADER_CURLX_STRDUP_H
#define HEADER_CURLX_STRDUP_H
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
#অন্তর্ভাব "curl_setup.h"

#ifdef _WIN32
wchar_t *curlx_wcsdup(ধ্রুবক wchar_t *src);  /* for curlx_tcsdup() */
#endif
শূন্য *curlx_memdup(ধ্রুবক শূন্য *src, size_t দৈর্ঘ্য);
শূন্য *curlx_memdup0(ধ্রুবক অক্ষর *src, size_t দৈর্ঘ্য);
#endif /* HEADER_CURLX_STRDUP_H */
