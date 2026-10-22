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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

ଶୂନ୍ୟ Curl_uint32_tbl_init(ସଂରଚନା uint32_tbl *tbl)
{
  memset(tbl, 0, ଆକାର(*tbl));
  tbl->last_ଚାବି_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ uint32_tbl_clear_rows(ସଂରଚନା uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  ପାଇଁ(i = from; i < end; ++i) {
    ଯଦି(tbl->rows[i]) {
      tbl->rows[i] = ଖାଲି;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(ସଂରଚନା uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ଯଦି(!nrows)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଯଦି(nrows != tbl->nrows) {
    ଶୂନ୍ୟ **rows = curlx_calloc(nrows, ଆକାର(ଶୂନ୍ୟ *));
    ଯଦି(!rows)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * ଆକାର(ଶୂନ୍ୟ *)));
      ଯଦି(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST ଶୂନ୍ୟ uint32_tbl_clear(ସଂରଚନା uint32_tbl *tbl);
UNITTEST ଶୂନ୍ୟ uint32_tbl_clear(ସଂରଚନା uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_ଚାବି_added = UINT32_MAX;
}

ଶୂନ୍ୟ Curl_uint32_tbl_destroy(ସଂରଚନା uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, ଆକାର(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(ସଂରଚନା uint32_tbl *tbl)
{
  ଫେରନ୍ତୁ tbl->nrows;
}

uint32_t Curl_uint32_tbl_ଗଣନା(ସଂରଚନା uint32_tbl *tbl)
{
  ଫେରନ୍ତୁ tbl->nentries;
}

ଶୂନ୍ୟ *Curl_uint32_tbl_get(ସଂରଚନା uint32_tbl *tbl, uint32_t ଚାବି)
{
  ଫେରନ୍ତୁ (ଚାବି < tbl->nrows) ? tbl->rows[ଚାବି] : ଖାଲି;
}

bool Curl_uint32_tbl_add(ସଂରଚନା uint32_tbl *tbl, ଶୂନ୍ୟ *entry, uint32_t *pkey)
{
  uint32_t ଚାବି, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ଯଦି(!entry || !pkey)
    ଫେରନ୍ତୁ FALSE;
  *pkey = UINT32_MAX;
  ଯଦି(tbl->nentries == tbl->nrows)  /* full */
    ଫେରନ୍ତୁ FALSE;

  start_pos = CURLMIN(tbl->last_ଚାବି_added, tbl->nrows) + 1;
  ପାଇଁ(ଚାବି = start_pos; ଚାବି < tbl->nrows; ++ଚାବି) {
    ଯଦି(!tbl->rows[ଚାବି]) {
      tbl->rows[ଚାବି] = entry;
      tbl->nentries++;
      tbl->last_ଚାବି_added = ଚାବି;
      *pkey = ଚାବି;
      ଫେରନ୍ତୁ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  ପାଇଁ(ଚାବି = 0; ଚାବି < start_pos; ++ଚାବି) {
    ଯଦି(!tbl->rows[ଚାବି]) {
      tbl->rows[ଚାବି] = entry;
      tbl->nentries++;
      tbl->last_ଚାବି_added = ଚାବି;
      *pkey = ଚାବି;
      ଫେରନ୍ତୁ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ Curl_uint32_tbl_remove(ସଂରଚନା uint32_tbl *tbl, uint32_t ଚାବି)
{
  ଯଦି(ଚାବି < tbl->nrows)
    uint32_tbl_clear_rows(tbl, ଚାବି, ଚାବି + 1);
}

bool Curl_uint32_tbl_contains(ସଂରଚନା uint32_tbl *tbl, uint32_t ଚାବି)
{
  ଫେରନ୍ତୁ (ଚାବି < tbl->nrows) ? !!tbl->rows[ଚାବି] : FALSE;
}

ସ୍ଥିର bool uint32_tbl_next_at(ସଂରଚନା uint32_tbl *tbl, uint32_t ଚାବି,
                               uint32_t *pkey, ଶୂନ୍ୟ **pentry)
{
  ପାଇଁ(; ଚାବି < tbl->nrows; ++ଚାବି) {
    ଯଦି(tbl->rows[ଚାବି]) {
      *pkey = ଚାବି;
      *pentry = tbl->rows[ଚାବି];
      ଫେରନ୍ତୁ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ଖାଲି;
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_uint32_tbl_first(ସଂରଚନା uint32_tbl *tbl,
                           uint32_t *pkey, ଶୂନ୍ୟ **pentry)
{
  ଯଦି(!pkey || !pentry)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    ଫେରନ୍ତୁ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ଖାଲି;
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_uint32_tbl_next(ସଂରଚନା uint32_tbl *tbl, uint32_t last_ଚାବି,
                          uint32_t *pkey, ଶୂନ୍ୟ **pentry)
{
  ଯଦି(!pkey || !pentry)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(uint32_tbl_next_at(tbl, last_ଚାବି + 1, pkey, pentry))
    ଫେରନ୍ତୁ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ଖାଲି;
  ଫେରନ୍ତୁ FALSE;
}
