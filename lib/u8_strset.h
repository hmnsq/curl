#جے_بنا_تعریف سرخی_CURL_U8_STRSET_H
#تعریف_کرو سرخی_CURL_U8_STRSET_H
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
#تعریف_کرو CURL_U8_STRSET_شروع_بٹ   3
#تعریف_کرو CURL_U8_STRSET_شروع_DIM    (1U << CURL_U8_STRSET_شروع_بٹ)

/* A set that can hold up to 256 strings identified by an `id'.
 * Setting a string for an existing id replaces the previous one.
 * Getting the string for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
بنية u8_strset {
  حرف **انکڑے; /* #slots array of null-terminated strings */
  uint8_t *ids; /* #slots array of `id` values */
  uint8_t *psl; /* #slots array of "probe sequence length" values */
  حرف *sdata[CURL_U8_STRSET_شروع_DIM];
  uint8_t sids[CURL_U8_STRSET_شروع_DIM];
  uint8_t spsl[CURL_U8_STRSET_شروع_DIM];
  uint16_t گنتی;
  uint8_t slotbits;
  uint8_t slotmask;
#جے_تعریف_شدہ DEBUGBUILD
  int32_t init;
#جے_انت
};

فراغ Curl_u8_strset_init(بنية u8_strset *سیٹ);
فراغ Curl_u8_strset_clear(بنية u8_strset *سیٹ);

#جے_تعریف_شدہ UNITTESTS
uint16_t Curl_u8_strset_گنتی(بنية u8_strset *سیٹ);
#جے_انت

ثابت حرف *Curl_u8_strset_get(بنية u8_strset *سیٹ, uint8_t id);

/* Set string for id, makes a copy. */
CURLcode Curl_u8_strset_setx(بنية u8_strset *سیٹ,
                             uint8_t id, ثابت حرف *str, size_t slen);
/* Set string for id, takes ownership of `str` even on failure. */
CURLcode Curl_u8_strset_setn(بنية u8_strset *سیٹ,
                             uint8_t id, حرف *str);
فراغ Curl_u8_strset_unset(بنية u8_strset *سیٹ, uint8_t id);

/* Remove the string if in the set and zero its memory */
فراغ Curl_u8_strset_unset0(بنية u8_strset *سیٹ, uint8_t id);

CURLcode Curl_u8_strset_copy(بنية u8_strset *dest, بنية u8_strset *src);

#جے_انت /* HEADER_CURL_U8_STRSET_H */
