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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

ശൂന്യം Curl_uint32_tbl_init(ഘടന uint32_tbl *tbl)
{
  memset(tbl, 0, വലുപ്പം(*tbl));
  tbl->last_താക്കോൽ_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

സ്ഥിതം ശൂന്യം uint32_tbl_clear_rows(ഘടന uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  വേണ്ടി(i = from; i < end; ++i) {
    എങ്കിൽ(tbl->rows[i]) {
      tbl->rows[i] = ശൂന്യ;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(ഘടന uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  എങ്കിൽ(!nrows)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  എങ്കിൽ(nrows != tbl->nrows) {
    ശൂന്യം **rows = curlx_calloc(nrows, വലുപ്പം(ശൂന്യം *));
    എങ്കിൽ(!rows)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * വലുപ്പം(ശൂന്യം *)));
      എങ്കിൽ(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  തിരികെ CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST ശൂന്യം uint32_tbl_clear(ഘടന uint32_tbl *tbl);
UNITTEST ശൂന്യം uint32_tbl_clear(ഘടന uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_താക്കോൽ_added = UINT32_MAX;
}

ശൂന്യം Curl_uint32_tbl_destroy(ഘടന uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, വലുപ്പം(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(ഘടന uint32_tbl *tbl)
{
  തിരികെ tbl->nrows;
}

uint32_t Curl_uint32_tbl_എണ്ണം(ഘടന uint32_tbl *tbl)
{
  തിരികെ tbl->nentries;
}

ശൂന്യം *Curl_uint32_tbl_get(ഘടന uint32_tbl *tbl, uint32_t താക്കോൽ)
{
  തിരികെ (താക്കോൽ < tbl->nrows) ? tbl->rows[താക്കോൽ] : ശൂന്യ;
}

bool Curl_uint32_tbl_add(ഘടന uint32_tbl *tbl, ശൂന്യം *entry, uint32_t *pkey)
{
  uint32_t താക്കോൽ, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  എങ്കിൽ(!entry || !pkey)
    തിരികെ FALSE;
  *pkey = UINT32_MAX;
  എങ്കിൽ(tbl->nentries == tbl->nrows)  /* full */
    തിരികെ FALSE;

  start_pos = CURLMIN(tbl->last_താക്കോൽ_added, tbl->nrows) + 1;
  വേണ്ടി(താക്കോൽ = start_pos; താക്കോൽ < tbl->nrows; ++താക്കോൽ) {
    എങ്കിൽ(!tbl->rows[താക്കോൽ]) {
      tbl->rows[താക്കോൽ] = entry;
      tbl->nentries++;
      tbl->last_താക്കോൽ_added = താക്കോൽ;
      *pkey = താക്കോൽ;
      തിരികെ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  വേണ്ടി(താക്കോൽ = 0; താക്കോൽ < start_pos; ++താക്കോൽ) {
    എങ്കിൽ(!tbl->rows[താക്കോൽ]) {
      tbl->rows[താക്കോൽ] = entry;
      tbl->nentries++;
      tbl->last_താക്കോൽ_added = താക്കോൽ;
      *pkey = താക്കോൽ;
      തിരികെ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  തിരികെ FALSE;
}

ശൂന്യം Curl_uint32_tbl_remove(ഘടന uint32_tbl *tbl, uint32_t താക്കോൽ)
{
  എങ്കിൽ(താക്കോൽ < tbl->nrows)
    uint32_tbl_clear_rows(tbl, താക്കോൽ, താക്കോൽ + 1);
}

bool Curl_uint32_tbl_contains(ഘടന uint32_tbl *tbl, uint32_t താക്കോൽ)
{
  തിരികെ (താക്കോൽ < tbl->nrows) ? !!tbl->rows[താക്കോൽ] : FALSE;
}

സ്ഥിതം bool uint32_tbl_next_at(ഘടന uint32_tbl *tbl, uint32_t താക്കോൽ,
                               uint32_t *pkey, ശൂന്യം **pentry)
{
  വേണ്ടി(; താക്കോൽ < tbl->nrows; ++താക്കോൽ) {
    എങ്കിൽ(tbl->rows[താക്കോൽ]) {
      *pkey = താക്കോൽ;
      *pentry = tbl->rows[താക്കോൽ];
      തിരികെ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ശൂന്യ;
  തിരികെ FALSE;
}

bool Curl_uint32_tbl_first(ഘടന uint32_tbl *tbl,
                           uint32_t *pkey, ശൂന്യം **pentry)
{
  എങ്കിൽ(!pkey || !pentry)
    തിരികെ FALSE;
  എങ്കിൽ(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    തിരികെ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ശൂന്യ;
  തിരികെ FALSE;
}

bool Curl_uint32_tbl_next(ഘടന uint32_tbl *tbl, uint32_t last_താക്കോൽ,
                          uint32_t *pkey, ശൂന്യം **pentry)
{
  എങ്കിൽ(!pkey || !pentry)
    തിരികെ FALSE;
  എങ്കിൽ(uint32_tbl_next_at(tbl, last_താക്കോൽ + 1, pkey, pentry))
    തിരികെ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ശൂന്യ;
  തിരികെ FALSE;
}
