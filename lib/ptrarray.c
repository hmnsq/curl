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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

शून्य Curl_ptrarray_init(संरचना ptrarray *pa)
{
  pa->data = रिक्त;
  pa->n = pa->capacity = 0;
}

शून्य Curl_ptrarray_clear(संरचना ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_गणनम्(संरचना ptrarray *pa)
{
  पटिनिवत्त pa->n;
}

CURLcode Curl_ptrarray_add(संरचना ptrarray *pa, शून्य *ptr)
{
  सचे(!ptr)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  सचे(pa->n >= pa->capacity) {
    uint32_t growth;
    शून्य **ndata;

    सचे(pa->capacity == UINT32_MAX)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    सचे(!pa->capacity)
      growth = 8;
    अञ्ञथा सचे((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    अञ्ञथा
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * परिमाण(शून्य *));
    सचे(!ndata)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    सचे(pa->n)
      memcpy(ndata, pa->data, pa->n * परिमाण(शून्य *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  पटिनिवत्त CURLE_OK;
}

शून्य *Curl_ptrarray_remove(संरचना ptrarray *pa, uint32_t idx)
{
  सचे(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    शून्य *ptr = pa->data[idx];
    pa->data[idx] = रिक्त;
    सचे(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * परिमाण(शून्य *));
    pa->n--;
    सचे(!pa->n)
      Curl_ptrarray_clear(pa);
    पटिनिवत्त ptr;
  }
  पटिनिवत्त रिक्त;
}

शून्य *Curl_ptrarray_get(संरचना ptrarray *pa, uint32_t idx)
{
  पटिनिवत्त (idx < pa->n) ? pa->data[idx] : रिक्त;
}
