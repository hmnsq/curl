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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "uint-table.h"

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు CURL_UINT32_TBL_MAGIC  0x62757473
#అయితే_ముగింపు

శూన్య Curl_uint32_tbl_init(నిర్మాణం uint32_tbl *tbl)
{
  memset(tbl, 0, పరిమాణం(*tbl));
  tbl->చివరి_కీలకం_added = UINT32_MAX;
#నిర్వచితమైతే DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#అయితే_ముగింపు
}

స్థిర శూన్య uint32_tbl_clear_అడ్డువరుసలు(నిర్మాణం uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, చివర;

  చివర = CURLMIN(upto_excluding, tbl->nrows);
  కొరకు(i = from; i < చివర; ++i) {
    ఒకవేళ(tbl->అడ్డువరుసలు[i]) {
      tbl->అడ్డువరుసలు[i] = ఖాళీ;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(నిర్మాణం uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ఒకవేళ(!nrows)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఒకవేళ(nrows != tbl->nrows) {
    శూన్య **అడ్డువరుసలు = curlx_calloc(nrows, పరిమాణం(శూన్య *));
    ఒకవేళ(!అడ్డువరుసలు)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(tbl->అడ్డువరుసలు) {
      జ్ఞాపకం_నకలు(అడ్డువరుసలు, tbl->అడ్డువరుసలు, (CURLMIN(nrows, tbl->nrows) * పరిమాణం(శూన్య *)));
      ఒకవేళ(nrows < tbl->nrows)
        uint32_tbl_clear_అడ్డువరుసలు(tbl, nrows, tbl->nrows);
      curlx_free(tbl->అడ్డువరుసలు);
    }
    tbl->అడ్డువరుసలు = అడ్డువరుసలు;
    tbl->nrows = nrows;
  }
  తిరిగి CURLE_OK;
}

/* Clear the table, making it empty.

   @unittest 3212
 */
UNITTEST శూన్య uint32_tbl_clear(నిర్మాణం uint32_tbl *tbl);
UNITTEST శూన్య uint32_tbl_clear(నిర్మాణం uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear_అడ్డువరుసలు(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->చివరి_కీలకం_added = UINT32_MAX;
}

శూన్య Curl_uint32_tbl_destroy(నిర్మాణం uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->అడ్డువరుసలు);
  memset(tbl, 0, పరిమాణం(*tbl));
}

uint32_t Curl_uint32_tbl_సామర్థ్యం(నిర్మాణం uint32_tbl *tbl)
{
  తిరిగి tbl->nrows;
}

uint32_t Curl_uint32_tbl_లెక్క(నిర్మాణం uint32_tbl *tbl)
{
  తిరిగి tbl->nentries;
}

శూన్య *Curl_uint32_tbl_get(నిర్మాణం uint32_tbl *tbl, uint32_t కీలకం)
{
  తిరిగి (కీలకం < tbl->nrows) ? tbl->అడ్డువరుసలు[కీలకం] : ఖాళీ;
}

bool Curl_uint32_tbl_కూడు(నిర్మాణం uint32_tbl *tbl, శూన్య *నమోదు_2, uint32_t *pkey)
{
  uint32_t కీలకం, ప్రారంభం_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  ఒకవేళ(!నమోదు_2 || !pkey)
    తిరిగి FALSE;
  *pkey = UINT32_MAX;
  ఒకవేళ(tbl->nentries == tbl->nrows)  /*నిండిన*/
    తిరిగి FALSE;

  ప్రారంభం_pos = CURLMIN(tbl->చివరి_కీలకం_added, tbl->nrows) + 1;
  కొరకు(కీలకం = ప్రారంభం_pos; కీలకం < tbl->nrows; ++కీలకం) {
    ఒకవేళ(!tbl->అడ్డువరుసలు[కీలకం]) {
      tbl->అడ్డువరుసలు[కీలకం] = నమోదు_2;
      tbl->nentries++;
      tbl->చివరి_కీలకం_added = కీలకం;
      *pkey = కీలకం;
      తిరిగి TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  కొరకు(కీలకం = 0; కీలకం < ప్రారంభం_pos; ++కీలకం) {
    ఒకవేళ(!tbl->అడ్డువరుసలు[కీలకం]) {
      tbl->అడ్డువరుసలు[కీలకం] = నమోదు_2;
      tbl->nentries++;
      tbl->చివరి_కీలకం_added = కీలకం;
      *pkey = కీలకం;
      తిరిగి TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  తిరిగి FALSE;
}

శూన్య Curl_uint32_tbl_remove(నిర్మాణం uint32_tbl *tbl, uint32_t కీలకం)
{
  ఒకవేళ(కీలకం < tbl->nrows)
    uint32_tbl_clear_అడ్డువరుసలు(tbl, కీలకం, కీలకం + 1);
}

bool Curl_uint32_tbl_కలిగియుంది(నిర్మాణం uint32_tbl *tbl, uint32_t కీలకం)
{
  తిరిగి (కీలకం < tbl->nrows) ? !!tbl->అడ్డువరుసలు[కీలకం] : FALSE;
}

స్థిర bool uint32_tbl_తదుపరి_at(నిర్మాణం uint32_tbl *tbl, uint32_t కీలకం,
                               uint32_t *pkey, శూన్య **pentry)
{
  కొరకు(; కీలకం < tbl->nrows; ++కీలకం) {
    ఒకవేళ(tbl->అడ్డువరుసలు[కీలకం]) {
      *pkey = కీలకం;
      *pentry = tbl->అడ్డువరుసలు[కీలకం];
      తిరిగి TRUE;
    }
  }
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ఖాళీ;
  తిరిగి FALSE;
}

bool Curl_uint32_tbl_మొదటి(నిర్మాణం uint32_tbl *tbl,
                           uint32_t *pkey, శూన్య **pentry)
{
  ఒకవేళ(!pkey || !pentry)
    తిరిగి FALSE;
  ఒకవేళ(tbl->nentries && uint32_tbl_తదుపరి_at(tbl, 0, pkey, pentry))
    తిరిగి TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ఖాళీ;
  తిరిగి FALSE;
}

bool Curl_uint32_tbl_తదుపరి(నిర్మాణం uint32_tbl *tbl, uint32_t చివరి_కీలకం,
                          uint32_t *pkey, శూన్య **pentry)
{
  ఒకవేళ(!pkey || !pentry)
    తిరిగి FALSE;
  ఒకవేళ(uint32_tbl_తదుపరి_at(tbl, చివరి_కీలకం + 1, pkey, pentry))
    తిరిగి TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = ఖాళీ;
  తిరిగి FALSE;
}
