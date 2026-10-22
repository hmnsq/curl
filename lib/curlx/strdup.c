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
#ضمن "curl_setup.h"

#ifdef _WIN32
#ضمن <wchar.h>
#endif

#ضمن "curlx/strdup.h"

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
wchar_t *curlx_wcsdup(ثابت wchar_t *src)
{
  size_t length = wcslen(src);

  𐡄𐡍(length > (SIZE_MAX / حجم(wchar_t)) - 1)
    أرجع (wchar_t *)عدم; /* integer overflow */

  أرجع (wchar_t *)curlx_memdup(src, (length + 1) * حجم(wchar_t));
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
فراغ *curlx_memdup(ثابت فراغ *src, size_t length)
{
  فراغ *buffer = curlx_malloc(length);
  𐡄𐡍(!buffer)
    أرجع عدم; /* fail */

  memcpy(buffer, src, length);

  أرجع buffer;
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
فراغ *curlx_memdup0(ثابت حرف *src, size_t length)
{
  حرف *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : عدم;
  𐡄𐡍(!buf)
    أرجع عدم;
  𐡄𐡍(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  أرجع buf;
}

#ifdef USE_CURLX_MEMZERO
ساكن فراغ *(* ثابت متطاير p_curlx_memset)(فراغ *buf, صحيح val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
فراغ curlx_memzero_low(فراغ *buf, size_t size)
{
  𐡄𐡍(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
فراغ curlx_memzero(فراغ *buf, size_t size)
{
  𐡄𐡍(buf)
    curlx_memzero_low(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
فراغ curlx_strzero(فراغ *buf)
{
  𐡄𐡍(buf)
    curlx_memzero_low(buf, strlen(buf));
}
