#ifndef HEADER_CURL_U8_STRSET_H
#define HEADER_CURL_U8_STRSET_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

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
#define CURL_U8_STRSET_START_BITS   3
#define CURL_U8_STRSET_START_DIM    (1U << CURL_U8_STRSET_START_BITS)

/* A set that can hold up to 256 strings identified by an `id'.
 * Setting a string for an existing id replaces the previous one.
 * Getting the string for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
ಸಂರಚನಾ u8_strset {
  ವರ್ಣ **data; /* #slots array of null-terminated strings */
  uint8_t *ids; /* #slots array of `id` values */
  uint8_t *psl; /* #slots array of "probe sequence length" values */
  ವರ್ಣ *sdata[CURL_U8_STRSET_START_DIM];
  uint8_t sids[CURL_U8_STRSET_START_DIM];
  uint8_t spsl[CURL_U8_STRSET_START_DIM];
  uint16_t ಎಣಿಕೆ;
  uint8_t slotbits;
  uint8_t slotmask;
#ifdef DEBUGBUILD
  int32_t init;
#endif
};

ಶೂನ್ಯ Curl_u8_strset_init(ಸಂರಚನಾ u8_strset *set);
ಶೂನ್ಯ Curl_u8_strset_clear(ಸಂರಚನಾ u8_strset *set);

#ifdef UNITTESTS
uint16_t Curl_u8_strset_ಎಣಿಕೆ(ಸಂರಚನಾ u8_strset *set);
#endif

ಧ್ರುವ ವರ್ಣ *Curl_u8_strset_get(ಸಂರಚನಾ u8_strset *set, uint8_t id);

/* Set string for id, makes a copy. */
CURLcode Curl_u8_strset_setx(ಸಂರಚನಾ u8_strset *set,
                             uint8_t id, ಧ್ರುವ ವರ್ಣ *str, size_t slen);
/* Set string for id, takes ownership of `str` even on failure. */
CURLcode Curl_u8_strset_setn(ಸಂರಚನಾ u8_strset *set,
                             uint8_t id, ವರ್ಣ *str);
ಶೂನ್ಯ Curl_u8_strset_unset(ಸಂರಚನಾ u8_strset *set, uint8_t id);

/* Remove the string if in the set and zero its memory */
ಶೂನ್ಯ Curl_u8_strset_unset0(ಸಂರಚನಾ u8_strset *set, uint8_t id);

CURLcode Curl_u8_strset_copy(ಸಂರಚನಾ u8_strset *dest, ಸಂರಚನಾ u8_strset *src);

#endif /* HEADER_CURL_U8_STRSET_H */
