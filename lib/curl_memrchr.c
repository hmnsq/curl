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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curl_memrchr.h"

#ifndef HAVE_MEMRCHR
/*
 * Curl_memrchr()
 *
 * Our memrchr() function clone for systems which lack this function. The
 * memrchr() function is like the memchr() function, except that it searches
 * backwards from the end of the n bytes pointed to by s instead of forward
 * from the beginning.
 */
ಶೂನ್ಯ *Curl_memrchr(ಧ್ರುವ ಶೂನ್ಯ *s, ಪೂರ್ಣಾಙ್ಕ c, size_t n)
{
  ಧ್ರುವ unsigned ವರ್ಣ *p = s;

  ಯಾವತ್(n) {
    ಯದಿ(p[--n] == (unsigned ವರ್ಣ)c)
      ಪ್ರತ್ಯಾಗಮ CURL_UNCONST(p + n);
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif /* HAVE_MEMRCHR */
