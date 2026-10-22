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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef _WIN32
#অন্তর্ভাৱ <wchar.h>
#endif

#অন্তর্ভাৱ "curlx/strdup.h"

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
wchar_t *curlx_wcsdup(ধ্রুৱ wchar_t *src)
{
  size_t length = wcslen(src);

  যদি(length > (SIZE_MAX / পরিমাণ(wchar_t)) - 1)
    ঘূৰাই (wchar_t *)রিক্ত; /* integer overflow */

  ঘূৰাই (wchar_t *)curlx_memdup(src, (length + 1) * পরিমাণ(wchar_t));
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
শূন্য *curlx_memdup(ধ্রুৱ শূন্য *src, size_t length)
{
  শূন্য *buffer = curlx_malloc(length);
  যদি(!buffer)
    ঘূৰাই রিক্ত; /* fail */

  memcpy(buffer, src, length);

  ঘূৰাই buffer;
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
শূন্য *curlx_memdup0(ধ্রুৱ ৱর্ণ *src, size_t length)
{
  ৱর্ণ *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : রিক্ত;
  যদি(!buf)
    ঘূৰাই রিক্ত;
  যদি(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  ঘূৰাই buf;
}

#ifdef USE_CURLX_MEMZERO
স্থির শূন্য *(* ধ্রুৱ চঞ্চল p_curlx_memset)(শূন্য *buf, পূর্ণাঙ্ক val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
শূন্য curlx_memzero_নিচেৰ(শূন্য *buf, size_t size)
{
  যদি(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
শূন্য curlx_memzero(শূন্য *buf, size_t size)
{
  যদি(buf)
    curlx_memzero_নিচেৰ(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
শূন্য curlx_strzero(শূন্য *buf)
{
  যদি(buf)
    curlx_memzero_নিচেৰ(buf, strlen(buf));
}
