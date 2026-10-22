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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "uint-table.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_TBL_MAGIC  0x62757473
#endif

शून्य Curl_uint32_tbl_init(संरचना uint32_tbl *tbl)
{
  memset(tbl, 0, परिमाण(*tbl));
  tbl->last_कुंजी_added = UINT32_MAX;
#ifdef DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#endif
}

स्थिर शून्य uint32_tbl_clear_rows(संरचना uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, end;

  end = CURLMIN(upto_excluding, tbl->nrows);
  प्रति(i = from; i < end; ++i) {
    यदि(tbl->rows[i]) {
      tbl->rows[i] = रिक्त;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(संरचना uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  यदि(!nrows)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  यदि(nrows != tbl->nrows) {
    शून्य **rows = curlx_calloc(nrows, परिमाण(शून्य *));
    यदि(!rows)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    यदि(tbl->rows) {
      memcpy(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * परिमाण(शून्य *)));
      यदि(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  प्रत्यागम CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST शून्य uint32_tbl_clear(संरचना uint32_tbl *tbl);
UNITTEST शून्य uint32_tbl_clear(संरचना uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_rows(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->last_कुंजी_added = UINT32_MAX;
}

शून्य Curl_uint32_tbl_destroy(संरचना uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  memset(tbl, 0, परिमाण(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(संरचना uint32_tbl *tbl)
{
  प्रत्यागम tbl->nrows;
}

uint32_t Curl_uint32_tbl_गिनती(संरचना uint32_tbl *tbl)
{
  प्रत्यागम tbl->nentries;
}

शून्य *Curl_uint32_tbl_get(संरचना uint32_tbl *tbl, uint32_t कुंजी)
{
  प्रत्यागम (कुंजी < tbl->nrows) ? tbl->rows[कुंजी] : रिक्त;
}

bool Curl_uint32_tbl_add(संरचना uint32_tbl *tbl, शून्य *entry, uint32_t *pkey)
{
  uint32_t कुंजी, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  यदि(!entry || !pkey)
    प्रत्यागम FALSE;
  *pkey = UINT32_MAX;
  यदि(tbl->nentries == tbl->nrows)  /* full */
    प्रत्यागम FALSE;

  start_pos = CURLMIN(tbl->last_कुंजी_added, tbl->nrows) + 1;
  प्रति(कुंजी = start_pos; कुंजी < tbl->nrows; ++कुंजी) {
    यदि(!tbl->rows[कुंजी]) {
      tbl->rows[कुंजी] = entry;
      tbl->nentries++;
      tbl->last_कुंजी_added = कुंजी;
      *pkey = कुंजी;
      प्रत्यागम TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  प्रति(कुंजी = 0; कुंजी < start_pos; ++कुंजी) {
    यदि(!tbl->rows[कुंजी]) {
      tbl->rows[कुंजी] = entry;
      tbl->nentries++;
      tbl->last_कुंजी_added = कुंजी;
      *pkey = कुंजी;
      प्रत्यागम TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  प्रत्यागम FALSE;
}

शून्य Curl_uint32_tbl_remove(संरचना uint32_tbl *tbl, uint32_t कुंजी)
{
  यदि(कुंजी < tbl->nrows)
    uint32_tbl_clear_rows(tbl, कुंजी, कुंजी + 1);
}

bool Curl_uint32_tbl_contains(संरचना uint32_tbl *tbl, uint32_t कुंजी)
{
  प्रत्यागम (कुंजी < tbl->nrows) ? !!tbl->rows[कुंजी] : FALSE;
}

स्थिर bool uint32_tbl_next_at(संरचना uint32_tbl *tbl, uint32_t कुंजी,
                               uint32_t *pkey, शून्य **pentry)
{
  प्रति(; कुंजी < tbl->nrows; ++कुंजी) {
    यदि(tbl->rows[कुंजी]) {
      *pkey = कुंजी;
      *pentry = tbl->rows[कुंजी];
      प्रत्यागम TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  प्रत्यागम FALSE;
}

bool Curl_uint32_tbl_first(संरचना uint32_tbl *tbl,
                           uint32_t *pkey, शून्य **pentry)
{
  यदि(!pkey || !pentry)
    प्रत्यागम FALSE;
  यदि(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    प्रत्यागम TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  प्रत्यागम FALSE;
}

bool Curl_uint32_tbl_next(संरचना uint32_tbl *tbl, uint32_t last_कुंजी,
                          uint32_t *pkey, शून्य **pentry)
{
  यदि(!pkey || !pentry)
    प्रत्यागम FALSE;
  यदि(uint32_tbl_next_at(tbl, last_कुंजी + 1, pkey, pentry))
    प्रत्यागम TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  प्रत्यागम FALSE;
}
