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

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

ಶೂನ್ಯ Curl_uint32_tbl_init(ಸಂರಚನಾ uint32_tbl *tbl)
{
  memset(tbl, 0, ಪರಿಮಾಣ(*tbl));
  tbl->last_ಕೀಲಿ_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ uint32_tbl_clear_rows(ಸಂರಚನಾ uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  ಪ್ರತಿ(i = from; i < end; ++i) {
    ಯದಿ(tbl->rows[i]) {
      tbl->rows[i] = ರಿಕ್ತ;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(ಸಂರಚನಾ uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ಯದಿ(!nrows)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಯದಿ(nrows != tbl->nrows) {
    ಶೂನ್ಯ **rows = curlx_calloc(nrows, ಪರಿಮಾಣ(ಶೂನ್ಯ *));
    ಯದಿ(!rows)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * ಪರಿಮಾಣ(ಶೂನ್ಯ *)));
      ಯದಿ(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST ಶೂನ್ಯ uint32_tbl_clear(ಸಂರಚನಾ uint32_tbl *tbl);
UNITTEST ಶೂನ್ಯ uint32_tbl_clear(ಸಂರಚನಾ uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_ಕೀಲಿ_added = UINT32_MAX;
}

ಶೂನ್ಯ Curl_uint32_tbl_destroy(ಸಂರಚನಾ uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, ಪರಿಮಾಣ(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(ಸಂರಚನಾ uint32_tbl *tbl)
{
  ಪ್ರತ್ಯಾಗಮ tbl->nrows;
}

uint32_t Curl_uint32_tbl_ಎಣಿಕೆ(ಸಂರಚನಾ uint32_tbl *tbl)
{
  ಪ್ರತ್ಯಾಗಮ tbl->nentries;
}

ಶೂನ್ಯ *Curl_uint32_tbl_get(ಸಂರಚನಾ uint32_tbl *tbl, uint32_t ಕೀಲಿ)
{
  ಪ್ರತ್ಯಾಗಮ (ಕೀಲಿ < tbl->nrows) ? tbl->rows[ಕೀಲಿ] : ರಿಕ್ತ;
}

bool Curl_uint32_tbl_add(ಸಂರಚನಾ uint32_tbl *tbl, ಶೂನ್ಯ *entry, uint32_t *pkey)
{
  uint32_t ಕೀಲಿ, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ಯದಿ(!entry || !pkey)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  *pkey = UINT32_MAX;
  ಯದಿ(tbl->nentries == tbl->nrows)  /* full */
    ಪ್ರತ್ಯಾಗಮ FALSE;

  start_pos = CURLMIN(tbl->last_ಕೀಲಿ_added, tbl->nrows) + 1;
  ಪ್ರತಿ(ಕೀಲಿ = start_pos; ಕೀಲಿ < tbl->nrows; ++ಕೀಲಿ) {
    ಯದಿ(!tbl->rows[ಕೀಲಿ]) {
      tbl->rows[ಕೀಲಿ] = entry;
      tbl->nentries++;
      tbl->last_ಕೀಲಿ_added = ಕೀಲಿ;
      *pkey = ಕೀಲಿ;
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  ಪ್ರತಿ(ಕೀಲಿ = 0; ಕೀಲಿ < start_pos; ++ಕೀಲಿ) {
    ಯದಿ(!tbl->rows[ಕೀಲಿ]) {
      tbl->rows[ಕೀಲಿ] = entry;
      tbl->nentries++;
      tbl->last_ಕೀಲಿ_added = ಕೀಲಿ;
      *pkey = ಕೀಲಿ;
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಶೂನ್ಯ Curl_uint32_tbl_remove(ಸಂರಚನಾ uint32_tbl *tbl, uint32_t ಕೀಲಿ)
{
  ಯದಿ(ಕೀಲಿ < tbl->nrows)
    uint32_tbl_clear_rows(tbl, ಕೀಲಿ, ಕೀಲಿ + 1);
}

bool Curl_uint32_tbl_contains(ಸಂರಚನಾ uint32_tbl *tbl, uint32_t ಕೀಲಿ)
{
  ಪ್ರತ್ಯಾಗಮ (ಕೀಲಿ < tbl->nrows) ? !!tbl->rows[ಕೀಲಿ] : FALSE;
}

ಸ್ಥಿರ bool uint32_tbl_next_at(ಸಂರಚನಾ uint32_tbl *tbl, uint32_t ಕೀಲಿ,
                               uint32_t *pkey, ಶೂನ್ಯ **pentry)
{
  ಪ್ರತಿ(; ಕೀಲಿ < tbl->nrows; ++ಕೀಲಿ) {
    ಯದಿ(tbl->rows[ಕೀಲಿ]) {
      *pkey = ಕೀಲಿ;
      *pentry = tbl->rows[ಕೀಲಿ];
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_uint32_tbl_first(ಸಂರಚನಾ uint32_tbl *tbl,
                           uint32_t *pkey, ಶೂನ್ಯ **pentry)
{
  ಯದಿ(!pkey || !pentry)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_uint32_tbl_next(ಸಂರಚನಾ uint32_tbl *tbl, uint32_t last_ಕೀಲಿ,
                          uint32_t *pkey, ಶೂನ್ಯ **pentry)
{
  ಯದಿ(!pkey || !pentry)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(uint32_tbl_next_at(tbl, last_ಕೀಲಿ + 1, pkey, pentry))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}
