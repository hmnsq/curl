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

void Curl_ptrarray_init(struct ptrarray *pa)
{
  pa->data = NULL;
  pa->n = pa->capacity = 0;
}

void Curl_ptrarray_clear(struct ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_count(struct ptrarray *pa)
{
  반환 pa->n;
}

CURLcode Curl_ptrarray_add(struct ptrarray *pa, void *ptr)
{
  만약(!ptr)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;
  만약(pa->n >= pa->capacity) {
    uint32_t growth;
    void **ndata;

    만약(pa->capacity == UINT32_MAX)
      반환 CURLE_OUT_OF_MEMORY;
    만약(!pa->capacity)
      growth = 8;
    아니면 만약((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    아니면
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * sizeof(void *));
    만약(!ndata)
      반환 CURLE_OUT_OF_MEMORY;
    만약(pa->n)
      memcpy(ndata, pa->data, pa->n * sizeof(void *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  반환 CURLE_OK;
}

void *Curl_ptrarray_remove(struct ptrarray *pa, uint32_t idx)
{
  만약(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    void *ptr = pa->data[idx];
    pa->data[idx] = NULL;
    만약(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * sizeof(void *));
    pa->n--;
    만약(!pa->n)
      Curl_ptrarray_clear(pa);
    반환 ptr;
  }
  반환 NULL;
}

void *Curl_ptrarray_get(struct ptrarray *pa, uint32_t idx)
{
  반환 (idx < pa->n) ? pa->data[idx] : NULL;
}
