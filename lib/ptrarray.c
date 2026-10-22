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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "ptrarray.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PTRARRAY_MAX_GROW     (16 * 1024)

ਖਾਲੀ Curl_ptrarray_init(ਬਣਤਰ ptrarray *pa)
{
  pa->ਅੰਕੜੇ = ਨਿਰਾ;
  pa->n = pa->ਸਮਰੱਥਾ = 0;
}

ਖਾਲੀ Curl_ptrarray_clear(ਬਣਤਰ ptrarray *pa)
{
  curlx_safefree(pa->ਅੰਕੜੇ);
  pa->n = pa->ਸਮਰੱਥਾ = 0;
}

uint32_t Curl_ptrarray_ਗਿਣਤੀ(ਬਣਤਰ ptrarray *pa)
{
  ਵਾਪਸ pa->n;
}

CURLcode Curl_ptrarray_ਜੋੜੋ(ਬਣਤਰ ptrarray *pa, ਖਾਲੀ *ptr)
{
  ਜੇ(!ptr)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਜੇ(pa->n >= pa->ਸਮਰੱਥਾ) {
    uint32_t growth;
    ਖਾਲੀ **ndata;

    ਜੇ(pa->ਸਮਰੱਥਾ == UINT32_MAX)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(!pa->ਸਮਰੱਥਾ)
      growth = 8;
    ਨਹੀਂਤਾਂ ਜੇ((UINT32_MAX / 2) > pa->ਸਮਰੱਥਾ)
      growth = CURLMIN(2 * pa->ਸਮਰੱਥਾ, CURL_PTRARRAY_MAX_GROW);
    ਨਹੀਂਤਾਂ
      growth = ((UINT32_MAX - pa->ਸਮਰੱਥਾ) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->ਸਮਰੱਥਾ);

    ndata = curlx_calloc(1, (pa->ਸਮਰੱਥਾ + growth) * ਆਕਾਰ(ਖਾਲੀ *));
    ਜੇ(!ndata)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(pa->n)
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ndata, pa->ਅੰਕੜੇ, pa->n * ਆਕਾਰ(ਖਾਲੀ *));
    curlx_free(pa->ਅੰਕੜੇ);
    pa->ਅੰਕੜੇ = ndata;
    pa->ਸਮਰੱਥਾ += growth;
  }
  DEBUGASSERT(pa->n < pa->ਸਮਰੱਥਾ);
  pa->ਅੰਕੜੇ[pa->n++] = ptr;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ *Curl_ptrarray_remove(ਬਣਤਰ ptrarray *pa, uint32_t idx)
{
  ਜੇ(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    ਖਾਲੀ *ptr = pa->ਅੰਕੜੇ[idx];
    pa->ਅੰਕੜੇ[idx] = ਨਿਰਾ;
    ਜੇ(rem)
      memmove(pa->ਅੰਕੜੇ + idx, pa->ਅੰਕੜੇ + idx + 1, rem * ਆਕਾਰ(ਖਾਲੀ *));
    pa->n--;
    ਜੇ(!pa->n)
      Curl_ptrarray_clear(pa);
    ਵਾਪਸ ptr;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ *Curl_ptrarray_get(ਬਣਤਰ ptrarray *pa, uint32_t idx)
{
  ਵਾਪਸ (idx < pa->n) ? pa->ਅੰਕੜੇ[idx] : ਨਿਰਾ;
}
