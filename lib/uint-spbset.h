#اگر_تعریف_نشده سرآیند_CURL_UINT_SPBSET_H
#تعریف_کن سرآیند_CURL_UINT_SPBSET_H
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

/* A "sparse" bitset for uint32_t values.
 * It can hold any uint32_t value.
 *
 * Optimized for the case where only a small set of numbers need
 * to be kept, especially when "close" together. Then storage space
 * is most efficient, deteriorating when many number are far apart.
 */

/* 4 slots = 256 bits, keep this a 2^n value. */
#تعریف_کن CURL_UINT32_SPBSET_CH_SLOTS  4
#تعریف_کن CURL_UINT32_SPBSET_CH_ماسک   ((CURL_UINT32_SPBSET_CH_SLOTS * 64) - 1)

/* store the uint value from offset to
 * (offset + (CURL_UINT32_SPBSET_CHUNK_SLOTS * 64) - 1 */
بنية uint32_spbset_chunk {
  بنية uint32_spbset_chunk *بعدی;
  uint64_t slots[CURL_UINT32_SPBSET_CH_SLOTS];
  uint32_t جابه_جایی;
};

بنية uint32_spbset {
  بنية uint32_spbset_chunk سر;
#اگر_تعریف_شده DEBUGBUILD
  صحيح init;
#پایان_اگر
};

فراغ Curl_uint32_spbset_init(بنية uint32_spbset *bset);

فراغ Curl_uint32_spbset_destroy(بنية uint32_spbset *bset);

/* Get the cardinality of the bitset, e.g. numbers present in the set. */
uint32_t Curl_uint32_spbset_شمار(بنية uint32_spbset *bset);

/* Add the number `i` to the bitset.
 * Numbers can be added more than once, without making a difference.
 * Returns FALSE if allocations failed. */
bool Curl_uint32_spbset_جمع_کن(بنية uint32_spbset *bset, uint32_t i);

/* Remove the number `i` from the bitset. */
فراغ Curl_uint32_spbset_remove(بنية uint32_spbset *bset, uint32_t i);

/* Return TRUE if the bitset contains number `i`. */
bool Curl_uint32_spbset_شامل_است(بنية uint32_spbset *bset, uint32_t i);

/* Get the first number in the bitset, e.g. the smallest.
 * Returns FALSE when the bitset is empty. */
bool Curl_uint32_spbset_اولی(بنية uint32_spbset *bset, uint32_t *pfirst);

/* Get the next number in the bitset, following `last` in natural order.
 * Put another way, this is the smallest number greater than `last` in
 * the bitset. `last` does not have to be present in the set.
 *
 * Returns FALSE when no such number is in the set.
 *
 * This allows to iterate the set while being modified:
 * - added numbers higher than 'last' will be picked up by the iteration.
 * - added numbers lower than 'last' will not show up.
 * - removed numbers lower or equal to 'last' will not show up.
 * - removed numbers higher than 'last' will not be visited. */
bool Curl_uint32_spbset_بعدی(بنية uint32_spbset *bset, uint32_t آخرین,
                             uint32_t *pnext);

#پایان_اگر /* HEADER_CURL_UINT_SPBSET_H */
