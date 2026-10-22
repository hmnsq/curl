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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

ଶୂନ୍ୟ Curl_ptrarray_init(ସଂରଚନା ptrarray *pa)
{
  pa->data = ଖାଲି;
  pa->n = pa->capacity = 0;
}

ଶୂନ୍ୟ Curl_ptrarray_clear(ସଂରଚନା ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_ଗଣନା(ସଂରଚନା ptrarray *pa)
{
  ଫେରନ୍ତୁ pa->n;
}

CURLcode Curl_ptrarray_add(ସଂରଚନା ptrarray *pa, ଶୂନ୍ୟ *ptr)
{
  ଯଦି(!ptr)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଯଦି(pa->n >= pa->capacity) {
    uint32_t growth;
    ଶୂନ୍ୟ **ndata;

    ଯଦି(pa->capacity == UINT32_MAX)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(!pa->capacity)
      growth = 8;
    ଅନ୍ୟଥା ଯଦି((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    ଅନ୍ୟଥା
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * ଆକାର(ଶୂନ୍ୟ *));
    ଯଦି(!ndata)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(pa->n)
      memcpy(ndata, pa->data, pa->n * ଆକାର(ଶୂନ୍ୟ *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ *Curl_ptrarray_remove(ସଂରଚନା ptrarray *pa, uint32_t idx)
{
  ଯଦି(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    ଶୂନ୍ୟ *ptr = pa->data[idx];
    pa->data[idx] = ଖାଲି;
    ଯଦି(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * ଆକାର(ଶୂନ୍ୟ *));
    pa->n--;
    ଯଦି(!pa->n)
      Curl_ptrarray_clear(pa);
    ଫେରନ୍ତୁ ptr;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ *Curl_ptrarray_get(ସଂରଚନା ptrarray *pa, uint32_t idx)
{
  ଫେରନ୍ତୁ (idx < pa->n) ? pa->data[idx] : ଖାଲି;
}
