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

#ضمن "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

فراغ Curl_ptrarray_init(بنية ptrarray *pa)
{
  pa->data = عدم;
  pa->n = pa->capacity = 0;
}

فراغ Curl_ptrarray_clear(بنية ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_کيراءن(بنية ptrarray *pa)
{
  أرجع pa->n;
}

CURLcode Curl_ptrarray_add(بنية ptrarray *pa, فراغ *ptr)
{
  إذا(!ptr)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  إذا(pa->n >= pa->capacity) {
    uint32_t growth;
    فراغ **ndata;

    إذا(pa->capacity == UINT32_MAX)
      أرجع CURLE_OUT_OF_MEMORY;
    إذا(!pa->capacity)
      growth = 8;
    وإلا إذا((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    وإلا
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * حجم(فراغ *));
    إذا(!ndata)
      أرجع CURLE_OUT_OF_MEMORY;
    إذا(pa->n)
      memcpy(ndata, pa->data, pa->n * حجم(فراغ *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  أرجع CURLE_OK;
}

فراغ *Curl_ptrarray_remove(بنية ptrarray *pa, uint32_t idx)
{
  إذا(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    فراغ *ptr = pa->data[idx];
    pa->data[idx] = عدم;
    إذا(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * حجم(فراغ *));
    pa->n--;
    إذا(!pa->n)
      Curl_ptrarray_clear(pa);
    أرجع ptr;
  }
  أرجع عدم;
}

فراغ *Curl_ptrarray_get(بنية ptrarray *pa, uint32_t idx)
{
  أرجع (idx < pa->n) ? pa->data[idx] : عدم;
}
