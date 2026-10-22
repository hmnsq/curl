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

#ಅನ್ತರ್ಭಾವ "uint-table.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_UINT32_TBL_MAGIC  0x62757473
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_uint32_tbl_init(ರಚನೆ uint32_tbl *tbl)
{
  memset(tbl, 0, ಗಾತ್ರ(*tbl));
  tbl->ಕೊನೆಯ_ಕೀಲಿ_added = UINT32_MAX;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ uint32_tbl_clear_ಅಡ್ಡಸಾಲುಗಳು(ರಚನೆ uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, ಕೊನೆ;

  ಕೊನೆ = CURLMIN(upto_excluding, tbl->nrows);
  ಗಾಗಿ(i = from; i < ಕೊನೆ; ++i) {
    ಆದರೆ(tbl->ಅಡ್ಡಸಾಲುಗಳು[i]) {
      tbl->ಅಡ್ಡಸಾಲುಗಳು[i] = ಖಾಲಿ;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(ರಚನೆ uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ಆದರೆ(!nrows)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಆದರೆ(nrows != tbl->nrows) {
    ಶೂನ್ಯ **ಅಡ್ಡಸಾಲುಗಳು = curlx_calloc(nrows, ಗಾತ್ರ(ಶೂನ್ಯ *));
    ಆದರೆ(!ಅಡ್ಡಸಾಲುಗಳು)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(tbl->ಅಡ್ಡಸಾಲುಗಳು) {
      ಸ್ಮರಣೆ_ನಕಲು(ಅಡ್ಡಸಾಲುಗಳು, tbl->ಅಡ್ಡಸಾಲುಗಳು, (CURLMIN(nrows, tbl->nrows) * ಗಾತ್ರ(ಶೂನ್ಯ *)));
      ಆದರೆ(nrows < tbl->nrows)
        uint32_tbl_clear_ಅಡ್ಡಸಾಲುಗಳು(tbl, nrows, tbl->nrows);
      curlx_free(tbl->ಅಡ್ಡಸಾಲುಗಳು);
    }
    tbl->ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
    tbl->nrows = nrows;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST ಶೂನ್ಯ uint32_tbl_clear(ರಚನೆ uint32_tbl *tbl);
UNITTEST ಶೂನ್ಯ uint32_tbl_clear(ರಚನೆ uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_ಅಡ್ಡಸಾಲುಗಳು(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->ಕೊನೆಯ_ಕೀಲಿ_added = UINT32_MAX;
}

ಶೂನ್ಯ Curl_uint32_tbl_destroy(ರಚನೆ uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->ಅಡ್ಡಸಾಲುಗಳು);
  memset(tbl, 0, ಗಾತ್ರ(*tbl));
}

uint32_t Curl_uint32_tbl_ಸಾಮರ್ಥ್ಯ(ರಚನೆ uint32_tbl *tbl)
{
  ಹಿಂತಿರುಗಿ tbl->nrows;
}

uint32_t Curl_uint32_tbl_ಎಣಿಕೆ(ರಚನೆ uint32_tbl *tbl)
{
  ಹಿಂತಿರುಗಿ tbl->nentries;
}

ಶೂನ್ಯ *Curl_uint32_tbl_get(ರಚನೆ uint32_tbl *tbl, uint32_t ಕೀಲಿ)
{
  ಹಿಂತಿರುಗಿ (ಕೀಲಿ < tbl->nrows) ? tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ] : ಖಾಲಿ;
}

bool Curl_uint32_tbl_ಕೂಡಿಸು(ರಚನೆ uint32_tbl *tbl, ಶೂನ್ಯ *ನಮೂದು, uint32_t *pkey)
{
  uint32_t ಕೀಲಿ, ಆರಂಭ_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ಆದರೆ(!ನಮೂದು || !pkey)
    ಹಿಂತಿರುಗಿ FALSE;
  *pkey = UINT32_MAX;
  ಆದರೆ(tbl->nentries == tbl->nrows)  /*ತುಂಬಿದ*/
    ಹಿಂತಿರುಗಿ FALSE;

  ಆರಂಭ_pos = CURLMIN(tbl->ಕೊನೆಯ_ಕೀಲಿ_added, tbl->nrows) + 1;
  ಗಾಗಿ(ಕೀಲಿ = ಆರಂಭ_pos; ಕೀಲಿ < tbl->nrows; ++ಕೀಲಿ) {
    ಆದರೆ(!tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ]) {
      tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ] = ನಮೂದು;
      tbl->nentries++;
      tbl->ಕೊನೆಯ_ಕೀಲಿ_added = ಕೀಲಿ;
      *pkey = ಕೀಲಿ;
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  ಗಾಗಿ(ಕೀಲಿ = 0; ಕೀಲಿ < ಆರಂಭ_pos; ++ಕೀಲಿ) {
    ಆದರೆ(!tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ]) {
      tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ] = ನಮೂದು;
      tbl->nentries++;
      tbl->ಕೊನೆಯ_ಕೀಲಿ_added = ಕೀಲಿ;
      *pkey = ಕೀಲಿ;
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  ಹಿಂತಿರುಗಿ FALSE;
}

ಶೂನ್ಯ Curl_uint32_tbl_remove(ರಚನೆ uint32_tbl *tbl, uint32_t ಕೀಲಿ)
{
  ಆದರೆ(ಕೀಲಿ < tbl->nrows)
    uint32_tbl_clear_ಅಡ್ಡಸಾಲುಗಳು(tbl, ಕೀಲಿ, ಕೀಲಿ + 1);
}

bool Curl_uint32_tbl_ಒಳಗೊಂಡಿದೆ(ರಚನೆ uint32_tbl *tbl, uint32_t ಕೀಲಿ)
{
  ಹಿಂತಿರುಗಿ (ಕೀಲಿ < tbl->nrows) ? !!tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ] : FALSE;
}

ಸ್ಥಿರ bool uint32_tbl_ಮುಂದಿನ_at(ರಚನೆ uint32_tbl *tbl, uint32_t ಕೀಲಿ,
                               uint32_t *pkey, ಶೂನ್ಯ **pentry)
{
  ಗಾಗಿ(; ಕೀಲಿ < tbl->nrows; ++ಕೀಲಿ) {
    ಆದರೆ(tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ]) {
      *pkey = ಕೀಲಿ;
      *pentry = tbl->ಅಡ್ಡಸಾಲುಗಳು[ಕೀಲಿ];
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_uint32_tbl_ಮೊದಲ(ರಚನೆ uint32_tbl *tbl,
                           uint32_t *pkey, ಶೂನ್ಯ **pentry)
{
  ಆದರೆ(!pkey || !pentry)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(tbl->nentries && uint32_tbl_ಮುಂದಿನ_at(tbl, 0, pkey, pentry))
    ಹಿಂತಿರುಗಿ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_uint32_tbl_ಮುಂದಿನ(ರಚನೆ uint32_tbl *tbl, uint32_t ಕೊನೆಯ_ಕೀಲಿ,
                          uint32_t *pkey, ಶೂನ್ಯ **pentry)
{
  ಆದರೆ(!pkey || !pentry)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(uint32_tbl_ಮುಂದಿನ_at(tbl, ಕೊನೆಯ_ಕೀಲಿ + 1, pkey, pentry))
    ಹಿಂತಿರುಗಿ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ FALSE;
}
