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

#شامل "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

تهی Curl_uint32_tbl_init(ساختار uint32_tbl *tbl)
{
  memset(tbl, 0, اندازه(*tbl));
  tbl->last_کلید_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

ایستا تهی uint32_tbl_clear_rows(ساختار uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  برای(i = from; i < end; ++i) {
    اگر(tbl->rows[i]) {
      tbl->rows[i] = پوچ;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(ساختار uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  اگر(!nrows)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  اگر(nrows != tbl->nrows) {
    تهی **rows = curlx_calloc(nrows, اندازه(تهی *));
    اگر(!rows)
      بازگشت CURLE_OUT_OF_MEMORY;
    اگر(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * اندازه(تهی *)));
      اگر(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  بازگشت CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST تهی uint32_tbl_clear(ساختار uint32_tbl *tbl);
UNITTEST تهی uint32_tbl_clear(ساختار uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_کلید_added = UINT32_MAX;
}

تهی Curl_uint32_tbl_destroy(ساختار uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, اندازه(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(ساختار uint32_tbl *tbl)
{
  بازگشت tbl->nrows;
}

uint32_t Curl_uint32_tbl_شمار(ساختار uint32_tbl *tbl)
{
  بازگشت tbl->nentries;
}

تهی *Curl_uint32_tbl_get(ساختار uint32_tbl *tbl, uint32_t کلید)
{
  بازگشت (کلید < tbl->nrows) ? tbl->rows[کلید] : پوچ;
}

bool Curl_uint32_tbl_add(ساختار uint32_tbl *tbl, تهی *entry, uint32_t *pkey)
{
  uint32_t کلید, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  اگر(!entry || !pkey)
    بازگشت FALSE;
  *pkey = UINT32_MAX;
  اگر(tbl->nentries == tbl->nrows)  /* full */
    بازگشت FALSE;

  start_pos = CURLMIN(tbl->last_کلید_added, tbl->nrows) + 1;
  برای(کلید = start_pos; کلید < tbl->nrows; ++کلید) {
    اگر(!tbl->rows[کلید]) {
      tbl->rows[کلید] = entry;
      tbl->nentries++;
      tbl->last_کلید_added = کلید;
      *pkey = کلید;
      بازگشت TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  برای(کلید = 0; کلید < start_pos; ++کلید) {
    اگر(!tbl->rows[کلید]) {
      tbl->rows[کلید] = entry;
      tbl->nentries++;
      tbl->last_کلید_added = کلید;
      *pkey = کلید;
      بازگشت TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  بازگشت FALSE;
}

تهی Curl_uint32_tbl_remove(ساختار uint32_tbl *tbl, uint32_t کلید)
{
  اگر(کلید < tbl->nrows)
    uint32_tbl_clear_rows(tbl, کلید, کلید + 1);
}

bool Curl_uint32_tbl_contains(ساختار uint32_tbl *tbl, uint32_t کلید)
{
  بازگشت (کلید < tbl->nrows) ? !!tbl->rows[کلید] : FALSE;
}

ایستا bool uint32_tbl_next_at(ساختار uint32_tbl *tbl, uint32_t کلید,
                               uint32_t *pkey, تهی **pentry)
{
  برای(; کلید < tbl->nrows; ++کلید) {
    اگر(tbl->rows[کلید]) {
      *pkey = کلید;
      *pentry = tbl->rows[کلید];
      بازگشت TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = پوچ;
  بازگشت FALSE;
}

bool Curl_uint32_tbl_first(ساختار uint32_tbl *tbl,
                           uint32_t *pkey, تهی **pentry)
{
  اگر(!pkey || !pentry)
    بازگشت FALSE;
  اگر(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    بازگشت TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = پوچ;
  بازگشت FALSE;
}

bool Curl_uint32_tbl_next(ساختار uint32_tbl *tbl, uint32_t last_کلید,
                          uint32_t *pkey, تهی **pentry)
{
  اگر(!pkey || !pentry)
    بازگشت FALSE;
  اگر(uint32_tbl_next_at(tbl, last_کلید + 1, pkey, pentry))
    بازگشت TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = پوچ;
  بازگشت FALSE;
}
