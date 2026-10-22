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
  вернуть pa->n;
}

CURLcode Curl_ptrarray_add(struct ptrarray *pa, void *ptr)
{
  если(!ptr)
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  если(pa->n >= pa->capacity) {
    uint32_t growth;
    void **ndata;

    если(pa->capacity == UINT32_MAX)
      вернуть CURLE_OUT_OF_MEMORY;
    если(!pa->capacity)
      growth = 8;
    иначе если((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    иначе
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * sizeof(void *));
    если(!ndata)
      вернуть CURLE_OUT_OF_MEMORY;
    если(pa->n)
      memcpy(ndata, pa->data, pa->n * sizeof(void *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  вернуть CURLE_OK;
}

void *Curl_ptrarray_remove(struct ptrarray *pa, uint32_t idx)
{
  если(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    void *ptr = pa->data[idx];
    pa->data[idx] = NULL;
    если(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * sizeof(void *));
    pa->n--;
    если(!pa->n)
      Curl_ptrarray_clear(pa);
    вернуть ptr;
  }
  вернуть NULL;
}

void *Curl_ptrarray_get(struct ptrarray *pa, uint32_t idx)
{
  вернуть (idx < pa->n) ? pa->data[idx] : NULL;
}
