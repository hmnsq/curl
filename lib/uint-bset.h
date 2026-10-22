#اگر_تعریف_نشده سرآیند_CURL_UINT_BSET_H
#تعریف_کن سرآیند_CURL_UINT_BSET_H
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

/* A bitset for unsigned int values.
 * It can hold the numbers from 0 - (nmax - 1),
 * rounded to the next 64 multiple.
 *
 * Optimized for high efficiency in adding/removing numbers.
 * Efficient storage when the set is (often) relatively full.
 *
 * If the set's cardinality is only expected to be a fraction of nmax,
 * uint_spbset offers a "sparse" variant with more memory efficiency at
 * the price of slightly slower operations.
 */

بنية uint32_bset {
  uint64_t *slots;
  uint32_t nslots;
  uint32_t اولی_جایگاه_استفاده_شده;
#اگر_تعریف_شده DEBUGBUILD
  صحيح init;
#پایان_اگر
};

/* Initialize the bitset with capacity 0. */
فراغ Curl_uint32_bset_init(بنية uint32_bset *bset);

/* Resize the bitset capacity to hold numbers from 0 to `nmax`,
 * which rounds up `nmax` to the next multiple of 64. */
CURLcode Curl_uint32_bset_resize(بنية uint32_bset *bset, uint32_t nmax);

/* Destroy the bitset, freeing all resources. */
فراغ Curl_uint32_bset_destroy(بنية uint32_bset *bset);

/* Get the cardinality of the bitset, e.g. numbers present in the set. */
uint32_t Curl_uint32_bset_شمار(بنية uint32_bset *bset);

/* TRUE of bitset is empty */
bool Curl_uint32_bset_خالی(بنية uint32_bset *bset);

/* Clear the bitset, making it empty. */
فراغ Curl_uint32_bset_clear(بنية uint32_bset *bset);

/* Add the number `i` to the bitset. Return FALSE if the number is
 * outside the set's capacity.
 * Numbers can be added more than once, without making a difference. */
bool Curl_uint32_bset_جمع_کن(بنية uint32_bset *bset, uint32_t i);

/* Remove the number `i` from the bitset. */
فراغ Curl_uint32_bset_remove(بنية uint32_bset *bset, uint32_t i);

/* Return TRUE if the bitset contains number `i`. */
bool Curl_uint32_bset_شامل_است(بنية uint32_bset *bset, uint32_t i);

/* Get the first number in the bitset, e.g. the smallest.
 * Returns FALSE when the bitset is empty. */
bool Curl_uint32_bset_اولی(بنية uint32_bset *bset, uint32_t *pfirst);

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
bool Curl_uint32_bset_بعدی(بنية uint32_bset *bset, uint32_t آخرین,
                           uint32_t *pnext);

#اگر_تعریف_نشده CURL_POPCOUNT64
#تعریف_کن CURL_POPCOUNT64(x)  Curl_popcount64(x)
#تعریف_کن CURL_POPCOUNT64_IMPLEMENT
uint32_t Curl_popcount64(uint64_t x);
#پایان_اگر /* !CURL_POPCOUNT64 */

#اگر_تعریف_نشده CURL_CTZ64
#تعریف_کن CURL_CTZ64(x)  Curl_ctz64(x)
#تعریف_کن CURL_CTZ64_IMPLEMENT
uint32_t Curl_ctz64(uint64_t x);
#پایان_اگر /* !CURL_CTZ64 */

#پایان_اگر /* HEADER_CURL_UINT_BSET_H */
