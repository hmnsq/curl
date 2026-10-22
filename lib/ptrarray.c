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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

ಶೂನ್ಯ Curl_ptrarray_init(ಸಂರಚನಾ ptrarray *pa)
{
  pa->data = ರಿಕ್ತ;
  pa->n = pa->capacity = 0;
}

ಶೂನ್ಯ Curl_ptrarray_clear(ಸಂರಚನಾ ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_ಎಣಿಕೆ(ಸಂರಚನಾ ptrarray *pa)
{
  ಪ್ರತ್ಯಾಗಮ pa->n;
}

CURLcode Curl_ptrarray_add(ಸಂರಚನಾ ptrarray *pa, ಶೂನ್ಯ *ptr)
{
  ಯದಿ(!ptr)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಯದಿ(pa->n >= pa->capacity) {
    uint32_t growth;
    ಶೂನ್ಯ **ndata;

    ಯದಿ(pa->capacity == UINT32_MAX)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(!pa->capacity)
      growth = 8;
    ಅನ್ಯಥಾ ಯದಿ((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    ಅನ್ಯಥಾ
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * ಪರಿಮಾಣ(ಶೂನ್ಯ *));
    ಯದಿ(!ndata)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(pa->n)
      memcpy(ndata, pa->data, pa->n * ಪರಿಮಾಣ(ಶೂನ್ಯ *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ *Curl_ptrarray_remove(ಸಂರಚನಾ ptrarray *pa, uint32_t idx)
{
  ಯದಿ(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    ಶೂನ್ಯ *ptr = pa->data[idx];
    pa->data[idx] = ರಿಕ್ತ;
    ಯದಿ(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * ಪರಿಮಾಣ(ಶೂನ್ಯ *));
    pa->n--;
    ಯದಿ(!pa->n)
      Curl_ptrarray_clear(pa);
    ಪ್ರತ್ಯಾಗಮ ptr;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ *Curl_ptrarray_get(ಸಂರಚನಾ ptrarray *pa, uint32_t idx)
{
  ಪ್ರತ್ಯಾಗಮ (idx < pa->n) ? pa->data[idx] : ರಿಕ್ತ;
}
