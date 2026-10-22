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

#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه CURL_UINT32_TBL_MAGIC  0x62757473
#د_که_پای

فراغ Curl_uint32_tbl_init(بنية uint32_tbl *tbl)
{
  memset(tbl, 0, حجم(*tbl));
  tbl->وروستی_کلی_added = UINT32_MAX;
#که_تعریف_شوی DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#د_که_پای
}

ساكن فراغ uint32_tbl_clear_لیکې(بنية uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, پای_ټکی;

  پای_ټکی = CURLMIN(upto_excluding, tbl->nrows);
  لكل(i = from; i < پای_ټکی; ++i) {
    إذا(tbl->لیکې[i]) {
      tbl->لیکې[i] = عدم;
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
    فراغ **لیکې = curlx_calloc(nrows, حجم(فراغ *));
    إذا(!لیکې)
      أرجع CURLE_بهر_د_حافظه;
    إذا(tbl->لیکې) {
      حافظه_نقل(لیکې, tbl->لیکې, (CURLMIN(nrows, tbl->nrows) * حجم(فراغ *)));
      إذا(nrows < tbl->nrows)
        uint32_tbl_clear_لیکې(tbl, nrows, tbl->nrows);
      curlx_free(tbl->لیکې);
    }
    tbl->لیکې = لیکې;
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
  uint32_tbl_clear_لیکې(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->وروستی_کلی_added = UINT32_MAX;
}

فراغ Curl_uint32_tbl_destroy(بنية uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->لیکې);
  memset(tbl, 0, حجم(*tbl));
}

uint32_t Curl_uint32_tbl_ظرفیت(بنية uint32_tbl *tbl)
{
  أرجع tbl->nrows;
}

uint32_t Curl_uint32_tbl_شمېر(بنية uint32_tbl *tbl)
{
  أرجع tbl->nentries;
}

فراغ *Curl_uint32_tbl_get(بنية uint32_tbl *tbl, uint32_t کلی)
{
  أرجع (کلی < tbl->nrows) ? tbl->لیکې[کلی] : عدم;
}

bool Curl_uint32_tbl_جمع_کړه(بنية uint32_tbl *tbl, فراغ *ثبت, uint32_t *pkey)
{
  uint32_t کلی, پیل_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!ثبت || !pkey)
    أرجع FALSE;
  *pkey = UINT32_MAX;
  إذا(tbl->nentries == tbl->nrows)  /*ډک*/
    أرجع FALSE;

  پیل_pos = CURLMIN(tbl->وروستی_کلی_added, tbl->nrows) + 1;
  لكل(کلی = پیل_pos; کلی < tbl->nrows; ++کلی) {
    إذا(!tbl->لیکې[کلی]) {
      tbl->لیکې[کلی] = ثبت;
      tbl->nentries++;
      tbl->وروستی_کلی_added = کلی;
      *pkey = کلی;
      أرجع TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  لكل(کلی = 0; کلی < پیل_pos; ++کلی) {
    إذا(!tbl->لیکې[کلی]) {
      tbl->لیکې[کلی] = ثبت;
      tbl->nentries++;
      tbl->وروستی_کلی_added = کلی;
      *pkey = کلی;
      أرجع TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  أرجع FALSE;
}

فراغ Curl_uint32_tbl_remove(بنية uint32_tbl *tbl, uint32_t کلی)
{
  إذا(کلی < tbl->nrows)
    uint32_tbl_clear_لیکې(tbl, کلی, کلی + 1);
}

bool Curl_uint32_tbl_لري(بنية uint32_tbl *tbl, uint32_t کلی)
{
  أرجع (کلی < tbl->nrows) ? !!tbl->لیکې[کلی] : FALSE;
}

ساكن bool uint32_tbl_بل_at(بنية uint32_tbl *tbl, uint32_t کلی,
                               uint32_t *pkey, فراغ **pentry)
{
  لكل(; کلی < tbl->nrows; ++کلی) {
    إذا(tbl->لیکې[کلی]) {
      *pkey = کلی;
      *pentry = tbl->لیکې[کلی];
      أرجع TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_لومړی(بنية uint32_tbl *tbl,
                           uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(tbl->nentries && uint32_tbl_بل_at(tbl, 0, pkey, pentry))
    أرجع TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_بل(بنية uint32_tbl *tbl, uint32_t وروستی_کلی,
                          uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(uint32_tbl_بل_at(tbl, وروستی_کلی + 1, pkey, pentry))
    أرجع TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}
