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

#اگر_تعریف_شده _WIN32
#ضمن <wchar.h>
#پایان_اگر

#ضمن "curlx/strdup.h"

#اگر_تعریف_شده _WIN32
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
  size_t درازی = wcslen(src);

  إذا(درازی > (SIZE_MAX / حجم(wchar_t)) - 1)
    أرجع (wchar_t *)عدم; /* integer overflow */

  أرجع (wchar_t *)curlx_memdup(src, (درازی + 1) * حجم(wchar_t));
}
#پایان_اگر

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
فراغ *curlx_memdup(ثابت فراغ *src, size_t درازی)
{
  فراغ *میانگیر = curlx_malloc(درازی);
  إذا(!میانگیر)
    أرجع عدم; /* fail */

  کپی_حافظه(میانگیر, src, درازی);

  أرجع میانگیر;
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
فراغ *curlx_memdup0(ثابت حرف *src, size_t درازی)
{
  حرف *buf = (درازی < SIZE_MAX) ? curlx_malloc(درازی + 1) : عدم;
  إذا(!buf)
    أرجع عدم;
  إذا(درازی) {
    DEBUGASSERT(src); /* must never be NULL */
    کپی_حافظه(buf, src, درازی);
  }
  buf[درازی] = 0;
  أرجع buf;
}

#اگر_تعریف_شده USE_CURLX_MEMZERO
ساكن فراغ *(* ثابت متطاير p_curlx_memset)(فراغ *buf, صحيح val,
                                               size_t اندازه) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
فراغ curlx_memzero_کم(فراغ *buf, size_t اندازه)
{
  إذا(buf)
    p_curlx_memset(buf, 0, اندازه);
}
#پایان_اگر

/* Fill 'buf' with zeroes. */
فراغ curlx_memzero(فراغ *buf, size_t اندازه)
{
  إذا(buf)
    curlx_memzero_کم(buf, اندازه);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
فراغ curlx_strzero(فراغ *buf)
{
  إذا(buf)
    curlx_memzero_کم(buf, طول_رشته(buf));
}
