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
#अन्तर्भाव "curl_setup.h"

#ifdef _WIN32
ब_अक्षर_प्रकार *curlx_wcsdup(स्थिरांक ब_अक्षर_प्रकार *src);  /* for curlx_tcsdup() */
#endif
शून्य *curlx_memdup(स्थिरांक शून्य *src, माप_प्रकार length);
शून्य *curlx_memdup0(स्थिरांक अक्षर *src, माप_प्रकार length);
#endif /* HEADER_CURLX_STRDUP_H */
