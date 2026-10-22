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
#شامل "curl_setup.h"

#شامل "ptrarray.h"

#تعریف_کرو CURL_PTRARRAY_MAX_GROW     (16 * 1024)

خالی Curl_ptrarray_init(ساخت ptrarray *pa)
{
  pa->ڈیٹا = تہی;
  pa->n = pa->استعداد = 0;
}

خالی Curl_ptrarray_clear(ساخت ptrarray *pa)
{
  curlx_safefree(pa->ڈیٹا);
  pa->n = pa->استعداد = 0;
}

uint32_t Curl_ptrarray_گنتی(ساخت ptrarray *pa)
{
  واپس pa->n;
}

CURLcode Curl_ptrarray_جوڑو(ساخت ptrarray *pa, خالی *ptr)
{
  اگر(!ptr)
    واپس CURLE_BAD_FUNCTION_دلیل;
  اگر(pa->n >= pa->استعداد) {
    uint32_t growth;
    خالی **ndata;

    اگر(pa->استعداد == UINT32_MAX)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(!pa->استعداد)
      growth = 8;
    ورنہ اگر((UINT32_MAX / 2) > pa->استعداد)
      growth = CURLMIN(2 * pa->استعداد, CURL_PTRARRAY_MAX_GROW);
    ورنہ
      growth = ((UINT32_MAX - pa->استعداد) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->استعداد);

    ndata = curlx_calloc(1, (pa->استعداد + growth) * حجم(خالی *));
    اگر(!ndata)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(pa->n)
      حافظہ_نقل(ndata, pa->ڈیٹا, pa->n * حجم(خالی *));
    curlx_free(pa->ڈیٹا);
    pa->ڈیٹا = ndata;
    pa->استعداد += growth;
  }
  DEBUGASSERT(pa->n < pa->استعداد);
  pa->ڈیٹا[pa->n++] = ptr;
  واپس CURLE_OK;
}

خالی *Curl_ptrarray_remove(ساخت ptrarray *pa, uint32_t idx)
{
  اگر(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    خالی *ptr = pa->ڈیٹا[idx];
    pa->ڈیٹا[idx] = تہی;
    اگر(rem)
      memmove(pa->ڈیٹا + idx, pa->ڈیٹا + idx + 1, rem * حجم(خالی *));
    pa->n--;
    اگر(!pa->n)
      Curl_ptrarray_clear(pa);
    واپس ptr;
  }
  واپس تہی;
}

خالی *Curl_ptrarray_get(ساخت ptrarray *pa, uint32_t idx)
{
  واپس (idx < pa->n) ? pa->ڈیٹا[idx] : تہی;
}
