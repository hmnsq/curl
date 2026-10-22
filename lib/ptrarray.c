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

#परिभाषय CURL_PTRARRAY_MAX_GROW     (16 * 1024)

शून्य Curl_ptrarray_init(संरचना ptrarray *pa)
{
  pa->दत्त = रिक्त;
  pa->n = pa->क्षमता = 0;
}

शून्य Curl_ptrarray_clear(संरचना ptrarray *pa)
{
  curlx_safefree(pa->दत्त);
  pa->n = pa->क्षमता = 0;
}

uint32_t Curl_ptrarray_गणनम्(संरचना ptrarray *pa)
{
  प्रत्यागम pa->n;
}

CURLcode Curl_ptrarray_सङ्कलय(संरचना ptrarray *pa, शून्य *ptr)
{
  यदि(!ptr)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  यदि(pa->n >= pa->क्षमता) {
    uint32_t growth;
    शून्य **ndata;

    यदि(pa->क्षमता == UINT32_MAX)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(!pa->क्षमता)
      growth = 8;
    अन्यथा यदि((UINT32_MAX / 2) > pa->क्षमता)
      growth = CURLMIN(2 * pa->क्षमता, CURL_PTRARRAY_MAX_GROW);
    अन्यथा
      growth = ((UINT32_MAX - pa->क्षमता) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->क्षमता);

    ndata = curlx_calloc(1, (pa->क्षमता + growth) * परिमाण(शून्य *));
    यदि(!ndata)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(pa->n)
      स्मृतिं_प्रतिलिख(ndata, pa->दत्त, pa->n * परिमाण(शून्य *));
    curlx_free(pa->दत्त);
    pa->दत्त = ndata;
    pa->क्षमता += growth;
  }
  DEBUGASSERT(pa->n < pa->क्षमता);
  pa->दत्त[pa->n++] = ptr;
  प्रत्यागम CURLE_OK;
}

शून्य *Curl_ptrarray_remove(संरचना ptrarray *pa, uint32_t idx)
{
  यदि(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    शून्य *ptr = pa->दत्त[idx];
    pa->दत्त[idx] = रिक्त;
    यदि(rem)
      memmove(pa->दत्त + idx, pa->दत्त + idx + 1, rem * परिमाण(शून्य *));
    pa->n--;
    यदि(!pa->n)
      Curl_ptrarray_clear(pa);
    प्रत्यागम ptr;
  }
  प्रत्यागम रिक्त;
}

शून्य *Curl_ptrarray_get(संरचना ptrarray *pa, uint32_t idx)
{
  प्रत्यागम (idx < pa->n) ? pa->दत्त[idx] : रिक्त;
}
