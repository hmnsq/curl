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

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو CURL_UINT32_TBL_MAGIC  0x62757473
#جيڪڏهن_پورو

فراغ Curl_uint32_tbl_init(بنية uint32_tbl *tbl)
{
  memset(tbl, 0, حجم(*tbl));
  tbl->آخري_ڪنجي_added = UINT32_MAX;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#جيڪڏهن_پورو
}

ساكن فراغ uint32_tbl_clear_صفون(بنية uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, پڇاڙي;

  پڇاڙي = CURLMIN(upto_excluding, tbl->nrows);
  لكل(i = from; i < پڇاڙي; ++i) {
    إذا(tbl->صفون[i]) {
      tbl->صفون[i] = عدم;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(بنية uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!nrows)
    أرجع CURLE_BAD_FUNCTION_دليل;
  إذا(nrows != tbl->nrows) {
    فراغ **صفون = curlx_calloc(nrows, حجم(فراغ *));
    إذا(!صفون)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    إذا(tbl->صفون) {
      يادگيري_نقل(صفون, tbl->صفون, (CURLMIN(nrows, tbl->nrows) * حجم(فراغ *)));
      إذا(nrows < tbl->nrows)
        uint32_tbl_clear_صفون(tbl, nrows, tbl->nrows);
      curlx_free(tbl->صفون);
    }
    tbl->صفون = صفون;
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
  uint32_tbl_clear_صفون(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->آخري_ڪنجي_added = UINT32_MAX;
}

فراغ Curl_uint32_tbl_destroy(بنية uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->صفون);
  memset(tbl, 0, حجم(*tbl));
}

uint32_t Curl_uint32_tbl_صلاحيت(بنية uint32_tbl *tbl)
{
  أرجع tbl->nrows;
}

uint32_t Curl_uint32_tbl_ڳڻپ(بنية uint32_tbl *tbl)
{
  أرجع tbl->nentries;
}

فراغ *Curl_uint32_tbl_get(بنية uint32_tbl *tbl, uint32_t ڪنجي)
{
  أرجع (ڪنجي < tbl->nrows) ? tbl->صفون[ڪنجي] : عدم;
}

bool Curl_uint32_tbl_جوڙيو(بنية uint32_tbl *tbl, فراغ *داخلا_2, uint32_t *pkey)
{
  uint32_t ڪنجي, شروعات_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  إذا(!داخلا_2 || !pkey)
    أرجع FALSE;
  *pkey = UINT32_MAX;
  إذا(tbl->nentries == tbl->nrows)  /*ڀريل*/
    أرجع FALSE;

  شروعات_pos = CURLMIN(tbl->آخري_ڪنجي_added, tbl->nrows) + 1;
  لكل(ڪنجي = شروعات_pos; ڪنجي < tbl->nrows; ++ڪنجي) {
    إذا(!tbl->صفون[ڪنجي]) {
      tbl->صفون[ڪنجي] = داخلا_2;
      tbl->nentries++;
      tbl->آخري_ڪنجي_added = ڪنجي;
      *pkey = ڪنجي;
      أرجع TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  لكل(ڪنجي = 0; ڪنجي < شروعات_pos; ++ڪنجي) {
    إذا(!tbl->صفون[ڪنجي]) {
      tbl->صفون[ڪنجي] = داخلا_2;
      tbl->nentries++;
      tbl->آخري_ڪنجي_added = ڪنجي;
      *pkey = ڪنجي;
      أرجع TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  أرجع FALSE;
}

فراغ Curl_uint32_tbl_remove(بنية uint32_tbl *tbl, uint32_t ڪنجي)
{
  إذا(ڪنجي < tbl->nrows)
    uint32_tbl_clear_صفون(tbl, ڪنجي, ڪنجي + 1);
}

bool Curl_uint32_tbl_شامل_آهي(بنية uint32_tbl *tbl, uint32_t ڪنجي)
{
  أرجع (ڪنجي < tbl->nrows) ? !!tbl->صفون[ڪنجي] : FALSE;
}

ساكن bool uint32_tbl_ايندڙ_at(بنية uint32_tbl *tbl, uint32_t ڪنجي,
                               uint32_t *pkey, فراغ **pentry)
{
  لكل(; ڪنجي < tbl->nrows; ++ڪنجي) {
    إذا(tbl->صفون[ڪنجي]) {
      *pkey = ڪنجي;
      *pentry = tbl->صفون[ڪنجي];
      أرجع TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_پهريون(بنية uint32_tbl *tbl,
                           uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(tbl->nentries && uint32_tbl_ايندڙ_at(tbl, 0, pkey, pentry))
    أرجع TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}

bool Curl_uint32_tbl_ايندڙ(بنية uint32_tbl *tbl, uint32_t آخري_ڪنجي,
                          uint32_t *pkey, فراغ **pentry)
{
  إذا(!pkey || !pentry)
    أرجع FALSE;
  إذا(uint32_tbl_ايندڙ_at(tbl, آخري_ڪنجي + 1, pkey, pentry))
    أرجع TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = عدم;
  أرجع FALSE;
}
