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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "uint-table.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_UINT32_TBL_MAGIC  0x62757473
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_uint32_tbl_init(ਬਣਤਰ uint32_tbl *tbl)
{
  memset(tbl, 0, ਆਕਾਰ(*tbl));
  tbl->ਆਖਰੀ_ਕੁੰਜੀ_added = UINT32_MAX;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ uint32_tbl_clear_ਪੰਕਤੀਆਂ(ਬਣਤਰ uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, ਸਿਰਾ;

  ਸਿਰਾ = CURLMIN(upto_excluding, tbl->nrows);
  ਲਈ(i = from; i < ਸਿਰਾ; ++i) {
    ਜੇ(tbl->ਪੰਕਤੀਆਂ[i]) {
      tbl->ਪੰਕਤੀਆਂ[i] = ਨਿਰਾ;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(ਬਣਤਰ uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ਜੇ(!nrows)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਜੇ(nrows != tbl->nrows) {
    ਖਾਲੀ **ਪੰਕਤੀਆਂ = curlx_calloc(nrows, ਆਕਾਰ(ਖਾਲੀ *));
    ਜੇ(!ਪੰਕਤੀਆਂ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(tbl->ਪੰਕਤੀਆਂ) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਪੰਕਤੀਆਂ, tbl->ਪੰਕਤੀਆਂ, (CURLMIN(nrows, tbl->nrows) * ਆਕਾਰ(ਖਾਲੀ *)));
      ਜੇ(nrows < tbl->nrows)
        uint32_tbl_clear_ਪੰਕਤੀਆਂ(tbl, nrows, tbl->nrows);
      curlx_free(tbl->ਪੰਕਤੀਆਂ);
    }
    tbl->ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
    tbl->nrows = nrows;
  }
  ਵਾਪਸ CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST ਖਾਲੀ uint32_tbl_clear(ਬਣਤਰ uint32_tbl *tbl);
UNITTEST ਖਾਲੀ uint32_tbl_clear(ਬਣਤਰ uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_ਪੰਕਤੀਆਂ(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->ਆਖਰੀ_ਕੁੰਜੀ_added = UINT32_MAX;
}

ਖਾਲੀ Curl_uint32_tbl_destroy(ਬਣਤਰ uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->ਪੰਕਤੀਆਂ);
  memset(tbl, 0, ਆਕਾਰ(*tbl));
}

uint32_t Curl_uint32_tbl_ਸਮਰੱਥਾ(ਬਣਤਰ uint32_tbl *tbl)
{
  ਵਾਪਸ tbl->nrows;
}

uint32_t Curl_uint32_tbl_ਗਿਣਤੀ(ਬਣਤਰ uint32_tbl *tbl)
{
  ਵਾਪਸ tbl->nentries;
}

ਖਾਲੀ *Curl_uint32_tbl_get(ਬਣਤਰ uint32_tbl *tbl, uint32_t ਕੁੰਜੀ)
{
  ਵਾਪਸ (ਕੁੰਜੀ < tbl->nrows) ? tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ] : ਨਿਰਾ;
}

bool Curl_uint32_tbl_ਜੋੜੋ(ਬਣਤਰ uint32_tbl *tbl, ਖਾਲੀ *ਇੰਦਰਾਜ, uint32_t *pkey)
{
  uint32_t ਕੁੰਜੀ, ਸ਼ੁਰੂ_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ਜੇ(!ਇੰਦਰਾਜ || !pkey)
    ਵਾਪਸ FALSE;
  *pkey = UINT32_MAX;
  ਜੇ(tbl->nentries == tbl->nrows)  /*ਭਰਿਆ*/
    ਵਾਪਸ FALSE;

  ਸ਼ੁਰੂ_pos = CURLMIN(tbl->ਆਖਰੀ_ਕੁੰਜੀ_added, tbl->nrows) + 1;
  ਲਈ(ਕੁੰਜੀ = ਸ਼ੁਰੂ_pos; ਕੁੰਜੀ < tbl->nrows; ++ਕੁੰਜੀ) {
    ਜੇ(!tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ]) {
      tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ] = ਇੰਦਰਾਜ;
      tbl->nentries++;
      tbl->ਆਖਰੀ_ਕੁੰਜੀ_added = ਕੁੰਜੀ;
      *pkey = ਕੁੰਜੀ;
      ਵਾਪਸ TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  ਲਈ(ਕੁੰਜੀ = 0; ਕੁੰਜੀ < ਸ਼ੁਰੂ_pos; ++ਕੁੰਜੀ) {
    ਜੇ(!tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ]) {
      tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ] = ਇੰਦਰਾਜ;
      tbl->nentries++;
      tbl->ਆਖਰੀ_ਕੁੰਜੀ_added = ਕੁੰਜੀ;
      *pkey = ਕੁੰਜੀ;
      ਵਾਪਸ TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  ਵਾਪਸ FALSE;
}

ਖਾਲੀ Curl_uint32_tbl_remove(ਬਣਤਰ uint32_tbl *tbl, uint32_t ਕੁੰਜੀ)
{
  ਜੇ(ਕੁੰਜੀ < tbl->nrows)
    uint32_tbl_clear_ਪੰਕਤੀਆਂ(tbl, ਕੁੰਜੀ, ਕੁੰਜੀ + 1);
}

bool Curl_uint32_tbl_ਸ਼ਾਮਲ_ਹੈ(ਬਣਤਰ uint32_tbl *tbl, uint32_t ਕੁੰਜੀ)
{
  ਵਾਪਸ (ਕੁੰਜੀ < tbl->nrows) ? !!tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ] : FALSE;
}

ਸਥਿਰ bool uint32_tbl_ਅਗਲਾ_at(ਬਣਤਰ uint32_tbl *tbl, uint32_t ਕੁੰਜੀ,
                               uint32_t *pkey, ਖਾਲੀ **pentry)
{
  ਲਈ(; ਕੁੰਜੀ < tbl->nrows; ++ਕੁੰਜੀ) {
    ਜੇ(tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ]) {
      *pkey = ਕੁੰਜੀ;
      *pentry = tbl->ਪੰਕਤੀਆਂ[ਕੁੰਜੀ];
      ਵਾਪਸ TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ਨਿਰਾ;
  ਵਾਪਸ FALSE;
}

bool Curl_uint32_tbl_ਪਹਿਲਾ(ਬਣਤਰ uint32_tbl *tbl,
                           uint32_t *pkey, ਖਾਲੀ **pentry)
{
  ਜੇ(!pkey || !pentry)
    ਵਾਪਸ FALSE;
  ਜੇ(tbl->nentries && uint32_tbl_ਅਗਲਾ_at(tbl, 0, pkey, pentry))
    ਵਾਪਸ TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ਨਿਰਾ;
  ਵਾਪਸ FALSE;
}

bool Curl_uint32_tbl_ਅਗਲਾ(ਬਣਤਰ uint32_tbl *tbl, uint32_t ਆਖਰੀ_ਕੁੰਜੀ,
                          uint32_t *pkey, ਖਾਲੀ **pentry)
{
  ਜੇ(!pkey || !pentry)
    ਵਾਪਸ FALSE;
  ਜੇ(uint32_tbl_ਅਗਲਾ_at(tbl, ਆਖਰੀ_ਕੁੰਜੀ + 1, pkey, pentry))
    ਵਾਪਸ TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ਨਿਰਾ;
  ਵਾਪਸ FALSE;
}
