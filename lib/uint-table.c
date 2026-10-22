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
  स_रखो(tbl, 0, परिमाण(*tbl));
  tbl->last_साँचो_added = UINT32_MAX;
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
  लागि(i = from; i < end; ++i) {
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
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  यदि(nrows != tbl->nrows) {
    शून्य **rows = curlx_calloc(nrows, परिमाण(शून्य *));
    यदि(!rows)
      फर्काऊ CURLE_OUT_OF_MEMORY;
    यदि(tbl->rows) {
      स_नकल(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * परिमाण(शून्य *)));
      यदि(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  फर्काऊ CURLE_OK;
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
  tbl->last_साँचो_added = UINT32_MAX;
}

शून्य Curl_uint32_tbl_destroy(संरचना uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  स_रखो(tbl, 0, परिमाण(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(संरचना uint32_tbl *tbl)
{
  फर्काऊ tbl->nrows;
}

uint32_t Curl_uint32_tbl_गन्ती(संरचना uint32_tbl *tbl)
{
  फर्काऊ tbl->nentries;
}

शून्य *Curl_uint32_tbl_get(संरचना uint32_tbl *tbl, uint32_t साँचो)
{
  फर्काऊ (साँचो < tbl->nrows) ? tbl->rows[साँचो] : रिक्त;
}

bool Curl_uint32_tbl_add(संरचना uint32_tbl *tbl, शून्य *entry, uint32_t *pkey)
{
  uint32_t साँचो, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  यदि(!entry || !pkey)
    फर्काऊ FALSE;
  *pkey = UINT32_MAX;
  यदि(tbl->nentries == tbl->nrows)  /* full */
    फर्काऊ FALSE;

  start_pos = CURLMIN(tbl->last_साँचो_added, tbl->nrows) + 1;
  लागि(साँचो = start_pos; साँचो < tbl->nrows; ++साँचो) {
    यदि(!tbl->rows[साँचो]) {
      tbl->rows[साँचो] = entry;
      tbl->nentries++;
      tbl->last_साँचो_added = साँचो;
      *pkey = साँचो;
      फर्काऊ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  लागि(साँचो = 0; साँचो < start_pos; ++साँचो) {
    यदि(!tbl->rows[साँचो]) {
      tbl->rows[साँचो] = entry;
      tbl->nentries++;
      tbl->last_साँचो_added = साँचो;
      *pkey = साँचो;
      फर्काऊ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  फर्काऊ FALSE;
}

शून्य Curl_uint32_tbl_remove(संरचना uint32_tbl *tbl, uint32_t साँचो)
{
  यदि(साँचो < tbl->nrows)
    uint32_tbl_clear_rows(tbl, साँचो, साँचो + 1);
}

bool Curl_uint32_tbl_contains(संरचना uint32_tbl *tbl, uint32_t साँचो)
{
  फर्काऊ (साँचो < tbl->nrows) ? !!tbl->rows[साँचो] : FALSE;
}

स्थिर bool uint32_tbl_next_at(संरचना uint32_tbl *tbl, uint32_t साँचो,
                               uint32_t *pkey, शून्य **pentry)
{
  लागि(; साँचो < tbl->nrows; ++साँचो) {
    यदि(tbl->rows[साँचो]) {
      *pkey = साँचो;
      *pentry = tbl->rows[साँचो];
      फर्काऊ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  फर्काऊ FALSE;
}

bool Curl_uint32_tbl_first(संरचना uint32_tbl *tbl,
                           uint32_t *pkey, शून्य **pentry)
{
  यदि(!pkey || !pentry)
    फर्काऊ FALSE;
  यदि(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    फर्काऊ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  फर्काऊ FALSE;
}

bool Curl_uint32_tbl_next(संरचना uint32_tbl *tbl, uint32_t last_साँचो,
                          uint32_t *pkey, शून्य **pentry)
{
  यदि(!pkey || !pentry)
    फर्काऊ FALSE;
  यदि(uint32_tbl_next_at(tbl, last_साँचो + 1, pkey, pentry))
    फर्काऊ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  फर्काऊ FALSE;
}
