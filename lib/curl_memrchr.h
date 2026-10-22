#अगर_अघोषित HEADER_CURL_MEMRCHR_H
#घोषणा HEADER_CURL_MEMRCHR_H
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

#अगर_घोषित HAVE_MEMRCHR

#अगर_घोषित HAVE_STRINGS_H
#  include <strings.h>
#पूर्ण_अगर

#अन्यथा /* HAVE_MEMRCHR */
व्योम *Curl_memrchr(स्थिर व्योम *s, पूर्णांक c, माप_प्रकार n);
#घोषणा memrchr(x, y, z)  Curl_memrchr(x, y, z)

#पूर्ण_अगर /* HAVE_MEMRCHR */

#पूर्ण_अगर /* HEADER_CURL_MEMRCHR_H */
