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

#ضمن "u8_strset.h"
#ضمن "curlx/strdup.h"

/* random patterns for API verification */
#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_U8_STRSET_MAGIC 0x7117e783
#جے_انت

#تعریف_کرو CURL_U8_STRSET_DEBUG      0

#تعریف_کرو CURL_وٹاندرا(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#تعریف_کرو CURL_U8_STRSET_تھاں_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#تعریف_کرو CURL_U8_تھاں_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#تعریف_کرو CURL_U8_تھاں_نقاب(i)             (uint8_t)((1U << (i)) - 1)
#تعریف_کرو CURL_U8_STRSET_تھاں_CNT(s)       CURL_U8_تھاں_CNT((s)->slotbits)

/* A hashset for tuples (id, string) using Robin Hood Hashing.
 * <https://www.cs.cornell.edu/courses/JavaAndDS/files/hashing_RobinHood.pdf>
 * The basic idea here to handle collisions by robbing "rich" entries and
 * giving to the "poor":
 * - We have an array: (id, string) are ideally placed at index "id % size".
 * - If slot at index is already occupied, we have a collision.
 * - A simple collision strategy would look at the next index, and the
 *   next until finding an empty slot.
 * - The drawback is that this may lead to many checks on lookups, as it
 *   will need to also look at subsequent slots until it finds the match.
 *   The amount of lookups is the "probe sequence length" (psl) and this
 *   may vary greatly between entries.
 * - Robin Hood Hashing balances the 'psl's of all entries more evenly:
 *   - psl == 0 means an entry is in exactly the right slot
 *   - psl == 1 means it is in the slot right after. psl == 2 is the slot
 *     after that, etc.
 *   - when inserting a new entry, track its psl. Finding a slot where
 *     the existing entry has a lower psl makes a swap. Put the new entry
 *     and its psl there, take the previous entry and its psl and find
 *     the next best slot for the previous entry. */
فراغ Curl_u8_strset_init(بنية u8_strset *سیٹ)
{
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Warray-bounds"
#جے_انت
  memset(سیٹ, 0, حجم(*سیٹ));
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#جے_انت
  سیٹ->انکڑے = سیٹ->sdata;
  سیٹ->ids = سیٹ->sids;
  سیٹ->psl = سیٹ->spsl;
  سیٹ->slotbits = CURL_U8_STRSET_شروع_بٹ;
  سیٹ->slotmask = CURL_U8_تھاں_نقاب(سیٹ->slotbits);
  سیٹ->گنتی = 0;
#جے_تعریف_شدہ DEBUGBUILD
  سیٹ->init = CURL_U8_STRSET_MAGIC;
#جے_انت
}

فراغ Curl_u8_strset_clear(بنية u8_strset *سیٹ)
{
  uint16_t i;
  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  لكل(i = 0; i < CURL_U8_STRSET_تھاں_CNT(سیٹ); ++i)
    curlx_safefree(سیٹ->انکڑے[i]);

  إذا(سیٹ->انکڑے != سیٹ->sdata)
    curlx_safefree(سیٹ->انکڑے);
  Curl_u8_strset_init(سیٹ);
}

ساكن فراغ u8_strset_addn(بنية u8_strset *سیٹ, uint8_t id, حرف *val)
{
  uint8_t i = CURL_U8_STRSET_تھاں_IDX(سیٹ, id);
  uint8_t psl = 0;
  طالما(سیٹ->انکڑے[i]) {
    إذا(psl > سیٹ->psl[i]) { /*وٹاندرا*/
      حرف *tmpdata;
      tmpdata = سیٹ->انکڑے[i];
      سیٹ->انکڑے[i] = val;
      val = tmpdata;
      CURL_وٹاندرا(سیٹ->psl[i], psl);
      CURL_وٹاندرا(سیٹ->ids[i], id);
    }
    i = CURL_U8_STRSET_تھاں_IDX(سیٹ, i + 1);
    ++psl;
  }
  سیٹ->ids[i] = id;
  سیٹ->انکڑے[i] = val;
  سیٹ->psl[i] = psl;
  ++سیٹ->گنتی;
}

ساكن bool u8_strset_grow(بنية u8_strset *سیٹ)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  حرف **prev_انکڑے;
  size_t nslots;
  فراغ *d;

  إذا(سیٹ->slotbits >= 8)
    أرجع FALSE;
  nslotbits = (uint8_t)(سیٹ->slotbits + 1);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                سیٹ->slotbits, nslotbits);
#جے_انت
  nslots = CURL_U8_تھاں_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * حجم(حرف *)) + (2 * nslots));
  إذا(!d)
    أرجع FALSE;

  prev_انکڑے = سیٹ->انکڑے;
  prev_ids = سیٹ->ids;
  prev_slots = سیٹ->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wanalyzer-allocation-size"
