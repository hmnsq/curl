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
  tbl->last_key_added = UINT32_MAX;
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
  עבור(i = from; i < end; ++i) {
    אם(tbl->rows[i]) {
      tbl->rows[i] = عدم;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(بنية uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  אם(!nrows)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  אם(nrows != tbl->nrows) {
    فراغ **rows = curlx_calloc(nrows, حجم(فراغ *));
    אם(!rows)
      החזר CURLE_OUT_OF_MEMORY;
    אם(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * حجم(فراغ *)));
      אם(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  החזר CURLE_OK;
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
  tbl->last_key_added = UINT32_MAX;
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
  החזר tbl->nrows;
}

uint32_t Curl_uint32_tbl_count(بنية uint32_tbl *tbl)
{
  החזר tbl->nentries;
}

فراغ *Curl_uint32_tbl_get(بنية uint32_tbl *tbl, uint32_t key)
{
  החזר (key < tbl->nrows) ? tbl->rows[key] : عدم;
}

bool Curl_uint32_tbl_add(بنية uint32_tbl *tbl, فراغ *entry, uint32_t *pkey)
{
  uint32_t key, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  אם(!entry || !pkey)
    החזר FALSE;
  *pkey = UINT32_MAX;
  אם(tbl->nentries == tbl->nrows)  /* full */
    החזר FALSE;

  start_pos = CURLMIN(tbl->last_key_added, tbl->nrows) + 1;
  עבור(key = start_pos; key < tbl->nrows; ++key) {
    אם(!tbl->rows[key]) {
      tbl->rows[key] = entry;
      tbl->nentries++;
      tbl->last_key_added = key;
      *pkey = key;
      החזר TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  עבור(key = 0; key < start_pos; ++key) {
    אם(!tbl->rows[key]) {
      tbl->rows[key] = entry;
      tbl->nentries++;
      tbl->last_key_added = key;
      *pkey = key;
      החזר TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  החזר FALSE;
}

فراغ Curl_uint32_tbl_remove(بنية uint32_tbl *tbl, uint32_t key)
{
  אם(key < tbl->nrows)
    uint32_tbl_clear_rows(tbl, key, key + 1);
}

bool Curl_uint32_tbl_contains(بنية uint32_tbl *tbl, uint32_t key)
{
  החזר (key < tbl->nrows) ? !!tbl->rows[key] : FALSE;
}

ساكن bool uint32_tbl_next_at(بنية uint32_tbl *tbl, uint32_t key,
                               uint32_t *pkey, فراغ **pentry)
{
  עבור(; key < tbl->nrows; ++key) {
    אם(tbl->rows[key]) {
      *pkey = key;
      *pentry = tbl->rows[key];
      החזר TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  החזר FALSE;
}

bool Curl_uint32_tbl_first(بنية uint32_tbl *tbl,
                           uint32_t *pkey, فراغ **pentry)
{
  אם(!pkey || !pentry)
    החזר FALSE;
  אם(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    החזר TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  החזר FALSE;
}

bool Curl_uint32_tbl_next(بنية uint32_tbl *tbl, uint32_t last_key,
                          uint32_t *pkey, فراغ **pentry)
{
  אם(!pkey || !pentry)
    החזר FALSE;
  אם(uint32_tbl_next_at(tbl, last_key + 1, pkey, pentry))
    החזר TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  החזר FALSE;
}
