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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curl_memrchr.h"

#ifndef HAVE_MEMRCHR
/*
 * Curl_memrchr()
 *
 * Our memrchr() function clone for systems which lack this function. The
 * memrchr() function is like the memchr() function, except that it searches
 * backwards from the end of the n bytes pointed to by s instead of forward
 * from the beginning.
 */
ശൂന്യം *Curl_memrchr(സ്ഥിരം ശൂന്യം *s, പൂർണ്ണസംഖ്യ c, size_t n)
{
  സ്ഥിരം unsigned അക്ഷരം *p = s;

  ആകുമ്പോൾ(n) {
    എങ്കിൽ(p[--n] == (unsigned അക്ഷരം)c)
      തിരികെ CURL_UNCONST(p + n);
  }
  തിരികെ ശൂന്യ;
}
#endif /* HAVE_MEMRCHR */
