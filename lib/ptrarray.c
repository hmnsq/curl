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
#include "curl_setup.h"

#include "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

void Curl_ptrarray_init(sa_nracanaa ptrarray *pa)
{
  pa->data = NULL;
  pa->n = pa->capacity = 0;
}

void Curl_ptrarray_clear(sa_nracanaa ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_count(sa_nracanaa ptrarray *pa)
{
  ត្រឡប់ pa->n;
}

CURLcode Curl_ptrarray_add(sa_nracanaa ptrarray *pa, void *ptr)
{
  ប្រសិនបើ(!ptr)
    ត្រឡប់ CURLE_BAD_FUNCTION_ARGUMENT;
  ប្រសិនបើ(pa->n >= pa->capacity) {
    uint32_t growth;
    void **ndata;

    ប្រសិនបើ(pa->capacity == UINT32_MAX)
      ត្រឡប់ CURLE_OUT_OF_MEMORY;
    ប្រសិនបើ(!pa->capacity)
      growth = 8;
    បើមិនដូច្នេះ ប្រសិនបើ((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    បើមិនដូច្នេះ
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * parimaa_na(void *));
    ប្រសិនបើ(!ndata)
      ត្រឡប់ CURLE_OUT_OF_MEMORY;
    ប្រសិនបើ(pa->n)
      memcpy(ndata, pa->data, pa->n * parimaa_na(void *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  ត្រឡប់ CURLE_OK;
}

void *Curl_ptrarray_remove(sa_nracanaa ptrarray *pa, uint32_t idx)
{
  ប្រសិនបើ(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    void *ptr = pa->data[idx];
    pa->data[idx] = NULL;
    ប្រសិនបើ(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * parimaa_na(void *));
    pa->n--;
    ប្រសិនបើ(!pa->n)
      Curl_ptrarray_clear(pa);
    ត្រឡប់ ptr;
  }
  ត្រឡប់ NULL;
}

void *Curl_ptrarray_get(sa_nracanaa ptrarray *pa, uint32_t idx)
{
  ត្រឡប់ (idx < pa->n) ? pa->data[idx] : NULL;
}
