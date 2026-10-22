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
#ضمن "curl_setup.h"

#ضمن "uint-table.h"

#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_UINT32_TBL_MAGIC  0x62757473
#جے_انت

فراغ Curl_uint32_tbl_init(بنية uint32_tbl *tbl)
{
  memset(tbl, 0, حجم(*tbl));
  tbl->آخری_کلید_added = UINT32_MAX;
#جے_تعریف_شدہ DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#جے_انت
}

ساكن فراغ uint32_tbl_clear_صفاں(بنية uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, سرا;

  سرا = CURLMIN(upto_excluding, tbl->nrows);
  لكل(i = from; i < سرا; ++i) {
    إذا(tbl->صفاں[i]) {
      tbl->صفاں[i] = عدم;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(بنية uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!nrows)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  إذا(nrows != tbl->nrows) {
    فراغ **صفاں = curlx_calloc(nrows, حجم(فراغ *));
    إذا(!صفاں)
      أرجع CURLE_باہرلا_دا_حافظہ;
    إذا(tbl->صفاں) {
      حافظہ_نقل(صفاں, tbl->صفاں, (CURLMIN(nrows, tbl->nrows) * حجم(فراغ *)));
      إذا(nrows < tbl->nrows)
        uint32_tbl_clear_صفاں(tbl, nrows, tbl->nrows);
      curlx_free(tbl->صفاں);
    }
    tbl->صفاں = صفاں;
    tbl->nrows = nrows;
  }
  أرجع CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST فراغ uint32_tbl_clear(بنية uint32_tbl *tbl);
UNITTEST فراغ uint32_tbl_clear(بنية uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_صفاں(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->آخری_کلید_added = UINT32_MAX;
}

فراغ Curl_uint32_tbl_destroy(بنية uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->صفاں);
  memset(tbl, 0, حجم(*tbl));
}

uint32_t Curl_uint32_tbl_سمرتھا(بنية uint32_tbl *tbl)
{
  أرجع tbl->nrows;
}

uint32_t Curl_uint32_tbl_گنتی(بنية uint32_tbl *tbl)
{
  أرجع tbl->nentries;
}

فراغ *Curl_uint32_tbl_get(بنية uint32_tbl *tbl, uint32_t کلید)
{
  أرجع (کلید < tbl->nrows) ? tbl->صفاں[کلید] : عدم;
}

bool Curl_uint32_tbl_جوڑو(بنية uint32_tbl *tbl, فراغ *اندراج, uint32_t *pkey)
{
  uint32_t کلید, شروع_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!اندراج || !pkey)
    أرجع FALSE;
  *pkey = UINT32_MAX;
  إذا(tbl->nentries == tbl->nrows)  /*بھریا*/
    أرجع FALSE;

  شروع_pos = CURLMIN(tbl->آخری_کلید_added, tbl->nrows) + 1;
  لكل(کلید = شروع_pos; کلید < tbl->nrows; ++کلید) {
    إذا(!tbl->صفاں[کلید]) {
      tbl->صفاں[کلید] = اندراج;
      tbl->nentries++;
      tbl->آخری_کلید_added = کلید;
      *pkey = کلید;
      أرجع TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  لكل(کلید = 0; کلید < شروع_pos; ++کلید) {
    إذا(!tbl->صفاں[کلید]) {
      tbl->صفاں[کلید] = اندراج;
      tbl->nentries++;
      tbl->آخری_کلید_added = کلید;
      *pkey = کلید;
      أرجع TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  أرجع FALSE;
}

فراغ Curl_uint32_tbl_remove(بنية uint32_tbl *tbl, uint32_t کلید)
{
  إذا(کلید < tbl->nrows)
    uint32_tbl_clear_صفاں(tbl, کلید, کلید + 1);
}

bool Curl_uint32_tbl_شامل_اے(بنية uint32_tbl *tbl, uint32_t کلید)
{
  أرجع (کلید < tbl->nrows) ? !!tbl->صفاں[کلید] : FALSE;
}

ساكن bool uint32_tbl_اگلا_at(بنية uint32_tbl *tbl, uint32_t کلید,
                               uint32_t *pkey, فراغ **pentry)
{
  لكل(; کلید < tbl->nrows; ++کلید) {
    إذا(tbl->صفاں[کلید]) {
      *pkey = کلید;
      *pentry = tbl->صفاں[کلید];
      أرجع TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_پہلا(بنية uint32_tbl *tbl,
                           uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(tbl->nentries && uint32_tbl_اگلا_at(tbl, 0, pkey, pentry))
    أرجع TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_اگلا(بنية uint32_tbl *tbl, uint32_t آخری_کلید,
                          uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(uint32_tbl_اگلا_at(tbl, آخری_کلید + 1, pkey, pentry))
    أرجع TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}
