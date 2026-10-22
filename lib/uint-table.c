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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

શૂન્ય Curl_uint32_tbl_init(સંરચના uint32_tbl *tbl)
{
  memset(tbl, 0, કદ(*tbl));
  tbl->last_ચાવી_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

સ્થિર શૂન્ય uint32_tbl_clear_rows(સંરચના uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  માટે(i = from; i < end; ++i) {
    જો(tbl->rows[i]) {
      tbl->rows[i] = ખાલી;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(સંરચના uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  જો(!nrows)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  જો(nrows != tbl->nrows) {
    શૂન્ય **rows = curlx_calloc(nrows, કદ(શૂન્ય *));
    જો(!rows)
      પાછા CURLE_OUT_OF_MEMORY;
    જો(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * કદ(શૂન્ય *)));
      જો(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  પાછા CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST શૂન્ય uint32_tbl_clear(સંરચના uint32_tbl *tbl);
UNITTEST શૂન્ય uint32_tbl_clear(સંરચના uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_ચાવી_added = UINT32_MAX;
}

શૂન્ય Curl_uint32_tbl_destroy(સંરચના uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, કદ(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(સંરચના uint32_tbl *tbl)
{
  પાછા tbl->nrows;
}

uint32_t Curl_uint32_tbl_ગણતરી(સંરચના uint32_tbl *tbl)
{
  પાછા tbl->nentries;
}

શૂન્ય *Curl_uint32_tbl_get(સંરચના uint32_tbl *tbl, uint32_t ચાવી)
{
  પાછા (ચાવી < tbl->nrows) ? tbl->rows[ચાવી] : ખાલી;
}

bool Curl_uint32_tbl_add(સંરચના uint32_tbl *tbl, શૂન્ય *entry, uint32_t *pkey)
{
  uint32_t ચાવી, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  જો(!entry || !pkey)
    પાછા FALSE;
  *pkey = UINT32_MAX;
  જો(tbl->nentries == tbl->nrows)  /* full */
    પાછા FALSE;

  start_pos = CURLMIN(tbl->last_ચાવી_added, tbl->nrows) + 1;
  માટે(ચાવી = start_pos; ચાવી < tbl->nrows; ++ચાવી) {
    જો(!tbl->rows[ચાવી]) {
      tbl->rows[ચાવી] = entry;
      tbl->nentries++;
      tbl->last_ચાવી_added = ચાવી;
      *pkey = ચાવી;
      પાછા TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  માટે(ચાવી = 0; ચાવી < start_pos; ++ચાવી) {
    જો(!tbl->rows[ચાવી]) {
      tbl->rows[ચાવી] = entry;
      tbl->nentries++;
      tbl->last_ચાવી_added = ચાવી;
      *pkey = ચાવી;
      પાછા TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  પાછા FALSE;
}

શૂન્ય Curl_uint32_tbl_remove(સંરચના uint32_tbl *tbl, uint32_t ચાવી)
{
  જો(ચાવી < tbl->nrows)
    uint32_tbl_clear_rows(tbl, ચાવી, ચાવી + 1);
}

bool Curl_uint32_tbl_contains(સંરચના uint32_tbl *tbl, uint32_t ચાવી)
{
  પાછા (ચાવી < tbl->nrows) ? !!tbl->rows[ચાવી] : FALSE;
}

સ્થિર bool uint32_tbl_next_at(સંરચના uint32_tbl *tbl, uint32_t ચાવી,
                               uint32_t *pkey, શૂન્ય **pentry)
{
  માટે(; ચાવી < tbl->nrows; ++ચાવી) {
    જો(tbl->rows[ચાવી]) {
      *pkey = ચાવી;
      *pentry = tbl->rows[ચાવી];
      પાછા TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ખાલી;
  પાછા FALSE;
}

bool Curl_uint32_tbl_first(સંરચના uint32_tbl *tbl,
                           uint32_t *pkey, શૂન્ય **pentry)
{
  જો(!pkey || !pentry)
    પાછા FALSE;
  જો(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    પાછા TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ખાલી;
  પાછા FALSE;
}

bool Curl_uint32_tbl_next(સંરચના uint32_tbl *tbl, uint32_t last_ચાવી,
                          uint32_t *pkey, શૂન્ય **pentry)
{
  જો(!pkey || !pentry)
    પાછા FALSE;
  જો(uint32_tbl_next_at(tbl, last_ચાવી + 1, pkey, pentry))
    પાછા TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ખાલી;
  પાછા FALSE;
}
