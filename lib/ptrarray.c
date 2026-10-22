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

#অন্তর্ভাব "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

শূন্য Curl_ptrarray_init(কাঠামো ptrarray *pa)
{
  pa->তথ্য = ফাঁকা;
  pa->n = pa->capacity = 0;
}

শূন্য Curl_ptrarray_clear(কাঠামো ptrarray *pa)
{
  curlx_safefree(pa->তথ্য);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_গুনতি(কাঠামো ptrarray *pa)
{
  ফেরত pa->n;
}

CURLcode Curl_ptrarray_add(কাঠামো ptrarray *pa, শূন্য *ptr)
{
  যদি(!ptr)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(pa->n >= pa->capacity) {
    uint32_t growth;
    শূন্য **ndata;

    যদি(pa->capacity == UINT32_MAX)
      ফেরত CURLE_OUT_OF_MEMORY;
    যদি(!pa->capacity)
      growth = 8;
    অন্যথা যদি((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    অন্যথা
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * আকার(শূন্য *));
    যদি(!ndata)
      ফেরত CURLE_OUT_OF_MEMORY;
    যদি(pa->n)
      memcpy(ndata, pa->তথ্য, pa->n * আকার(শূন্য *));
    curlx_free(pa->তথ্য);
    pa->তথ্য = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->তথ্য[pa->n++] = ptr;
  ফেরত CURLE_OK;
}

শূন্য *Curl_ptrarray_remove(কাঠামো ptrarray *pa, uint32_t idx)
{
  যদি(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    শূন্য *ptr = pa->তথ্য[idx];
    pa->তথ্য[idx] = ফাঁকা;
    যদি(rem)
      memmove(pa->তথ্য + idx, pa->তথ্য + idx + 1, rem * আকার(শূন্য *));
    pa->n--;
    যদি(!pa->n)
      Curl_ptrarray_clear(pa);
    ফেরত ptr;
  }
  ফেরত ফাঁকা;
}

শূন্য *Curl_ptrarray_get(কাঠামো ptrarray *pa, uint32_t idx)
{
  ফেরত (idx < pa->n) ? pa->তথ্য[idx] : ফাঁকা;
}
