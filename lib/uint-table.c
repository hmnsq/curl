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
#شامل "curl_setup.h"

#شامل "uint-table.h"

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_UINT32_TBL_MAGIC  0x62757473
#اگر_ختم

خالی Curl_uint32_tbl_init(ساخت uint32_tbl *tbl)
{
  memset(tbl, 0, حجم(*tbl));
  tbl->آخری_کلید_added = UINT32_MAX;
#اگر_تعریف_شدہ DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#اگر_ختم
}

ساکن خالی uint32_tbl_clear_صفیں(ساخت uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, انتہا;

  انتہا = CURLMIN(upto_excluding, tbl->nrows);
  برائے(i = from; i < انتہا; ++i) {
    اگر(tbl->صفیں[i]) {
      tbl->صفیں[i] = تہی;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(ساخت uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  اگر(!nrows)
    واپس CURLE_BAD_FUNCTION_دلیل;
  اگر(nrows != tbl->nrows) {
    خالی **صفیں = curlx_calloc(nrows, حجم(خالی *));
    اگر(!صفیں)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(tbl->صفیں) {
      حافظہ_نقل(صفیں, tbl->صفیں, (CURLMIN(nrows, tbl->nrows) * حجم(خالی *)));
      اگر(nrows < tbl->nrows)
        uint32_tbl_clear_صفیں(tbl, nrows, tbl->nrows);
      curlx_free(tbl->صفیں);
    }
    tbl->صفیں = صفیں;
    tbl->nrows = nrows;
  }
  واپس CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST خالی uint32_tbl_clear(ساخت uint32_tbl *tbl);
UNITTEST خالی uint32_tbl_clear(ساخت uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_صفیں(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->آخری_کلید_added = UINT32_MAX;
}

خالی Curl_uint32_tbl_destroy(ساخت uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->صفیں);
  memset(tbl, 0, حجم(*tbl));
}

uint32_t Curl_uint32_tbl_استعداد(ساخت uint32_tbl *tbl)
{
  واپس tbl->nrows;
}

uint32_t Curl_uint32_tbl_گنتی(ساخت uint32_tbl *tbl)
{
  واپس tbl->nentries;
}

خالی *Curl_uint32_tbl_get(ساخت uint32_tbl *tbl, uint32_t کلید)
{
  واپس (کلید < tbl->nrows) ? tbl->صفیں[کلید] : تہی;
}

bool Curl_uint32_tbl_جوڑو(ساخت uint32_tbl *tbl, خالی *اندراج_2, uint32_t *pkey)
{
  uint32_t کلید, آغاز_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  اگر(!اندراج_2 || !pkey)
    واپس FALSE;
  *pkey = UINT32_MAX;
  اگر(tbl->nentries == tbl->nrows)  /*بھرا*/
    واپس FALSE;

  آغاز_pos = CURLMIN(tbl->آخری_کلید_added, tbl->nrows) + 1;
  برائے(کلید = آغاز_pos; کلید < tbl->nrows; ++کلید) {
    اگر(!tbl->صفیں[کلید]) {
      tbl->صفیں[کلید] = اندراج_2;
      tbl->nentries++;
      tbl->آخری_کلید_added = کلید;
      *pkey = کلید;
      واپس TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  برائے(کلید = 0; کلید < آغاز_pos; ++کلید) {
    اگر(!tbl->صفیں[کلید]) {
      tbl->صفیں[کلید] = اندراج_2;
      tbl->nentries++;
      tbl->آخری_کلید_added = کلید;
      *pkey = کلید;
      واپس TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  واپس FALSE;
}

خالی Curl_uint32_tbl_remove(ساخت uint32_tbl *tbl, uint32_t کلید)
{
  اگر(کلید < tbl->nrows)
    uint32_tbl_clear_صفیں(tbl, کلید, کلید + 1);
}

bool Curl_uint32_tbl_شامل_ہے(ساخت uint32_tbl *tbl, uint32_t کلید)
{
  واپس (کلید < tbl->nrows) ? !!tbl->صفیں[کلید] : FALSE;
}

ساکن bool uint32_tbl_اگلا_at(ساخت uint32_tbl *tbl, uint32_t کلید,
                               uint32_t *pkey, خالی **pentry)
{
  برائے(; کلید < tbl->nrows; ++کلید) {
    اگر(tbl->صفیں[کلید]) {
      *pkey = کلید;
      *pentry = tbl->صفیں[کلید];
      واپس TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = تہی;
  واپس FALSE;
}

bool Curl_uint32_tbl_پہلا(ساخت uint32_tbl *tbl,
                           uint32_t *pkey, خالی **pentry)
{
  اگر(!pkey || !pentry)
    واپس FALSE;
  اگر(tbl->nentries && uint32_tbl_اگلا_at(tbl, 0, pkey, pentry))
    واپس TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = تہی;
  واپس FALSE;
}

bool Curl_uint32_tbl_اگلا(ساخت uint32_tbl *tbl, uint32_t آخری_کلید,
                          uint32_t *pkey, خالی **pentry)
{
  اگر(!pkey || !pentry)
    واپس FALSE;
  اگر(uint32_tbl_اگلا_at(tbl, آخری_کلید + 1, pkey, pentry))
    واپس TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = تہی;
  واپس FALSE;
}
