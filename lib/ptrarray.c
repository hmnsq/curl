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

#شامل "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

تهی Curl_ptrarray_init(ساختار ptrarray *pa)
{
  pa->data = پوچ;
  pa->n = pa->capacity = 0;
}

تهی Curl_ptrarray_clear(ساختار ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_شمار(ساختار ptrarray *pa)
{
  بازگشت pa->n;
}

CURLcode Curl_ptrarray_add(ساختار ptrarray *pa, تهی *ptr)
{
  اگر(!ptr)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  اگر(pa->n >= pa->capacity) {
    uint32_t growth;
    تهی **ndata;

    اگر(pa->capacity == UINT32_MAX)
      بازگشت CURLE_OUT_OF_MEMORY;
    اگر(!pa->capacity)
      growth = 8;
    وگرنه اگر((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    وگرنه
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * اندازه(تهی *));
    اگر(!ndata)
      بازگشت CURLE_OUT_OF_MEMORY;
    اگر(pa->n)
      memcpy(ndata, pa->data, pa->n * اندازه(تهی *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  بازگشت CURLE_OK;
}

تهی *Curl_ptrarray_remove(ساختار ptrarray *pa, uint32_t idx)
{
  اگر(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    تهی *ptr = pa->data[idx];
    pa->data[idx] = پوچ;
    اگر(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * اندازه(تهی *));
    pa->n--;
    اگر(!pa->n)
      Curl_ptrarray_clear(pa);
    بازگشت ptr;
  }
  بازگشت پوچ;
}

تهی *Curl_ptrarray_get(ساختار ptrarray *pa, uint32_t idx)
{
  بازگشت (idx < pa->n) ? pa->data[idx] : پوچ;
}
