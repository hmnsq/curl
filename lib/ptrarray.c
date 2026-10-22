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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "ptrarray.h"

#నిర్వచించు CURL_PTRARRAY_MAX_GROW     (16 * 1024)

శూన్య Curl_ptrarray_init(నిర్మాణం ptrarray *pa)
{
  pa->దత్తాంశం = ఖాళీ;
  pa->n = pa->సామర్థ్యం = 0;
}

శూన్య Curl_ptrarray_clear(నిర్మాణం ptrarray *pa)
{
  curlx_safefree(pa->దత్తాంశం);
  pa->n = pa->సామర్థ్యం = 0;
}

uint32_t Curl_ptrarray_లెక్క(నిర్మాణం ptrarray *pa)
{
  తిరిగి pa->n;
}

CURLcode Curl_ptrarray_కూడు(నిర్మాణం ptrarray *pa, శూన్య *ptr)
{
  ఒకవేళ(!ptr)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఒకవేళ(pa->n >= pa->సామర్థ్యం) {
    uint32_t growth;
    శూన్య **ndata;

    ఒకవేళ(pa->సామర్థ్యం == UINT32_MAX)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(!pa->సామర్థ్యం)
      growth = 8;
    లేకపోతే ఒకవేళ((UINT32_MAX / 2) > pa->సామర్థ్యం)
      growth = CURLMIN(2 * pa->సామర్థ్యం, CURL_PTRARRAY_MAX_GROW);
    లేకపోతే
      growth = ((UINT32_MAX - pa->సామర్థ్యం) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->సామర్థ్యం);

    ndata = curlx_calloc(1, (pa->సామర్థ్యం + growth) * పరిమాణం(శూన్య *));
    ఒకవేళ(!ndata)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(pa->n)
      జ్ఞాపకం_నకలు(ndata, pa->దత్తాంశం, pa->n * పరిమాణం(శూన్య *));
    curlx_free(pa->దత్తాంశం);
    pa->దత్తాంశం = ndata;
    pa->సామర్థ్యం += growth;
  }
  DEBUGASSERT(pa->n < pa->సామర్థ్యం);
  pa->దత్తాంశం[pa->n++] = ptr;
  తిరిగి CURLE_OK;
}

శూన్య *Curl_ptrarray_remove(నిర్మాణం ptrarray *pa, uint32_t idx)
{
  ఒకవేళ(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    శూన్య *ptr = pa->దత్తాంశం[idx];
    pa->దత్తాంశం[idx] = ఖాళీ;
    ఒకవేళ(rem)
      memmove(pa->దత్తాంశం + idx, pa->దత్తాంశం + idx + 1, rem * పరిమాణం(శూన్య *));
    pa->n--;
    ఒకవేళ(!pa->n)
      Curl_ptrarray_clear(pa);
    తిరిగి ptr;
  }
  తిరిగి ఖాళీ;
}

శూన్య *Curl_ptrarray_get(నిర్మాణం ptrarray *pa, uint32_t idx)
{
  తిరిగి (idx < pa->n) ? pa->దత్తాంశం[idx] : ఖాళీ;
}
