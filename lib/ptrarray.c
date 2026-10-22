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
#ضمن "curl_setup.h"

#ضمن "ptrarray.h"

#تعریف_کن CURL_PTRARRAY_MAX_GROW     (16 * 1024)

فراغ Curl_ptrarray_init(بنية ptrarray *pa)
{
  pa->داده = عدم;
  pa->n = pa->ظرفیت = 0;
}

فراغ Curl_ptrarray_clear(بنية ptrarray *pa)
{
  curlx_safefree(pa->داده);
  pa->n = pa->ظرفیت = 0;
}

uint32_t Curl_ptrarray_شمار(بنية ptrarray *pa)
{
  أرجع pa->n;
}

CURLcode Curl_ptrarray_جمع_کن(بنية ptrarray *pa, فراغ *ptr)
{
  إذا(!ptr)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  إذا(pa->n >= pa->ظرفیت) {
    uint32_t growth;
    فراغ **ndata;

    إذا(pa->ظرفیت == UINT32_MAX)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(!pa->ظرفیت)
      growth = 8;
    وإلا إذا((UINT32_MAX / 2) > pa->ظرفیت)
      growth = CURLMIN(2 * pa->ظرفیت, CURL_PTRARRAY_MAX_GROW);
    وإلا
      growth = ((UINT32_MAX - pa->ظرفیت) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->ظرفیت);

    ndata = curlx_calloc(1, (pa->ظرفیت + growth) * حجم(فراغ *));
    إذا(!ndata)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(pa->n)
      کپی_حافظه(ndata, pa->داده, pa->n * حجم(فراغ *));
    curlx_free(pa->داده);
    pa->داده = ndata;
    pa->ظرفیت += growth;
  }
  DEBUGASSERT(pa->n < pa->ظرفیت);
  pa->داده[pa->n++] = ptr;
  أرجع CURLE_OK;
}

فراغ *Curl_ptrarray_remove(بنية ptrarray *pa, uint32_t idx)
{
  إذا(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    فراغ *ptr = pa->داده[idx];
    pa->داده[idx] = عدم;
    إذا(rem)
      memmove(pa->داده + idx, pa->داده + idx + 1, rem * حجم(فراغ *));
    pa->n--;
    إذا(!pa->n)
      Curl_ptrarray_clear(pa);
    أرجع ptr;
  }
  أرجع عدم;
}

فراغ *Curl_ptrarray_get(بنية ptrarray *pa, uint32_t idx)
{
  أرجع (idx < pa->n) ? pa->داده[idx] : عدم;
}
