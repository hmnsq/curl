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

#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय CURL_UINT32_TBL_MAGIC  0x62757473
#यदि_अन्तः

शून्य Curl_uint32_tbl_init(संरचना uint32_tbl *tbl)
{
  स्मृतिं_पूरय(tbl, 0, परिमाण(*tbl));
  tbl->चरमः_कुञ्चिका_added = UINT32_MAX;
#यदि_परिभाषितम् DEBUGBUILD
  tbl->init = CURL_UINT32_TBL_MAGIC;
#यदि_अन्तः
}

स्थिर शून्य uint32_tbl_clear_पङ्क्तिकाः(संरचना uint32_tbl *tbl,
                                  uint32_t from,
                                  uint32_t upto_excluding)
{
  uint32_t i, अन्तः;

  अन्तः = CURLMIN(upto_excluding, tbl->nrows);
  प्रति(i = from; i < अन्तः; ++i) {
    यदि(tbl->पङ्क्तिकाः[i]) {
      tbl->पङ्क्तिकाः[i] = रिक्त;
      tbl->nentries--;
    }
  }
}

CURLcode Curl_uint32_tbl_resize(संरचना uint32_tbl *tbl, uint32_t nrows)
{
  /* we use `tbl->nrows + 1` during iteration, want that to work */
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  यदि(!nrows)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  यदि(nrows != tbl->nrows) {
    शून्य **पङ्क्तिकाः = curlx_calloc(nrows, परिमाण(शून्य *));
    यदि(!पङ्क्तिकाः)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(tbl->पङ्क्तिकाः) {
      स्मृतिं_प्रतिलिख(पङ्क्तिकाः, tbl->पङ्क्तिकाः, (CURLMIN(nrows, tbl->nrows) * परिमाण(शून्य *)));
      यदि(nrows < tbl->nrows)
        uint32_tbl_clear_पङ्क्तिकाः(tbl, nrows, tbl->nrows);
      curlx_free(tbl->पङ्क्तिकाः);
    }
    tbl->पङ्क्तिकाः = पङ्क्तिकाः;
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
  uint32_tbl_clear_पङ्क्तिकाः(tbl, 0, tbl->nrows);
  DEBUGASSERT(!tbl->nentries);
  tbl->चरमः_कुञ्चिका_added = UINT32_MAX;
}

शून्य Curl_uint32_tbl_destroy(संरचना uint32_tbl *tbl)
{
  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  uint32_tbl_clear(tbl);
  curlx_free(tbl->पङ्क्तिकाः);
  स्मृतिं_पूरय(tbl, 0, परिमाण(*tbl));
}

uint32_t Curl_uint32_tbl_क्षमता(संरचना uint32_tbl *tbl)
{
  प्रत्यागम tbl->nrows;
}

uint32_t Curl_uint32_tbl_गणनम्(संरचना uint32_tbl *tbl)
{
  प्रत्यागम tbl->nentries;
}

शून्य *Curl_uint32_tbl_get(संरचना uint32_tbl *tbl, uint32_t कुञ्चिका)
{
  प्रत्यागम (कुञ्चिका < tbl->nrows) ? tbl->पङ्क्तिकाः[कुञ्चिका] : रिक्त;
}

bool Curl_uint32_tbl_सङ्कलय(संरचना uint32_tbl *tbl, शून्य *प्रविष्टिः, uint32_t *pkey)
{
  uint32_t कुञ्चिका, start_pos;

  DEBUGASSERT(tbl->init == CURL_UINT32_TBL_MAGIC);
  यदि(!प्रविष्टिः || !pkey)
    प्रत्यागम FALSE;
  *pkey = UINT32_MAX;
  यदि(tbl->nentries == tbl->nrows)  /*पूर्णम्*/
    प्रत्यागम FALSE;

  start_pos = CURLMIN(tbl->चरमः_कुञ्चिका_added, tbl->nrows) + 1;
  प्रति(कुञ्चिका = start_pos; कुञ्चिका < tbl->nrows; ++कुञ्चिका) {
    यदि(!tbl->पङ्क्तिकाः[कुञ्चिका]) {
      tbl->पङ्क्तिकाः[कुञ्चिका] = प्रविष्टिः;
      tbl->nentries++;
      tbl->चरमः_कुञ्चिका_added = कुञ्चिका;
      *pkey = कुञ्चिका;
      प्रत्यागम TRUE;
    }
  }
  /* no free entry at or above tbl->maybe_next_key, wrap around */
  प्रति(कुञ्चिका = 0; कुञ्चिका < start_pos; ++कुञ्चिका) {
    यदि(!tbl->पङ्क्तिकाः[कुञ्चिका]) {
      tbl->पङ्क्तिकाः[कुञ्चिका] = प्रविष्टिः;
      tbl->nentries++;
      tbl->चरमः_कुञ्चिका_added = कुञ्चिका;
      *pkey = कुञ्चिका;
      प्रत्यागम TRUE;
    }
  }
  /* Did not find any free row? Should not happen */
  DEBUGASSERT(0);
  प्रत्यागम FALSE;
}

शून्य Curl_uint32_tbl_remove(संरचना uint32_tbl *tbl, uint32_t कुञ्चिका)
{
  यदि(कुञ्चिका < tbl->nrows)
    uint32_tbl_clear_पङ्क्तिकाः(tbl, कुञ्चिका, कुञ्चिका + 1);
}

bool Curl_uint32_tbl_अन्तर्भवति(संरचना uint32_tbl *tbl, uint32_t कुञ्चिका)
{
  प्रत्यागम (कुञ्चिका < tbl->nrows) ? !!tbl->पङ्क्तिकाः[कुञ्चिका] : FALSE;
}

स्थिर bool uint32_tbl_अग्र_at(संरचना uint32_tbl *tbl, uint32_t कुञ्चिका,
                               uint32_t *pkey, शून्य **pentry)
{
  प्रति(; कुञ्चिका < tbl->nrows; ++कुञ्चिका) {
    यदि(tbl->पङ्क्तिकाः[कुञ्चिका]) {
      *pkey = कुञ्चिका;
      *pentry = tbl->पङ्क्तिकाः[कुञ्चिका];
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
  यदि(tbl->nentries && uint32_tbl_अग्र_at(tbl, 0, pkey, pentry))
    प्रत्यागम TRUE;
  DEBUGASSERT(!tbl->nentries);
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  प्रत्यागम FALSE;
}

bool Curl_uint32_tbl_अग्र(संरचना uint32_tbl *tbl, uint32_t चरमः_कुञ्चिका,
                          uint32_t *pkey, शून्य **pentry)
{
  यदि(!pkey || !pentry)
    प्रत्यागम FALSE;
  यदि(uint32_tbl_अग्र_at(tbl, चरमः_कुञ्चिका + 1, pkey, pentry))
    प्रत्यागम TRUE;
  *pkey = UINT32_MAX;  /* always invalid */
  *pentry = रिक्त;
  प्रत्यागम FALSE;
}
