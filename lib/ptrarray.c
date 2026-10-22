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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

வெறுமை Curl_ptrarray_init(கட்டமைப்பு ptrarray *pa)
{
  pa->data = வெற்று;
  pa->n = pa->capacity = 0;
}

வெறுமை Curl_ptrarray_clear(கட்டமைப்பு ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_மொத்தம்(கட்டமைப்பு ptrarray *pa)
{
  திரும்பு pa->n;
}

CURLcode Curl_ptrarray_add(கட்டமைப்பு ptrarray *pa, வெறுமை *ptr)
{
  எனில்(!ptr)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  எனில்(pa->n >= pa->capacity) {
    uint32_t growth;
    வெறுமை **ndata;

    எனில்(pa->capacity == UINT32_MAX)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(!pa->capacity)
      growth = 8;
    இல்லையெனில் எனில்((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    இல்லையெனில்
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * அளவு(வெறுமை *));
    எனில்(!ndata)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(pa->n)
      memcpy(ndata, pa->data, pa->n * அளவு(வெறுமை *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  திரும்பு CURLE_OK;
}

வெறுமை *Curl_ptrarray_remove(கட்டமைப்பு ptrarray *pa, uint32_t idx)
{
  எனில்(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    வெறுமை *ptr = pa->data[idx];
    pa->data[idx] = வெற்று;
    எனில்(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * அளவு(வெறுமை *));
    pa->n--;
    எனில்(!pa->n)
      Curl_ptrarray_clear(pa);
    திரும்பு ptr;
  }
  திரும்பு வெற்று;
}

வெறுமை *Curl_ptrarray_get(கட்டமைப்பு ptrarray *pa, uint32_t idx)
{
  திரும்பு (idx < pa->n) ? pa->data[idx] : வெற்று;
}
