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

#അന്തര്ഭാവ "curlx/strcopy.h"

/*
 * curlx_strcopy() is a replacement for strcpy.
 *
 * Provide the target buffer @dest and size of the target buffer @dsize, If
 * the source string @src with its *string length* @slen fits in the target
 * buffer it is copied there - including storing a null-terminator.
 *
 * If the target buffer is too small, the copy is not performed but if the
 * target buffer has a non-zero size it gets a null-terminator stored.
 */
ശൂന്യം curlx_strcopy(അക്ഷരം *dest,      /* destination buffer */
                   size_t dsize,    /* size of target buffer */
                   സ്ഥിരം അക്ഷരം *src, /* source string */
                   size_t slen)     /* length of source string to copy */
{
  DEBUGASSERT(slen < dsize);
  എങ്കിൽ(slen < dsize) {
    memcpy(dest, src, slen);
    dest[slen] = 0;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(dsize)
    dest[0] = 0;
}
