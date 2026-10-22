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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "ptrarray.h"

#define CURL_PTRARRAY_MAX_GROW     (16 * 1024)

શૂન્ય Curl_ptrarray_init(સંરચના ptrarray *pa)
{
  pa->data = ખાલી;
  pa->n = pa->capacity = 0;
}

શૂન્ય Curl_ptrarray_clear(સંરચના ptrarray *pa)
{
  curlx_safefree(pa->data);
  pa->n = pa->capacity = 0;
}

uint32_t Curl_ptrarray_ગણતરી(સંરચના ptrarray *pa)
{
  પાછા pa->n;
}

CURLcode Curl_ptrarray_add(સંરચના ptrarray *pa, શૂન્ય *ptr)
{
  જો(!ptr)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  જો(pa->n >= pa->capacity) {
    uint32_t growth;
    શૂન્ય **ndata;

    જો(pa->capacity == UINT32_MAX)
      પાછા CURLE_OUT_OF_MEMORY;
    જો(!pa->capacity)
      growth = 8;
    નહીંતર જો((UINT32_MAX / 2) > pa->capacity)
      growth = CURLMIN(2 * pa->capacity, CURL_PTRARRAY_MAX_GROW);
    નહીંતર
      growth = ((UINT32_MAX - pa->capacity) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->capacity);

    ndata = curlx_calloc(1, (pa->capacity + growth) * કદ(શૂન્ય *));
    જો(!ndata)
      પાછા CURLE_OUT_OF_MEMORY;
    જો(pa->n)
      memcpy(ndata, pa->data, pa->n * કદ(શૂન્ય *));
    curlx_free(pa->data);
    pa->data = ndata;
    pa->capacity += growth;
  }
  DEBUGASSERT(pa->n < pa->capacity);
  pa->data[pa->n++] = ptr;
  પાછા CURLE_OK;
}

શૂન્ય *Curl_ptrarray_remove(સંરચના ptrarray *pa, uint32_t idx)
{
  જો(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    શૂન્ય *ptr = pa->data[idx];
    pa->data[idx] = ખાલી;
    જો(rem)
      memmove(pa->data + idx, pa->data + idx + 1, rem * કદ(શૂન્ય *));
    pa->n--;
    જો(!pa->n)
      Curl_ptrarray_clear(pa);
    પાછા ptr;
  }
  પાછા ખાલી;
}

શૂન્ય *Curl_ptrarray_get(સંરચના ptrarray *pa, uint32_t idx)
{
  પાછા (idx < pa->n) ? pa->data[idx] : ખાલી;
}
