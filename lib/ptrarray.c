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

#অন্তর্ভাৱ "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

শূন্য Curl_ptrarray_init(সংরচনা ptrarray *pa)
{
  pa->data = রিক্ত;
  pa->n = pa->capacity = 0;
}

শূন্য Curl_ptrarray_clear(সংরচনা ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_গুনতি(সংরচনা ptrarray *pa)
{
  ঘূৰাই pa->n;
}

CURLcode Curl_ptrarray_add(সংরচনা ptrarray *pa, শূন্য *ptr)
{
  যদি(!ptr)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(pa->n >= pa->capacity) {
    uint32_t growth;
    শূন্য **ndata;

    যদি(pa->capacity == UINT32_MAX)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    যদি(!pa->capacity)
      growth = 8;
    অন্যথা যদি((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    অন্যথা
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * পরিমাণ(শূন্য *));
    যদি(!ndata)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    যদি(pa->n)
      memcpy(ndata, pa->data, pa->n * পরিমাণ(শূন্য *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  ঘূৰাই CURLE_OK;
}

শূন্য *Curl_ptrarray_remove(সংরচনা ptrarray *pa, uint32_t idx)
{
  যদি(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    শূন্য *ptr = pa->data[idx];
    pa->data[idx] = রিক্ত;
    যদি(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * পরিমাণ(শূন্য *));
    pa->n--;
    যদি(!pa->n)
      Curl_ptrarray_clear(pa);
    ঘূৰাই ptr;
  }
  ঘূৰাই রিক্ত;
}

শূন্য *Curl_ptrarray_get(সংরচনা ptrarray *pa, uint32_t idx)
{
  ঘূৰাই (idx < pa->n) ? pa->data[idx] : রিক্ত;
}
