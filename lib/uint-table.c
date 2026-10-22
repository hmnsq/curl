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
  स_रखो(tbl, 0, आकार(*tbl));
  tbl->last_किल्ली_added = UINT32_MAX;
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
  साठी(i = from; i < end; ++i) {
    जर(tbl->rows[i]) {
      tbl->rows[i] = रिक्त;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(संरचना uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  जर(!nrows)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  जर(nrows != tbl->nrows) {
    शून्य **rows = curlx_calloc(nrows, आकार(शून्य *));
    जर(!rows)
      परत CURLE_OUT_OF_MEMORY;
    जर(tbl->rows) {
      स_नकल(rows, tbl->rows, (CURLMIN(nrows, tbl->nrows) * आकार(शून्य *)));
      जर(nrows < tbl->nrows)
        uint32_tbl_clear_rows(tbl, nrows, tbl->nrows);
      curlx_free(tbl->rows);
    }
    tbl->rows = rows;
    tbl->nrows = nrows;
  }
  परत CURLE_OK;
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
  tbl->last_किल्ली_added = UINT32_MAX;
}

शून्य Curl_uint32_tbl_destroy(संरचना uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->rows);
  स_रखो(tbl, 0, आकार(*tbl));
}

uint32_t Curl_uint32_tbl_capacity(संरचना uint32_tbl *tbl)
{
  परत tbl->nrows;
}

uint32_t Curl_uint32_tbl_मोजणी(संरचना uint32_tbl *tbl)
{
  परत tbl->nentries;
}

शून्य *Curl_uint32_tbl_get(संरचना uint32_tbl *tbl, uint32_t किल्ली)
{
  परत (किल्ली < tbl->nrows) ? tbl->rows[किल्ली] : रिक्त;
}

bool Curl_uint32_tbl_add(संरचना uint32_tbl *tbl, शून्य *entry, uint32_t *pkey)
{
  uint32_t किल्ली, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  जर(!entry || !pkey)
    परत FALSE;
  *pkey = UINT32_MAX;
  जर(tbl->nentries == tbl->nrows)  /* full */
    परत FALSE;

  start_pos = CURLMIN(tbl->last_किल्ली_added, tbl->nrows) + 1;
  साठी(किल्ली = start_pos; किल्ली < tbl->nrows; ++किल्ली) {
    जर(!tbl->rows[किल्ली]) {
      tbl->rows[किल्ली] = entry;
      tbl->nentries++;
      tbl->last_किल्ली_added = किल्ली;
      *pkey = किल्ली;
      परत TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  साठी(किल्ली = 0; किल्ली < start_pos; ++किल्ली) {
    जर(!tbl->rows[किल्ली]) {
      tbl->rows[किल्ली] = entry;
      tbl->nentries++;
      tbl->last_किल्ली_added = किल्ली;
      *pkey = किल्ली;
      परत TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  परत FALSE;
}

शून्य Curl_uint32_tbl_remove(संरचना uint32_tbl *tbl, uint32_t किल्ली)
{
  जर(किल्ली < tbl->nrows)
    uint32_tbl_clear_rows(tbl, किल्ली, किल्ली + 1);
}

bool Curl_uint32_tbl_contains(संरचना uint32_tbl *tbl, uint32_t किल्ली)
{
  परत (किल्ली < tbl->nrows) ? !!tbl->rows[किल्ली] : FALSE;
}

स्थिर bool uint32_tbl_next_at(संरचना uint32_tbl *tbl, uint32_t किल्ली,
                               uint32_t *pkey, शून्य **pentry)
{
  साठी(; किल्ली < tbl->nrows; ++किल्ली) {
    जर(tbl->rows[किल्ली]) {
      *pkey = किल्ली;
      *pentry = tbl->rows[किल्ली];
      परत TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  परत FALSE;
}

bool Curl_uint32_tbl_first(संरचना uint32_tbl *tbl,
                           uint32_t *pkey, शून्य **pentry)
{
  जर(!pkey || !pentry)
    परत FALSE;
  जर(tbl->nentries && uint32_tbl_next_at(tbl, 0, pkey, pentry))
    परत TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  परत FALSE;
}

bool Curl_uint32_tbl_next(संरचना uint32_tbl *tbl, uint32_t last_किल्ली,
                          uint32_t *pkey, शून्य **pentry)
{
  जर(!pkey || !pentry)
    परत FALSE;
  जर(uint32_tbl_next_at(tbl, last_किल्ली + 1, pkey, pentry))
    परत TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  परत FALSE;
}
