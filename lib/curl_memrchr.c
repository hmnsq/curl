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

#समावेश "curl_memrchr.h"

#अगर_अघोषित HAVE_MEMRCHR
/*
 * Curl_memrchr()
 *
 * Our memrchr() function clone for systems which lack this function. The
 * memrchr() function is like the memchr() function, except that it searches
 * backwards from the end of the n bytes pointed to by s instead of forward
 * from the beginning.
 */
व्योम *Curl_memrchr(स्थिर व्योम *s, पूर्णांक c, माप_प्रकार n)
{
  स्थिर अचिन्हित अक्षर *p = s;

  जबतक(n) {
    अगर(p[--n] == (अचिन्हित अक्षर)c)
      वापस CURL_UNCONST(p + n);
  }
  वापस शून्य;
}
#पूर्ण_अगर /* HAVE_MEMRCHR */
