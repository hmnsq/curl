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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

শূন্য Curl_uint32_tbl_init(সংরচনা uint32_tbl *tbl)
{
  memset(tbl, 0, পরিমাণ(*tbl));
  tbl->last_চাবি_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

স্থির শূন্য uint32_tbl_clear_rows(সংরচনা uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  বাবে(i = from; i < end; ++i) {
    যদি(tbl->rows[i]) {
      tbl->rows[i] = রিক্ত;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(সংরচনা uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  যদি(!nrows)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(nrows != tbl->nrows) {
    শূন্য **rows = curlx_calloc(nrows, পরিমাণ(শূন্য *));
    যদি(!rows)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    যদি(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * পরিমাণ(শূন্য *)));
      যদি(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  ঘূৰাই CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST শূন্য uint32_tbl_clear(সংরচনা uint32_tbl *tbl);
UNITTEST শূন্য uint32_tbl_clear(সংরচনা uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_চাবি_added = UINT32_MAX;
}

শূন্য Curl_uint32_tbl_destroy(সংরচনা uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, পরিমাণ(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(সংরচনা uint32_tbl *tbl)
{
  ঘূৰাই tbl->nrows;
}

uint32_t Curl_uint32_tbl_গুনতি(সংরচনা uint32_tbl *tbl)
{
  ঘূৰাই tbl->nentries;
}

শূন্য *Curl_uint32_tbl_get(সংরচনা uint32_tbl *tbl, uint32_t চাবি)
{
  ঘূৰাই (চাবি < tbl->nrows) ? tbl->rows[চাবি] : রিক্ত;
}

bool Curl_uint32_tbl_add(সংরচনা uint32_tbl *tbl, শূন্য *entry, uint32_t *pkey)
{
  uint32_t চাবি, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  যদি(!entry || !pkey)
    ঘূৰাই FALSE;
  *pkey = UINT32_MAX;
  যদি(tbl->nentries == tbl->nrows)  /* full */
    ঘূৰাই FALSE;

  start_pos = CURLMIN(tbl->last_চাবি_added, tbl->nrows) + 1;
  বাবে(চাবি = start_pos; চাবি < tbl->nrows; ++চাবি) {
    যদি(!tbl->rows[চাবি]) {
      tbl->rows[চাবি] = entry;
      tbl->nentries++;
      tbl->last_চাবি_added = চাবি;
      *pkey = চাবি;
      ঘূৰাই TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  বাবে(চাবি = 0; চাবি < start_pos; ++চাবি) {
    যদি(!tbl->rows[চাবি]) {
      tbl->rows[চাবি] = entry;
      tbl->nentries++;
      tbl->last_চাবি_added = চাবি;
      *pkey = চাবি;
      ঘূৰাই TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  ঘূৰাই FALSE;
}

শূন্য Curl_uint32_tbl_remove(সংরচনা uint32_tbl *tbl, uint32_t চাবি)
{
  যদি(চাবি < tbl->nrows)
    uint32_tbl_clear_rows(tbl, চাবি, চাবি + 1);
}

bool Curl_uint32_tbl_contains(সংরচনা uint32_tbl *tbl, uint32_t চাবি)
{
  ঘূৰাই (চাবি < tbl->nrows) ? !!tbl->rows[চাবি] : FALSE;
}

স্থির bool uint32_tbl_next_at(সংরচনা uint32_tbl *tbl, uint32_t চাবি,
                               uint32_t *pkey, শূন্য **pentry)
{
  বাবে(; চাবি < tbl->nrows; ++চাবি) {
    যদি(tbl->rows[চাবি]) {
      *pkey = চাবি;
      *pentry = tbl->rows[চাবি];
      ঘূৰাই TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = রিক্ত;
  ঘূৰাই FALSE;
}

bool Curl_uint32_tbl_first(সংরচনা uint32_tbl *tbl,
                           uint32_t *pkey, শূন্য **pentry)
{
  যদি(!pkey || !pentry)
    ঘূৰাই FALSE;
  যদি(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    ঘূৰাই TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = রিক্ত;
  ঘূৰাই FALSE;
}

bool Curl_uint32_tbl_next(সংরচনা uint32_tbl *tbl, uint32_t last_চাবি,
                          uint32_t *pkey, শূন্য **pentry)
{
  যদি(!pkey || !pentry)
    ঘূৰাই FALSE;
  যদি(uint32_tbl_next_at(tbl, last_চাবি + 1, pkey, pentry))
    ঘূৰাই TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = রিক্ত;
  ঘূৰাই FALSE;
}
