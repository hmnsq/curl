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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

ശൂന്യം Curl_ptrarray_init(ഘടന ptrarray *pa)
{
  pa->data = ശൂന്യ;
  pa->n = pa->capacity = 0;
}

ശൂന്യം Curl_ptrarray_clear(ഘടന ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_എണ്ണം(ഘടന ptrarray *pa)
{
  തിരികെ pa->n;
}

CURLcode Curl_ptrarray_add(ഘടന ptrarray *pa, ശൂന്യം *ptr)
{
  എങ്കിൽ(!ptr)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  എങ്കിൽ(pa->n >= pa->capacity) {
    uint32_t growth;
    ശൂന്യം **ndata;

    എങ്കിൽ(pa->capacity == UINT32_MAX)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(!pa->capacity)
      growth = 8;
    അല്ലെങ്കിൽ എങ്കിൽ((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    അല്ലെങ്കിൽ
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * വലുപ്പം(ശൂന്യം *));
    എങ്കിൽ(!ndata)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(pa->n)
      memcpy(ndata, pa->data, pa->n * വലുപ്പം(ശൂന്യം *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  തിരികെ CURLE_OK;
}

ശൂന്യം *Curl_ptrarray_remove(ഘടന ptrarray *pa, uint32_t idx)
{
  എങ്കിൽ(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    ശൂന്യം *ptr = pa->data[idx];
    pa->data[idx] = ശൂന്യ;
    എങ്കിൽ(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * വലുപ്പം(ശൂന്യം *));
    pa->n--;
    എങ്കിൽ(!pa->n)
      Curl_ptrarray_clear(pa);
    തിരികെ ptr;
  }
  തിരികെ ശൂന്യ;
}

ശൂന്യം *Curl_ptrarray_get(ഘടന ptrarray *pa, uint32_t idx)
{
  തിരികെ (idx < pa->n) ? pa->data[idx] : ശൂന്യ;
}
