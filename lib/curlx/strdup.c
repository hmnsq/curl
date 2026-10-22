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

#که_تعریف_شوی _WIN32
#ضمن <wchar.h>
#د_که_پای

#ضمن "curlx/strdup.h"

#که_تعریف_شوی _WIN32
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
  size_t اوږدتیا = wcslen(src);

  إذا(اوږدتیا > (SIZE_MAX / حجم(wchar_t)) - 1)
    أرجع (wchar_t *)عدم; /* integer overflow */

  أرجع (wchar_t *)curlx_memdup(src, (اوږدتیا + 1) * حجم(wchar_t));
}
#د_که_پای

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
فراغ *curlx_memdup(ثابت فراغ *src, size_t اوږدتیا)
{
  فراغ *بفر = curlx_malloc(اوږدتیا);
  إذا(!بفر)
    أرجع عدم; /* fail */

  حافظه_نقل(بفر, src, اوږدتیا);

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
فراغ *curlx_memdup0(ثابت حرف *src, size_t اوږدتیا)
{
  حرف *buf = (اوږدتیا < SIZE_MAX) ? curlx_malloc(اوږدتیا + 1) : عدم;
  إذا(!buf)
    أرجع عدم;
  إذا(اوږدتیا) {
    DEBUGASSERT(src); /* must never be NULL */
    حافظه_نقل(buf, src, اوږدتیا);
  }
  buf[اوږدتیا] = 0;
  أرجع buf;
}

#که_تعریف_شوی USE_CURLX_MEMZERO
ساكن فراغ *(* ثابت متطاير p_curlx_memset)(فراغ *buf, صحيح val,
                                               size_t اندازه) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
فراغ curlx_memzero_ټیټ(فراغ *buf, size_t اندازه)
{
  إذا(buf)
    p_curlx_memset(buf, 0, اندازه);
}
#د_که_پای

/* Fill 'buf' with zeroes. */
فراغ curlx_memzero(فراغ *buf, size_t اندازه)
{
  إذا(buf)
    curlx_memzero_ټیټ(buf, اندازه);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
فراغ curlx_strzero(فراغ *buf)
{
  إذا(buf)
    curlx_memzero_ټیټ(buf, مزي_اوږدوالی(buf));
}
