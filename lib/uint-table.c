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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

வெறுமை Curl_uint32_tbl_init(கட்டமைப்பு uint32_tbl *tbl)
{
  memset(tbl, 0, அளவு(*tbl));
  tbl->last_சாவி_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

நிலையான வெறுமை uint32_tbl_clear_rows(கட்டமைப்பு uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  க்கு(i = from; i < end; ++i) {
    எனில்(tbl->rows[i]) {
      tbl->rows[i] = வெற்று;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(கட்டமைப்பு uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  எனில்(!nrows)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  எனில்(nrows != tbl->nrows) {
    வெறுமை **rows = curlx_calloc(nrows, அளவு(வெறுமை *));
    எனில்(!rows)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * அளவு(வெறுமை *)));
      எனில்(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  திரும்பு CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST வெறுமை uint32_tbl_clear(கட்டமைப்பு uint32_tbl *tbl);
UNITTEST வெறுமை uint32_tbl_clear(கட்டமைப்பு uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_சாவி_added = UINT32_MAX;
}

வெறுமை Curl_uint32_tbl_destroy(கட்டமைப்பு uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, அளவு(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(கட்டமைப்பு uint32_tbl *tbl)
{
  திரும்பு tbl->nrows;
}

uint32_t Curl_uint32_tbl_மொத்தம்(கட்டமைப்பு uint32_tbl *tbl)
{
  திரும்பு tbl->nentries;
}

வெறுமை *Curl_uint32_tbl_get(கட்டமைப்பு uint32_tbl *tbl, uint32_t சாவி)
{
  திரும்பு (சாவி < tbl->nrows) ? tbl->rows[சாவி] : வெற்று;
}

bool Curl_uint32_tbl_add(கட்டமைப்பு uint32_tbl *tbl, வெறுமை *entry, uint32_t *pkey)
{
  uint32_t சாவி, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  எனில்(!entry || !pkey)
    திரும்பு FALSE;
  *pkey = UINT32_MAX;
  எனில்(tbl->nentries == tbl->nrows)  /* full */
    திரும்பு FALSE;

  start_pos = CURLMIN(tbl->last_சாவி_added, tbl->nrows) + 1;
  க்கு(சாவி = start_pos; சாவி < tbl->nrows; ++சாவி) {
    எனில்(!tbl->rows[சாவி]) {
      tbl->rows[சாவி] = entry;
      tbl->nentries++;
      tbl->last_சாவி_added = சாவி;
      *pkey = சாவி;
      திரும்பு TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  க்கு(சாவி = 0; சாவி < start_pos; ++சாவி) {
    எனில்(!tbl->rows[சாவி]) {
      tbl->rows[சாவி] = entry;
      tbl->nentries++;
      tbl->last_சாவி_added = சாவி;
      *pkey = சாவி;
      திரும்பு TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  திரும்பு FALSE;
}

வெறுமை Curl_uint32_tbl_remove(கட்டமைப்பு uint32_tbl *tbl, uint32_t சாவி)
{
  எனில்(சாவி < tbl->nrows)
    uint32_tbl_clear_rows(tbl, சாவி, சாவி + 1);
}

bool Curl_uint32_tbl_contains(கட்டமைப்பு uint32_tbl *tbl, uint32_t சாவி)
{
  திரும்பு (சாவி < tbl->nrows) ? !!tbl->rows[சாவி] : FALSE;
}

நிலையான bool uint32_tbl_next_at(கட்டமைப்பு uint32_tbl *tbl, uint32_t சாவி,
                               uint32_t *pkey, வெறுமை **pentry)
{
  க்கு(; சாவி < tbl->nrows; ++சாவி) {
    எனில்(tbl->rows[சாவி]) {
      *pkey = சாவி;
      *pentry = tbl->rows[சாவி];
      திரும்பு TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = வெற்று;
  திரும்பு FALSE;
}

bool Curl_uint32_tbl_first(கட்டமைப்பு uint32_tbl *tbl,
                           uint32_t *pkey, வெறுமை **pentry)
{
  எனில்(!pkey || !pentry)
    திரும்பு FALSE;
  எனில்(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    திரும்பு TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = வெற்று;
  திரும்பு FALSE;
}

bool Curl_uint32_tbl_next(கட்டமைப்பு uint32_tbl *tbl, uint32_t last_சாவி,
                          uint32_t *pkey, வெறுமை **pentry)
{
  எனில்(!pkey || !pentry)
    திரும்பு FALSE;
  எனில்(uint32_tbl_next_at(tbl, last_சாவி + 1, pkey, pentry))
    திரும்பு TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = வெற்று;
  திரும்பு FALSE;
}
