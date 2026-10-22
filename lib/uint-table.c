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
#समावेश "curl_setup.h"

#समावेश "uint-table.h"

#अगर_घोषित DEBUGBUILD
#घोषणा CURL_UINT32_TBL_MAGIC  0x62757473
#पूर्ण_अगर

व्योम Curl_uint32_tbl_init(काष्ठा uint32_tbl *tbl)
{
  स_रखो(tbl, 0, माप(*tbl));
  tbl->last_कुंजी_added = UINT32_MAX;
#अगर_घोषित DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#पूर्ण_अगर
}

अटल व्योम uint32_tbl_clear_rows(काष्ठा uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  क्रम(i = from; i < end; ++i) {
    अगर(tbl->rows[i]) {
      tbl->rows[i] = शून्य;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(काष्ठा uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  अगर(!nrows)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  अगर(nrows != tbl->nrows) {
    व्योम **rows = curlx_calloc(nrows, माप(व्योम *));
    अगर(!rows)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(tbl->rows) {
      स_नकल(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * माप(व्योम *)));
      अगर(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  वापस CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST व्योम uint32_tbl_clear(काष्ठा uint32_tbl *tbl);
UNITTEST व्योम uint32_tbl_clear(काष्ठा uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_कुंजी_added = UINT32_MAX;
}

व्योम Curl_uint32_tbl_destroy(काष्ठा uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  स_रखो(tbl, 0, माप(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(काष्ठा uint32_tbl *tbl)
{
  वापस tbl->nrows;
}

uint32_t Curl_uint32_tbl_गिनती(काष्ठा uint32_tbl *tbl)
{
  वापस tbl->nentries;
}

व्योम *Curl_uint32_tbl_get(काष्ठा uint32_tbl *tbl, uint32_t कुंजी)
{
  वापस (कुंजी < tbl->nrows) ? tbl->rows[कुंजी] : शून्य;
}

bool Curl_uint32_tbl_add(काष्ठा uint32_tbl *tbl, व्योम *entry, uint32_t *pkey)
{
  uint32_t कुंजी, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  अगर(!entry || !pkey)
    वापस FALSE;
  *pkey = UINT32_MAX;
  अगर(tbl->nentries == tbl->nrows)  /* full */
    वापस FALSE;

  start_pos = CURLMIN(tbl->last_कुंजी_added, tbl->nrows) + 1;
  क्रम(कुंजी = start_pos; कुंजी < tbl->nrows; ++कुंजी) {
    अगर(!tbl->rows[कुंजी]) {
      tbl->rows[कुंजी] = entry;
      tbl->nentries++;
      tbl->last_कुंजी_added = कुंजी;
      *pkey = कुंजी;
      वापस TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  क्रम(कुंजी = 0; कुंजी < start_pos; ++कुंजी) {
    अगर(!tbl->rows[कुंजी]) {
      tbl->rows[कुंजी] = entry;
      tbl->nentries++;
      tbl->last_कुंजी_added = कुंजी;
      *pkey = कुंजी;
      वापस TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  वापस FALSE;
}

व्योम Curl_uint32_tbl_remove(काष्ठा uint32_tbl *tbl, uint32_t कुंजी)
{
  अगर(कुंजी < tbl->nrows)
    uint32_tbl_clear_rows(tbl, कुंजी, कुंजी + 1);
}

bool Curl_uint32_tbl_contains(काष्ठा uint32_tbl *tbl, uint32_t कुंजी)
{
  वापस (कुंजी < tbl->nrows) ? !!tbl->rows[कुंजी] : FALSE;
}

अटल bool uint32_tbl_अग्र_at(काष्ठा uint32_tbl *tbl, uint32_t कुंजी,
                               uint32_t *pkey, व्योम **pentry)
{
  क्रम(; कुंजी < tbl->nrows; ++कुंजी) {
    अगर(tbl->rows[कुंजी]) {
      *pkey = कुंजी;
      *pentry = tbl->rows[कुंजी];
      वापस TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = शून्य;
  वापस FALSE;
}

bool Curl_uint32_tbl_first(काष्ठा uint32_tbl *tbl,
                           uint32_t *pkey, व्योम **pentry)
{
  अगर(!pkey || !pentry)
    वापस FALSE;
  अगर(tbl->nentries && uint32_tbl_अग्र_at(tbl, 0, pkey, pentry))
    वापस TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = शून्य;
  वापस FALSE;
}

bool Curl_uint32_tbl_अग्र(काष्ठा uint32_tbl *tbl, uint32_t last_कुंजी,
                          uint32_t *pkey, व्योम **pentry)
{
  अगर(!pkey || !pentry)
    वापस FALSE;
  अगर(uint32_tbl_अग्र_at(tbl, last_कुंजी + 1, pkey, pentry))
    वापस TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = शून्य;
  वापस FALSE;
}
