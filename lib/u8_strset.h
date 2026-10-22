#నిర్వచితం_కాకపోతే శీర్షిక_CURL_U8_STRSET_H
#నిర్వచించు శీర్షిక_CURL_U8_STRSET_H
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

/* How large should the initial set be?
 * Measuring our test suite with set growth force fail, gives
 * BITS RESULT
 * 1    1261 tests out of 1951 reported OK: 64%
 * 2    1792 tests out of 1951 reported OK: 91%
 * 3    1944 tests out of 1951 reported OK: 99%
 * 4    1949 tests out of 1951 reported OK: 99%
 * 5    single fail of 3211, unit test for u8_strset
 * meaning 91% of our tests to not set more than 4 strings and
 * 99% do not set more than 8.
 */
#నిర్వచించు CURL_U8_STRSET_ప్రారంభం_బిట్లు   3
#నిర్వచించు CURL_U8_STRSET_ప్రారంభం_DIM    (1U << CURL_U8_STRSET_ప్రారంభం_బిట్లు)

/* A set that can hold up to 256 strings identified by an `id'.
 * Setting a string for an existing id replaces the previous one.
 * Getting the string for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
నిర్మాణం u8_strset {
  అక్షరం **దత్తాంశం; /* #slots array of null-terminated strings */
  uint8_t *ids; /* #slots array of `id` values */
  uint8_t *psl; /* #slots array of "probe sequence length" values */
  అక్షరం *sdata[CURL_U8_STRSET_ప్రారంభం_DIM];
  uint8_t sids[CURL_U8_STRSET_ప్రారంభం_DIM];
  uint8_t spsl[CURL_U8_STRSET_ప్రారంభం_DIM];
  uint16_t లెక్క;
  uint8_t slotbits;
  uint8_t slotmask;
#నిర్వచితమైతే DEBUGBUILD
  int32_t init;
#అయితే_ముగింపు
};

శూన్య Curl_u8_strset_init(నిర్మాణం u8_strset *సమితి);
శూన్య Curl_u8_strset_clear(నిర్మాణం u8_strset *సమితి);

#నిర్వచితమైతే UNITTESTS
uint16_t Curl_u8_strset_లెక్క(నిర్మాణం u8_strset *సమితి);
#అయితే_ముగింపు

స్థిరరాశి అక్షరం *Curl_u8_strset_get(నిర్మాణం u8_strset *సమితి, uint8_t id);

/* Set string for id, makes a copy. */
CURLcode Curl_u8_strset_setx(నిర్మాణం u8_strset *సమితి,
                             uint8_t id, స్థిరరాశి అక్షరం *str, size_t slen);
/* Set string for id, takes ownership of `str` even on failure. */
CURLcode Curl_u8_strset_setn(నిర్మాణం u8_strset *సమితి,
                             uint8_t id, అక్షరం *str);
శూన్య Curl_u8_strset_unset(నిర్మాణం u8_strset *సమితి, uint8_t id);

/* Remove the string if in the set and zero its memory */
శూన్య Curl_u8_strset_unset0(నిర్మాణం u8_strset *సమితి, uint8_t id);

CURLcode Curl_u8_strset_copy(నిర్మాణం u8_strset *dest, నిర్మాణం u8_strset *src);

#అయితే_ముగింపు /* HEADER_CURL_U8_STRSET_H */
