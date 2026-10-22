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
#அந்தர்பாவ "curl_setup.h"

#ifdef _WIN32
wchar_t *curlx_wcsdup(மாறிலி wchar_t *src);  /* for curlx_tcsdup() */
#endif
வெறுமை *curlx_memdup(மாறிலி வெறுமை *src, size_t length);
வெறுமை *curlx_memdup0(மாறிலி எழுத்து *src, size_t length);
#endif /* HEADER_CURLX_STRDUP_H */
