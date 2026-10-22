#अगर_अघोषित HEADER_CURLX_STRDUP_H
#घोषणा HEADER_CURLX_STRDUP_H
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

#अगर_घोषित _WIN32
ब_अक्षर_प्रकार *curlx_wcsdup(स्थिर ब_अक्षर_प्रकार *src);  /* for curlx_tcsdup() */
#पूर्ण_अगर
व्योम *curlx_memdup(स्थिर व्योम *src, माप_प्रकार लंबाई);
व्योम *curlx_memdup0(स्थिर अक्षर *src, माप_प्रकार लंबाई);
#पूर्ण_अगर /* HEADER_CURLX_STRDUP_H */
