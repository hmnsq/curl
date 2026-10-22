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
#include "curl_setup.h"

#include "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

void Curl_uint32_tbl_init(sa_nracanaa uint32_tbl *tbl)
{
  memset(tbl, 0, parimaa_na(*tbl));
  tbl->last_key_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

static void uint32_tbl_clear_rows(sa_nracanaa uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  ꦏꦁꦒꦺꦴ(i = from; i < end; ++i) {
    ꦪꦼꦤ꧀(tbl->rows[i]) {
      tbl->rows[i] = NULL;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(sa_nracanaa uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ꦪꦼꦤ꧀(!nrows)
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;
  ꦪꦼꦤ꧀(nrows != tbl->nrows) {
    void **rows = curlx_calloc(nrows, parimaa_na(void *));
    ꦪꦼꦤ꧀(!rows)
      ꦮꦭꦶ CURLE_OUT_OF_MEMORY;
    ꦪꦼꦤ꧀(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * parimaa_na(void *)));
      ꦪꦼꦤ꧀(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  ꦮꦭꦶ CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST void uint32_tbl_clear(sa_nracanaa uint32_tbl *tbl);
UNITTEST void uint32_tbl_clear(sa_nracanaa uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_key_added = UINT32_MAX;
}

void Curl_uint32_tbl_destroy(sa_nracanaa uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, parimaa_na(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(sa_nracanaa uint32_tbl *tbl)
{
  ꦮꦭꦶ tbl->nrows;
}

uint32_t Curl_uint32_tbl_count(sa_nracanaa uint32_tbl *tbl)
{
  ꦮꦭꦶ tbl->nentries;
}

void *Curl_uint32_tbl_get(sa_nracanaa uint32_tbl *tbl, uint32_t key)
{
  ꦮꦭꦶ (key < tbl->nrows) ? tbl->rows[key] : NULL;
}

bool Curl_uint32_tbl_add(sa_nracanaa uint32_tbl *tbl, void *entry, uint32_t *pkey)
{
  uint32_t key, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ꦪꦼꦤ꧀(!entry || !pkey)
    ꦮꦭꦶ FALSE;
  *pkey = UINT32_MAX;
  ꦪꦼꦤ꧀(tbl->nentries == tbl->nrows)  /* full */
    ꦮꦭꦶ FALSE;

  start_pos = CURLMIN(tbl->last_key_added, tbl->nrows) + 1;
  ꦏꦁꦒꦺꦴ(key = start_pos; key < tbl->nrows; ++key) {
    ꦪꦼꦤ꧀(!tbl->rows[key]) {
      tbl->rows[key] = entry;
      tbl->nentries++;
      tbl->last_key_added = key;
      *pkey = key;
      ꦮꦭꦶ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  ꦏꦁꦒꦺꦴ(key = 0; key < start_pos; ++key) {
    ꦪꦼꦤ꧀(!tbl->rows[key]) {
      tbl->rows[key] = entry;
      tbl->nentries++;
      tbl->last_key_added = key;
      *pkey = key;
      ꦮꦭꦶ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  ꦮꦭꦶ FALSE;
}

void Curl_uint32_tbl_remove(sa_nracanaa uint32_tbl *tbl, uint32_t key)
{
  ꦪꦼꦤ꧀(key < tbl->nrows)
    uint32_tbl_clear_rows(tbl, key, key + 1);
}

bool Curl_uint32_tbl_contains(sa_nracanaa uint32_tbl *tbl, uint32_t key)
{
  ꦮꦭꦶ (key < tbl->nrows) ? !!tbl->rows[key] : FALSE;
}

static bool uint32_tbl_next_at(sa_nracanaa uint32_tbl *tbl, uint32_t key,
                               uint32_t *pkey, void **pentry)
{
  ꦏꦁꦒꦺꦴ(; key < tbl->nrows; ++key) {
    ꦪꦼꦤ꧀(tbl->rows[key]) {
      *pkey = key;
      *pentry = tbl->rows[key];
      ꦮꦭꦶ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = NULL;
  ꦮꦭꦶ FALSE;
}

bool Curl_uint32_tbl_first(sa_nracanaa uint32_tbl *tbl,
                           uint32_t *pkey, void **pentry)
{
  ꦪꦼꦤ꧀(!pkey || !pentry)
    ꦮꦭꦶ FALSE;
  ꦪꦼꦤ꧀(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    ꦮꦭꦶ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = NULL;
  ꦮꦭꦶ FALSE;
}

bool Curl_uint32_tbl_next(sa_nracanaa uint32_tbl *tbl, uint32_t last_key,
                          uint32_t *pkey, void **pentry)
{
  ꦪꦼꦤ꧀(!pkey || !pentry)
    ꦮꦭꦶ FALSE;
  ꦪꦼꦤ꧀(uint32_tbl_next_at(tbl, last_key + 1, pkey, pentry))
    ꦮꦭꦶ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = NULL;
  ꦮꦭꦶ FALSE;
}
