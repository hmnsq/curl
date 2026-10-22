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
#অন্তর্ভাব "curl_setup.h"

#ifdef _WIN32
#অন্তর্ভাব <wchar.h>
#endif

#অন্তর্ভাব "curlx/strdup.h"

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
wchar_t *curlx_wcsdup(ধ্রুব wchar_t *src)
{
  size_t length = wcslen(src);

  যদি(length > (SIZE_MAX / পরিমাণ(wchar_t)) - 1)
    প্রত্যাগম (wchar_t *)রিক্ত; /* integer overflow */

  প্রত্যাগম (wchar_t *)curlx_memdup(src, (length + 1) * পরিমাণ(wchar_t));
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
শূন্য *curlx_memdup(ধ্রুব শূন্য *src, size_t length)
{
  শূন্য *buffer = curlx_malloc(length);
  যদি(!buffer)
    প্রত্যাগম রিক্ত; /* fail */

  memcpy(buffer, src, length);

  প্রত্যাগম buffer;
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
শূন্য *curlx_memdup0(ধ্রুব বর্ণ *src, size_t length)
{
  বর্ণ *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : রিক্ত;
  যদি(!buf)
    প্রত্যাগম রিক্ত;
  যদি(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  প্রত্যাগম buf;
}

#ifdef USE_CURLX_MEMZERO
স্থির শূন্য *(* ধ্রুব চঞ্চল p_curlx_memset)(শূন্য *buf, পূর্ণাঙ্ক val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
শূন্য curlx_memzero_নিচের(শূন্য *buf, size_t size)
{
  যদি(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
শূন্য curlx_memzero(শূন্য *buf, size_t size)
{
  যদি(buf)
    curlx_memzero_নিচের(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
শূন্য curlx_strzero(শূন্য *buf)
{
  যদি(buf)
    curlx_memzero_নিচের(buf, strlen(buf));
}
