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
#समावेश "curl_setup.h"

#समावेश "ptrarray.h"

#घोषणा CURL_PTRARRAY_MAX_GROW     (16 * 1024)

व्योम Curl_ptrarray_init(काष्ठा ptrarray *pa)
{
  pa->तथ्य = शून्य;
  pa->n = pa->capacity = 0;
}

व्योम Curl_ptrarray_clear(काष्ठा ptrarray *pa)
{
  curlx_safefree(pa->तथ्य);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_गिनती(काष्ठा ptrarray *pa)
{
  वापस pa->n;
}

CURLcode Curl_ptrarray_add(काष्ठा ptrarray *pa, व्योम *ptr)
{
  अगर(!ptr)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  अगर(pa->n >= pa->capacity) {
    uint32_t growth;
    व्योम **ndata;

    अगर(pa->capacity == UINT32_MAX)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(!pa->capacity)
      growth = 8;
    अन्यथा अगर((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    अन्यथा
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * माप(व्योम *));
    अगर(!ndata)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(pa->n)
      स_नकल(ndata, pa->तथ्य, pa->n * माप(व्योम *));
    curlx_free(pa->तथ्य);
    pa->तथ्य = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->तथ्य[pa->n++] = ptr;
  वापस CURLE_OK;
}

व्योम *Curl_ptrarray_remove(काष्ठा ptrarray *pa, uint32_t idx)
{
  अगर(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    व्योम *ptr = pa->तथ्य[idx];
    pa->तथ्य[idx] = शून्य;
    अगर(rem)
      स_हटाओ(pa->तथ्य + idx, pa->तथ्य + idx + 1, rem * माप(व्योम *));
    pa->n--;
    अगर(!pa->n)
      Curl_ptrarray_clear(pa);
    वापस ptr;
  }
  वापस शून्य;
}

व्योम *Curl_ptrarray_get(काष्ठा ptrarray *pa, uint32_t idx)
{
  वापस (idx < pa->n) ? pa->तथ्य[idx] : शून्य;
}
