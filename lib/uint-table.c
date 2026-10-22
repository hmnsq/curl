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

#اگر_تعریف_شده DEBUGBUILD
#تعریف_کن CURL_UINT32_TBL_MAGIC  0x62757473
#پایان_اگر

فراغ Curl_uint32_tbl_init(بنية uint32_tbl *tbl)
{
  memset(tbl, 0, حجم(*tbl));
  tbl->آخرین_کلید_added = UINT32_MAX;
#اگر_تعریف_شده DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#پایان_اگر
}

ساكن فراغ uint32_tbl_clear_ردیفها(بنية uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, انتها;

  انتها = CURLMIN(upto_excluding, tbl->nrows);
  لكل(i = from; i < انتها; ++i) {
    إذا(tbl->ردیفها[i]) {
      tbl->ردیفها[i] = عدم;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(بنية uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!nrows)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  إذا(nrows != tbl->nrows) {
    فراغ **ردیفها = curlx_calloc(nrows, حجم(فراغ *));
    إذا(!ردیفها)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(tbl->ردیفها) {
      کپی_حافظه(ردیفها, tbl->ردیفها, (CURLMIN(nrows, tbl->nrows) * حجم(فراغ *)));
      إذا(nrows < tbl->nrows)
        uint32_tbl_clear_ردیفها(tbl, nrows, tbl->nrows);
      curlx_free(tbl->ردیفها);
    }
    tbl->ردیفها = ردیفها;
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
  uint32_tbl_clear_ردیفها(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->آخرین_کلید_added = UINT32_MAX;
}

فراغ Curl_uint32_tbl_destroy(بنية uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->ردیفها);
  memset(tbl, 0, حجم(*tbl));
}

uint32_t Curl_uint32_tbl_ظرفیت(بنية uint32_tbl *tbl)
{
  أرجع tbl->nrows;
}

uint32_t Curl_uint32_tbl_شمار(بنية uint32_tbl *tbl)
{
  أرجع tbl->nentries;
}

فراغ *Curl_uint32_tbl_get(بنية uint32_tbl *tbl, uint32_t کلید)
{
  أرجع (کلید < tbl->nrows) ? tbl->ردیفها[کلید] : عدم;
}

bool Curl_uint32_tbl_جمع_کن(بنية uint32_tbl *tbl, فراغ *مدخل, uint32_t *pkey)
{
  uint32_t کلید, شروع_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!مدخل || !pkey)
    أرجع FALSE;
  *pkey = UINT32_MAX;
  إذا(tbl->nentries == tbl->nrows)  /*پر*/
    أرجع FALSE;

  شروع_pos = CURLMIN(tbl->آخرین_کلید_added, tbl->nrows) + 1;
  لكل(کلید = شروع_pos; کلید < tbl->nrows; ++کلید) {
    إذا(!tbl->ردیفها[کلید]) {
      tbl->ردیفها[کلید] = مدخل;
      tbl->nentries++;
      tbl->آخرین_کلید_added = کلید;
      *pkey = کلید;
      أرجع TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  لكل(کلید = 0; کلید < شروع_pos; ++کلید) {
    إذا(!tbl->ردیفها[کلید]) {
      tbl->ردیفها[کلید] = مدخل;
      tbl->nentries++;
      tbl->آخرین_کلید_added = کلید;
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
    uint32_tbl_clear_ردیفها(tbl, کلید, کلید + 1);
}

bool Curl_uint32_tbl_شامل_است(بنية uint32_tbl *tbl, uint32_t کلید)
{
  أرجع (کلید < tbl->nrows) ? !!tbl->ردیفها[کلید] : FALSE;
}

ساكن bool uint32_tbl_بعدی_at(بنية uint32_tbl *tbl, uint32_t کلید,
                               uint32_t *pkey, فراغ **pentry)
{
  لكل(; کلید < tbl->nrows; ++کلید) {
    إذا(tbl->ردیفها[کلید]) {
      *pkey = کلید;
      *pentry = tbl->ردیفها[کلید];
      أرجع TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_اولی(بنية uint32_tbl *tbl,
                           uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(tbl->nentries && uint32_tbl_بعدی_at(tbl, 0, pkey, pentry))
    أرجع TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_بعدی(بنية uint32_tbl *tbl, uint32_t آخرین_کلید,
                          uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(uint32_tbl_بعدی_at(tbl, آخرین_کلید + 1, pkey, pentry))
    أرجع TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}