#جے_انت
  سیٹ->انکڑے = (حرف **)d;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#جے_انت
  سیٹ->ids = (uint8_t *)d + (nslots * حجم(حرف *));
  سیٹ->psl = سیٹ->ids + nslots;
  سیٹ->slotbits = nslotbits;
  سیٹ->slotmask = CURL_U8_تھاں_نقاب(سیٹ->slotbits);
  سیٹ->گنتی = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    لكل(i = 0; i < CURL_U8_تھاں_CNT(prev_slots); ++i) {
      إذا(prev_انکڑے[i])
        u8_strset_addn(سیٹ, prev_ids[i], prev_انکڑے[i]);
    }
  }
  إذا(prev_انکڑے != سیٹ->sdata)
    curlx_free(prev_انکڑے);
  أرجع TRUE;
}

ساكن bool u8_strset_get_اشاریہ(بنية u8_strset *سیٹ,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_تھاں_IDX(سیٹ, id);
  uint8_t psl = 0;
  طالما(سیٹ->انکڑے[i] && (psl <= سیٹ->psl[i])) {
    إذا(سیٹ->ids[i] == id) {
      *pindex = i;
#إذا CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, سیٹ->انکڑے[i]);
#جے_انت
      أرجع TRUE;
    }
    i = CURL_U8_STRSET_تھاں_IDX(سیٹ, i + 1);
    ++psl;
  }
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#جے_انت
  *pindex = 0;
  أرجع FALSE;
}

#جے_تعریف_شدہ UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_گنتی(بنية u8_strset *سیٹ)
{
  أرجع سیٹ->گنتی;
}
#جے_انت

ثابت حرف *Curl_u8_strset_get(بنية u8_strset *سیٹ, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_اشاریہ(سیٹ, id, &i))
    أرجع سیٹ->انکڑے[i];
  أرجع عدم;
}

CURLcode Curl_u8_strset_setn(بنية u8_strset *سیٹ,
                             uint8_t id, حرف *str)
{
  uint8_t i;

  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#جے_انت
  إذا(!str) {
    Curl_u8_strset_unset(سیٹ, id);
    أرجع CURLE_OK;
  }

  إذا(u8_strset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(سیٹ->انکڑے[i]);
    سیٹ->انکڑے[i] = str;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((سیٹ->گنتی >= CURL_U8_STRSET_تھاں_CNT(سیٹ)) && !u8_strset_grow(سیٹ)) {
    curlx_free(str);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  u8_strset_addn(سیٹ, id, str);
  أرجع CURLE_OK;
}

CURLcode Curl_u8_strset_setx(بنية u8_strset *سیٹ,
                             uint8_t id, ثابت حرف *str, size_t slen)
{
  حرف *val;

  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  إذا(!str) {
    Curl_u8_strset_unset(سیٹ, id);
    أرجع CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  إذا(!val)
    أرجع CURLE_باہرلا_دا_حافظہ;
  أرجع Curl_u8_strset_setn(سیٹ, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_سیٹ(بنية u8_strset *سیٹ,
                                uint8_t id, ثابت حرف *str);
UNITTEST CURLcode u8_strset_سیٹ(بنية u8_strset *سیٹ,
                                uint8_t id, ثابت حرف *str)
{
  أرجع Curl_u8_strset_setx(سیٹ, id, str, str ? لڑی_لمبائی(str) : 0);
}

ساكن فراغ u8_strset_unset(بنية u8_strset *سیٹ, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set */
    إذا(zero)
      curlx_strzero(سیٹ->انکڑے[i]);
    curlx_safefree(سیٹ->انکڑے[i]);
    سیٹ->ids[i] = سیٹ->psl[i] = 0;
    --سیٹ->گنتی;
    j = CURL_U8_STRSET_تھاں_IDX(سیٹ, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(سیٹ->انکڑے[j] && سیٹ->psl[j]) {
      سیٹ->انکڑے[i] = سیٹ->انکڑے[j];
      سیٹ->ids[i] = سیٹ->ids[j];
      سیٹ->psl[i] = (uint8_t)(سیٹ->psl[j] - 1);
      سیٹ->انکڑے[j] = عدم;
      سیٹ->ids[j] = سیٹ->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_تھاں_IDX(سیٹ, i + 1);
    }
  }
}

فراغ Curl_u8_strset_unset(بنية u8_strset *سیٹ, uint8_t id)
{
  u8_strset_unset(سیٹ, id, FALSE);
}

فراغ Curl_u8_strset_unset0(بنية u8_strset *سیٹ, uint8_t id)
{
  u8_strset_unset(سیٹ, id, TRUE);
}

CURLcode Curl_u8_strset_copy(بنية u8_strset *dest, بنية u8_strset *src)
{
  CURLcode نتیجہ = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  لكل(i = 0; !نتیجہ && (i < CURL_U8_STRSET_تھاں_CNT(src)); ++i) {
    إذا(src->انکڑے[i])
      نتیجہ = u8_strset_سیٹ(dest, src->ids[i], src->انکڑے[i]);
  }
  أرجع نتیجہ;
}
