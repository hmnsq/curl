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
  សម្រាប់(i = from; i < end; ++i) {
    ប្រសិនបើ(tbl->rows[i]) {
      tbl->rows[i] = NULL;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(sa_nracanaa uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ប្រសិនបើ(!nrows)
    ត្រឡប់ CURLE_BAD_FUNCTION_ARGUMENT;
  ប្រសិនបើ(nrows != tbl->nrows) {
    void **rows = curlx_calloc(nrows, parimaa_na(void *));
    ប្រសិនបើ(!rows)
      ត្រឡប់ CURLE_OUT_OF_MEMORY;
    ប្រសិនបើ(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * parimaa_na(void *)));
      ប្រសិនបើ(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  ត្រឡប់ CURLE_OK;
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
  ត្រឡប់ tbl->nrows;
}

uint32_t Curl_uint32_tbl_count(sa_nracanaa uint32_tbl *tbl)
{
  ត្រឡប់ tbl->nentries;
}

void *Curl_uint32_tbl_get(sa_nracanaa uint32_tbl *tbl, uint32_t key)
{
  ត្រឡប់ (key < tbl->nrows) ? tbl->rows[key] : NULL;
}

bool Curl_uint32_tbl_add(sa_nracanaa uint32_tbl *tbl, void *entry, uint32_t *pkey)
{
  uint32_t key, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ប្រសិនបើ(!entry || !pkey)
    ត្រឡប់ FALSE;
  *pkey = UINT32_MAX;
  ប្រសិនបើ(tbl->nentries == tbl->nrows)  /* full */
    ត្រឡប់ FALSE;

  start_pos = CURLMIN(tbl->last_key_added, tbl->nrows) + 1;
  សម្រាប់(key = start_pos; key < tbl->nrows; ++key) {
    ប្រសិនបើ(!tbl->rows[key]) {
      tbl->rows[key] = entry;
      tbl->nentries++;
      tbl->last_key_added = key;
      *pkey = key;
      ត្រឡប់ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  សម្រាប់(key = 0; key < start_pos; ++key) {
    ប្រសិនបើ(!tbl->rows[key]) {
      tbl->rows[key] = entry;
      tbl->nentries++;
      tbl->last_key_added = key;
      *pkey = key;
      ត្រឡប់ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  ត្រឡប់ FALSE;
}

void Curl_uint32_tbl_remove(sa_nracanaa uint32_tbl *tbl, uint32_t key)
{
  ប្រសិនបើ(key < tbl->nrows)
    uint32_tbl_clear_rows(tbl, key, key + 1);
}

bool Curl_uint32_tbl_contains(sa_nracanaa uint32_tbl *tbl, uint32_t key)
{
  ត្រឡប់ (key < tbl->nrows) ? !!tbl->rows[key] : FALSE;
}

static bool uint32_tbl_next_at(sa_nracanaa uint32_tbl *tbl, uint32_t key,
                               uint32_t *pkey, void **pentry)
{
  សម្រាប់(; key < tbl->nrows; ++key) {
    ប្រសិនបើ(tbl->rows[key]) {
      *pkey = key;
      *pentry = tbl->rows[key];
      ត្រឡប់ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = NULL;
  ត្រឡប់ FALSE;
}

bool Curl_uint32_tbl_first(sa_nracanaa uint32_tbl *tbl,
                           uint32_t *pkey, void **pentry)
{
  ប្រសិនបើ(!pkey || !pentry)
    ត្រឡប់ FALSE;
  ប្រសិនបើ(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    ត្រឡប់ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = NULL;
  ត្រឡប់ FALSE;
}

bool Curl_uint32_tbl_next(sa_nracanaa uint32_tbl *tbl, uint32_t last_key,
                          uint32_t *pkey, void **pentry)
{
  ប្រសិនបើ(!pkey || !pentry)
    ត្រឡប់ FALSE;
  ប្រសិនបើ(uint32_tbl_next_at(tbl, last_key + 1, pkey, pentry))
    ត្រឡប់ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = NULL;
  ត្រឡប់ FALSE;
}
