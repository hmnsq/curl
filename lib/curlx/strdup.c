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

#جے_تعریف_شدہ _WIN32
#ضمن <wchar.h>
#جے_انت

#ضمن "curlx/strdup.h"

#جے_تعریف_شدہ _WIN32
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
  size_t لمائی = wcslen(src);

  إذا(لمائی > (SIZE_MAX / حجم(wchar_t)) - 1)
    أرجع (wchar_t *)عدم; /* integer overflow */

  أرجع (wchar_t *)curlx_memdup(src, (لمائی + 1) * حجم(wchar_t));
}
#جے_انت

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
فراغ *curlx_memdup(ثابت فراغ *src, size_t لمائی)
{
  فراغ *بفر = curlx_malloc(لمائی);
  إذا(!بفر)
    أرجع عدم; /* fail */

  حافظہ_نقل(بفر, src, لمائی);

  أرجع بفر;
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
فراغ *curlx_memdup0(ثابت حرف *src, size_t لمائی)
{
  حرف *buf = (لمائی < SIZE_MAX) ? curlx_malloc(لمائی + 1) : عدم;
  إذا(!buf)
    أرجع عدم;
  إذا(لمائی) {
    DEBUGASSERT(src); /* must never be NULL */
    حافظہ_نقل(buf, src, لمائی);
  }
  buf[لمائی] = 0;
  أرجع buf;
}

#جے_تعریف_شدہ USE_CURLX_MEMZERO
ساكن فراغ *(* ثابت متطاير p_curlx_memset)(فراغ *buf, صحيح val,
                                               size_t ناپ) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
فراغ curlx_memzero_کم(فراغ *buf, size_t ناپ)
{
  إذا(buf)
    p_curlx_memset(buf, 0, ناپ);
}
#جے_انت

/* Fill 'buf' with zeroes. */
فراغ curlx_memzero(فراغ *buf, size_t ناپ)
{
  إذا(buf)
    curlx_memzero_کم(buf, ناپ);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
فراغ curlx_strzero(فراغ *buf)
{
  إذا(buf)
    curlx_memzero_کم(buf, لڑی_لمبائی(buf));
}
