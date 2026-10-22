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
#شامل "curl_setup.h"

#ifdef _WIN32
#شامل <wchar.h>
#endif

#شامل "curlx/strdup.h"

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

  اگر(length > (SIZE_MAX / اندازه(wchar_t)) - 1)
    بازگشت (wchar_t *)پوچ; /* integer overflow */

  بازگشت (wchar_t *)curlx_memdup(src, (length + 1) * اندازه(wchar_t));
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
تهی *curlx_memdup(ثابت تهی *src, size_t length)
{
  تهی *buffer = curlx_malloc(length);
  اگر(!buffer)
    بازگشت پوچ; /* fail */

  memcpy(buffer, src, length);

  بازگشت buffer;
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
تهی *curlx_memdup0(ثابت نویسه *src, size_t length)
{
  نویسه *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : پوچ;
  اگر(!buf)
    بازگشت پوچ;
  اگر(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  بازگشت buf;
}

#ifdef USE_CURLX_MEMZERO
ایستا تهی *(* ثابت فرار p_curlx_memset)(تهی *buf, صحیح val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
تهی curlx_memzero_کم(تهی *buf, size_t size)
{
  اگر(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
تهی curlx_memzero(تهی *buf, size_t size)
{
  اگر(buf)
    curlx_memzero_کم(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
تهی curlx_strzero(تهی *buf)
{
  اگر(buf)
    curlx_memzero_کم(buf, strlen(buf));
}
