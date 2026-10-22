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

#ifdef _WIN32
#അന്തര്ഭാവ <wchar.h>
#endif

#അന്തര്ഭാവ "curlx/strdup.h"

#ifdef _WIN32
/***************************************************************************
 *
 * curlx_wcsdup(source)
 *
 * Copies the 'source' wchar string to a newly allocated buffer (that is
 * returned). Used by macro curlx_tcsdup().
 *
 * Returns the new pointer or NULL on failure.
 *
 ***************************************************************************/
wchar_t *curlx_wcsdup(സ്ഥിരം wchar_t *src)
{
  size_t length = wcslen(src);

  എങ്കിൽ(length > (SIZE_MAX / വലുപ്പം(wchar_t)) - 1)
    തിരികെ (wchar_t *)ശൂന്യ; /* integer overflow */

  തിരികെ (wchar_t *)curlx_memdup(src, (length + 1) * വലുപ്പം(wchar_t));
}
#endif

/***************************************************************************
 *
 * curlx_memdup(source, length)
 *
 * Copies the 'source' data to a newly allocated buffer (that is
 * returned). Copies 'length' bytes.
 *
 * Returns the new pointer or NULL on failure.
 *
 ***************************************************************************/
ശൂന്യം *curlx_memdup(സ്ഥിരം ശൂന്യം *src, size_t length)
{
  ശൂന്യം *buffer = curlx_malloc(length);
  എങ്കിൽ(!buffer)
    തിരികെ ശൂന്യ; /* fail */

  memcpy(buffer, src, length);

  തിരികെ buffer;
}

/***************************************************************************
 *
 * curlx_memdup0(source, length)
 *
 * Copies the 'source' string to a newly allocated buffer (that is returned).
 * Copies 'length' bytes then adds a null-terminator.
 *
 * Returns the new pointer or NULL on failure.
 *
 ***************************************************************************/
ശൂന്യം *curlx_memdup0(സ്ഥിരം അക്ഷരം *src, size_t length)
{
  അക്ഷരം *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : ശൂന്യ;
  എങ്കിൽ(!buf)
    തിരികെ ശൂന്യ;
  എങ്കിൽ(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  തിരികെ buf;
}

#ifdef USE_CURLX_MEMZERO
സ്ഥിതം ശൂന്യം *(* സ്ഥിരം അസ്ഥിര p_curlx_memset)(ശൂന്യം *buf, പൂർണ്ണസംഖ്യ val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
ശൂന്യം curlx_memzero_താഴെയുള്ള(ശൂന്യം *buf, size_t size)
{
  എങ്കിൽ(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
ശൂന്യം curlx_memzero(ശൂന്യം *buf, size_t size)
{
  എങ്കിൽ(buf)
    curlx_memzero_താഴെയുള്ള(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
ശൂന്യം curlx_strzero(ശൂന്യം *buf)
{
  എങ്കിൽ(buf)
    curlx_memzero_താഴെയുള്ള(buf, strlen(buf));
}
