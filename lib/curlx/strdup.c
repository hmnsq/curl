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
wchar_t *curlx_wcsdup(ধ্রুবক wchar_t *src)
{
  size_t দৈর্ঘ্য = wcslen(src);

  যদি(দৈর্ঘ্য > (SIZE_MAX / আকার(wchar_t)) - 1)
    ফেরত (wchar_t *)ফাঁকা; /* integer overflow */

  ফেরত (wchar_t *)curlx_memdup(src, (দৈর্ঘ্য + 1) * আকার(wchar_t));
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
শূন্য *curlx_memdup(ধ্রুবক শূন্য *src, size_t দৈর্ঘ্য)
{
  শূন্য *বাফার = curlx_malloc(দৈর্ঘ্য);
  যদি(!বাফার)
    ফেরত ফাঁকা; /* fail */

  memcpy(বাফার, src, দৈর্ঘ্য);

  ফেরত বাফার;
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
শূন্য *curlx_memdup0(ধ্রুবক অক্ষর *src, size_t দৈর্ঘ্য)
{
  অক্ষর *buf = (দৈর্ঘ্য < SIZE_MAX) ? curlx_malloc(দৈর্ঘ্য + 1) : ফাঁকা;
  যদি(!buf)
    ফেরত ফাঁকা;
  যদি(দৈর্ঘ্য) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, দৈর্ঘ্য);
  }
  buf[দৈর্ঘ্য] = 0;
  ফেরত buf;
}

#ifdef USE_CURLX_MEMZERO
স্থির শূন্য *(* ধ্রুবক অস্থির p_curlx_memset)(শূন্য *buf, পূর্ণসংখ্যা val,
                                               size_t আকার_2) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
শূন্য curlx_memzero_নিচের(শূন্য *buf, size_t আকার_2)
{
  যদি(buf)
    p_curlx_memset(buf, 0, আকার_2);
}
#endif

/* Fill 'buf' with zeroes. */
শূন্য curlx_memzero(শূন্য *buf, size_t আকার_2)
{
  যদি(buf)
    curlx_memzero_নিচের(buf, আকার_2);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
শূন্য curlx_strzero(শূন্য *buf)
{
  যদি(buf)
    curlx_memzero_নিচের(buf, strlen(buf));
}
