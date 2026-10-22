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

#اگر_تعریف_شدہ _WIN32
#شامل <wchar.h>
#اگر_ختم

#شامل "curlx/strdup.h"

#اگر_تعریف_شدہ _WIN32
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
  size_t لمبائی = wcslen(src);

  اگر(لمبائی > (SIZE_MAX / حجم(wchar_t)) - 1)
    واپس (wchar_t *)تہی; /* integer overflow */

  واپس (wchar_t *)curlx_memdup(src, (لمبائی + 1) * حجم(wchar_t));
}
#اگر_ختم

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
خالی *curlx_memdup(ثابت خالی *src, size_t لمبائی)
{
  خالی *بفر = curlx_malloc(لمبائی);
  اگر(!بفر)
    واپس تہی; /* fail */

  حافظہ_نقل(بفر, src, لمبائی);

  واپس بفر;
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
خالی *curlx_memdup0(ثابت حرف *src, size_t لمبائی)
{
  حرف *buf = (لمبائی < SIZE_MAX) ? curlx_malloc(لمبائی + 1) : تہی;
  اگر(!buf)
    واپس تہی;
  اگر(لمبائی) {
    DEBUGASSERT(src); /* must never be NULL */
    حافظہ_نقل(buf, src, لمبائی);
  }
  buf[لمبائی] = 0;
  واپس buf;
}

#اگر_تعریف_شدہ USE_CURLX_MEMZERO
ساکن خالی *(* ثابت متلون p_curlx_memset)(خالی *buf, صحیح val,
                                               size_t سائز) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
خالی curlx_memzero_کم(خالی *buf, size_t سائز)
{
  اگر(buf)
    p_curlx_memset(buf, 0, سائز);
}
#اگر_ختم

/* Fill 'buf' with zeroes. */
خالی curlx_memzero(خالی *buf, size_t سائز)
{
  اگر(buf)
    curlx_memzero_کم(buf, سائز);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
خالی curlx_strzero(خالی *buf)
{
  اگر(buf)
    curlx_memzero_کم(buf, لڑی_لمبائی(buf));
}
