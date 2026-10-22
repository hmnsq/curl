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

#ضمن "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

فراغ Curl_uint32_tbl_init(بنية uint32_tbl *tbl)
{
  memset(tbl, 0, حجم(*tbl));
  tbl->last_کلیل_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

ساكن فراغ uint32_tbl_clear_rows(بنية uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  لكل(i = from; i < end; ++i) {
    إذا(tbl->rows[i]) {
      tbl->rows[i] = عدم;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(بنية uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!nrows)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  إذا(nrows != tbl->nrows) {
    فراغ **rows = curlx_calloc(nrows, حجم(فراغ *));
    إذا(!rows)
      أرجع CURLE_OUT_OF_MEMORY;
    إذا(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * حجم(فراغ *)));
      إذا(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  أرجع CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST فراغ uint32_tbl_clear(بنية uint32_tbl *tbl);
UNITTEST فراغ uint32_tbl_clear(بنية uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_کلیل_added = UINT32_MAX;
}

فراغ Curl_uint32_tbl_destroy(بنية uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, حجم(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(بنية uint32_tbl *tbl)
{
  أرجع tbl->nrows;
}

uint32_t Curl_uint32_tbl_ژماردن(بنية uint32_tbl *tbl)
{
  أرجع tbl->nentries;
}

فراغ *Curl_uint32_tbl_get(بنية uint32_tbl *tbl, uint32_t کلیل)
{
  أرجع (کلیل < tbl->nrows) ? tbl->rows[کلیل] : عدم;
}

bool Curl_uint32_tbl_add(بنية uint32_tbl *tbl, فراغ *entry, uint32_t *pkey)
{
  uint32_t کلیل, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!entry || !pkey)
    أرجع FALSE;
  *pkey = UINT32_MAX;
  إذا(tbl->nentries == tbl->nrows)  /* full */
    أرجع FALSE;

  start_pos = CURLMIN(tbl->last_کلیل_added, tbl->nrows) + 1;
  لكل(کلیل = start_pos; کلیل < tbl->nrows; ++کلیل) {
    إذا(!tbl->rows[کلیل]) {
      tbl->rows[کلیل] = entry;
      tbl->nentries++;
      tbl->last_کلیل_added = کلیل;
      *pkey = کلیل;
      أرجع TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  لكل(کلیل = 0; کلیل < start_pos; ++کلیل) {
    إذا(!tbl->rows[کلیل]) {
      tbl->rows[کلیل] = entry;
      tbl->nentries++;
      tbl->last_کلیل_added = کلیل;
      *pkey = کلیل;
      أرجع TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  أرجع FALSE;
}

فراغ Curl_uint32_tbl_remove(بنية uint32_tbl *tbl, uint32_t کلیل)
{
  إذا(کلیل < tbl->nrows)
    uint32_tbl_clear_rows(tbl, کلیل, کلیل + 1);
}

bool Curl_uint32_tbl_contains(بنية uint32_tbl *tbl, uint32_t کلیل)
{
  أرجع (کلیل < tbl->nrows) ? !!tbl->rows[کلیل] : FALSE;
}

ساكن bool uint32_tbl_next_at(بنية uint32_tbl *tbl, uint32_t کلیل,
                               uint32_t *pkey, فراغ **pentry)
{
  لكل(; کلیل < tbl->nrows; ++کلیل) {
    إذا(tbl->rows[کلیل]) {
      *pkey = کلیل;
      *pentry = tbl->rows[کلیل];
      أرجع TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_first(بنية uint32_tbl *tbl,
                           uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    أرجع TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_next(بنية uint32_tbl *tbl, uint32_t last_کلیل,
                          uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(uint32_tbl_next_at(tbl, last_کلیل + 1, pkey, pentry))
    أرجع TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}
