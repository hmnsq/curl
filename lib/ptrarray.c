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

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PTRARRAY_MAX_GROW     (16 * 1024)

ಶೂನ್ಯ Curl_ptrarray_init(ರಚನೆ ptrarray *pa)
{
  pa->ದತ್ತಾಂಶ = ಖಾಲಿ;
  pa->n = pa->ಸಾಮರ್ಥ್ಯ = 0;
}

ಶೂನ್ಯ Curl_ptrarray_clear(ರಚನೆ ptrarray *pa)
{
  curlx_safefree(pa->ದತ್ತಾಂಶ);
  pa->n = pa->ಸಾಮರ್ಥ್ಯ = 0;
}

uint32_t Curl_ptrarray_ಎಣಿಕೆ(ರಚನೆ ptrarray *pa)
{
  ಹಿಂತಿರುಗಿ pa->n;
}

CURLcode Curl_ptrarray_ಕೂಡಿಸು(ರಚನೆ ptrarray *pa, ಶೂನ್ಯ *ptr)
{
  ಆದರೆ(!ptr)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಆದರೆ(pa->n >= pa->ಸಾಮರ್ಥ್ಯ) {
    uint32_t growth;
    ಶೂನ್ಯ **ndata;

    ಆದರೆ(pa->ಸಾಮರ್ಥ್ಯ == UINT32_MAX)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(!pa->ಸಾಮರ್ಥ್ಯ)
      growth = 8;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((UINT32_MAX / 2) > pa->ಸಾಮರ್ಥ್ಯ)
      growth = CURLMIN(2 * pa->ಸಾಮರ್ಥ್ಯ, CURL_PTRARRAY_MAX_GROW);
    ಇಲ್ಲದಿದ್ದರೆ
      growth = ((UINT32_MAX - pa->ಸಾಮರ್ಥ್ಯ) > CURL_PTRARRAY_MAX_GROW) ?
               CURL_PTRARRAY_MAX_GROW : (UINT32_MAX - pa->ಸಾಮರ್ಥ್ಯ);

    ndata = curlx_calloc(1, (pa->ಸಾಮರ್ಥ್ಯ + growth) * ಗಾತ್ರ(ಶೂನ್ಯ *));
    ಆದರೆ(!ndata)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(pa->n)
      ಸ್ಮರಣೆ_ನಕಲು(ndata, pa->ದತ್ತಾಂಶ, pa->n * ಗಾತ್ರ(ಶೂನ್ಯ *));
    curlx_free(pa->ದತ್ತಾಂಶ);
    pa->ದತ್ತಾಂಶ = ndata;
    pa->ಸಾಮರ್ಥ್ಯ += growth;
  }
  DEBUGASSERT(pa->n < pa->ಸಾಮರ್ಥ್ಯ);
  pa->ದತ್ತಾಂಶ[pa->n++] = ptr;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ *Curl_ptrarray_remove(ರಚನೆ ptrarray *pa, uint32_t idx)
{
  ಆದರೆ(idx < pa->n) {
    uint32_t rem = pa->n - idx - 1;
    ಶೂನ್ಯ *ptr = pa->ದತ್ತಾಂಶ[idx];
    pa->ದತ್ತಾಂಶ[idx] = ಖಾಲಿ;
    ಆದರೆ(rem)
      memmove(pa->ದತ್ತಾಂಶ + idx, pa->ದತ್ತಾಂಶ + idx + 1, rem * ಗಾತ್ರ(ಶೂನ್ಯ *));
    pa->n--;
    ಆದರೆ(!pa->n)
      Curl_ptrarray_clear(pa);
    ಹಿಂತಿರುಗಿ ptr;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ *Curl_ptrarray_get(ರಚನೆ ptrarray *pa, uint32_t idx)
{
  ಹಿಂತಿರುಗಿ (idx < pa->n) ? pa->ದತ್ತಾಂಶ[idx] : ಖಾಲಿ;
}
