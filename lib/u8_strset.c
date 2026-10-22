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
#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه CURL_U8_STRSET_MAGIC 0x7117e783
#د_که_پای

#تعریف_کړه CURL_U8_STRSET_DEBUG      0

#تعریف_کړه CURL_بدلون(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#تعریف_کړه CURL_U8_STRSET_خونه_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#تعریف_کړه CURL_U8_خونه_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#تعریف_کړه CURL_U8_خونه_ماسک(i)             (uint8_t)((1U << (i)) - 1)
#تعریف_کړه CURL_U8_STRSET_خونه_CNT(s)       CURL_U8_خونه_CNT((s)->slotbits)

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
فراغ Curl_u8_strset_init(بنية u8_strset *سېټ)
{
#إذا defined(__GNUC__) && __GNUC__ >= 13
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Warray-bounds"
#د_که_پای
  memset(سېټ, 0, حجم(*سېټ));
#إذا defined(__GNUC__) && __GNUC__ >= 13
#لارښوونه GCC diagnostic راوباسه
#د_که_پای
  سېټ->معلومات = سېټ->sdata;
  سېټ->ids = سېټ->sids;
  سېټ->psl = سېټ->spsl;
  سېټ->slotbits = CURL_U8_STRSET_پیل_بټونه;
  سېټ->slotmask = CURL_U8_خونه_ماسک(سېټ->slotbits);
  سېټ->شمېر = 0;
#که_تعریف_شوی DEBUGBUILD
  سېټ->init = CURL_U8_STRSET_MAGIC;
#د_که_پای
}

فراغ Curl_u8_strset_clear(بنية u8_strset *سېټ)
{
  uint16_t i;
  DEBUGASSERT(سېټ->init == CURL_U8_STRSET_MAGIC);
  لكل(i = 0; i < CURL_U8_STRSET_خونه_CNT(سېټ); ++i)
    curlx_safefree(سېټ->معلومات[i]);

  إذا(سېټ->معلومات != سېټ->sdata)
    curlx_safefree(سېټ->معلومات);
  Curl_u8_strset_init(سېټ);
}

ساكن فراغ u8_strset_addn(بنية u8_strset *سېټ, uint8_t id, حرف *val)
{
  uint8_t i = CURL_U8_STRSET_خونه_IDX(سېټ, id);
  uint8_t psl = 0;
  طالما(سېټ->معلومات[i]) {
    إذا(psl > سېټ->psl[i]) { /*بدلون*/
      حرف *tmpdata;
      tmpdata = سېټ->معلومات[i];
      سېټ->معلومات[i] = val;
      val = tmpdata;
      CURL_بدلون(سېټ->psl[i], psl);
      CURL_بدلون(سېټ->ids[i], id);
    }
    i = CURL_U8_STRSET_خونه_IDX(سېټ, i + 1);
    ++psl;
  }
  سېټ->ids[i] = id;
  سېټ->معلومات[i] = val;
  سېټ->psl[i] = psl;
  ++سېټ->شمېر;
}

ساكن bool u8_strset_grow(بنية u8_strset *سېټ)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  حرف **prev_معلومات;
  size_t nslots;
  فراغ *d;

  إذا(سېټ->slotbits >= 8)
    أرجع FALSE;
  nslotbits = (uint8_t)(سېټ->slotbits + 1);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                سېټ->slotbits, nslotbits);
#د_که_پای
  nslots = CURL_U8_خونه_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * حجم(حرف *)) + (2 * nslots));
  إذا(!d)
    أرجع FALSE;

  prev_معلومات = سېټ->معلومات;
  prev_ids = سېټ->ids;
  prev_slots = سېټ->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wanalyzer-allocation-size"
#د_که_پای
  سېټ->معلومات = (حرف **)d;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#لارښوونه GCC diagnostic راوباسه
#د_که_پای
  سېټ->ids = (uint8_t *)d + (nslots * حجم(حرف *));
  سېټ->psl = سېټ->ids + nslots;
  سېټ->slotbits = nslotbits;
  سېټ->slotmask = CURL_U8_خونه_ماسک(سېټ->slotbits);
  سېټ->شمېر = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    لكل(i = 0; i < CURL_U8_خونه_CNT(prev_slots); ++i) {
      إذا(prev_معلومات[i])
        u8_strset_addn(سېټ, prev_ids[i], prev_معلومات[i]);
    }
  }
  إذا(prev_معلومات != سېټ->sdata)
    curlx_free(prev_معلومات);
  أرجع TRUE;
}

ساكن bool u8_strset_get_شاخص(بنية u8_strset *سېټ,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_خونه_IDX(سېټ, id);
  uint8_t psl = 0;
  طالما(سېټ->معلومات[i] && (psl <= سېټ->psl[i])) {
    إذا(سېټ->ids[i] == id) {
      *pindex = i;
#إذا CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, سېټ->معلومات[i]);
#د_که_پای
      أرجع TRUE;
    }
    i = CURL_U8_STRSET_خونه_IDX(سېټ, i + 1);
    ++psl;
  }
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#د_که_پای
  *pindex = 0;
  أرجع FALSE;
}

#که_تعریف_شوی UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_شمېر(بنية u8_strset *سېټ)
{
  أرجع سېټ->شمېر;
}
#د_که_پای

ثابت حرف *Curl_u8_strset_get(بنية u8_strset *سېټ, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(سېټ->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_شاخص(سېټ, id, &i))
    أرجع سېټ->معلومات[i];
  أرجع عدم;
}

CURLcode Curl_u8_strset_setn(بنية u8_strset *سېټ,
                             uint8_t id, حرف *str)
{
  uint8_t i;

  DEBUGASSERT(سېټ->init == CURL_U8_STRSET_MAGIC);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#د_که_پای
  إذا(!str) {
    Curl_u8_strset_unset(سېټ, id);
    أرجع CURLE_OK;
  }

  إذا(u8_strset_get_شاخص(سېټ, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(سېټ->معلومات[i]);
    سېټ->معلومات[i] = str;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((سېټ->شمېر >= CURL_U8_STRSET_خونه_CNT(سېټ)) && !u8_strset_grow(سېټ)) {
    curlx_free(str);
    أرجع CURLE_بهر_د_حافظه;
  }

  u8_strset_addn(سېټ, id, str);
  أرجع CURLE_OK;
}

CURLcode Curl_u8_strset_setx(بنية u8_strset *سېټ,
                             uint8_t id, ثابت حرف *str, size_t slen)
{
  حرف *val;

  DEBUGASSERT(سېټ->init == CURL_U8_STRSET_MAGIC);
  إذا(!str) {
    Curl_u8_strset_unset(سېټ, id);
    أرجع CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  إذا(!val)
    أرجع CURLE_بهر_د_حافظه;
  أرجع Curl_u8_strset_setn(سېټ, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_سېټ(بنية u8_strset *سېټ,
                                uint8_t id, ثابت حرف *str);
UNITTEST CURLcode u8_strset_سېټ(بنية u8_strset *سېټ,
                                uint8_t id, ثابت حرف *str)
{
  أرجع Curl_u8_strset_setx(سېټ, id, str, str ? مزي_اوږدوالی(str) : 0);
}

ساكن فراغ u8_strset_unset(بنية u8_strset *سېټ, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(سېټ->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_شاخص(سېټ, id, &i)) {
    /* `id` is in set */
    إذا(zero)
      curlx_strzero(سېټ->معلومات[i]);
    curlx_safefree(سېټ->معلومات[i]);
    سېټ->ids[i] = سېټ->psl[i] = 0;
    --سېټ->شمېر;
    j = CURL_U8_STRSET_خونه_IDX(سېټ, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(سېټ->معلومات[j] && سېټ->psl[j]) {
      سېټ->معلومات[i] = سېټ->معلومات[j];
      سېټ->ids[i] = سېټ->ids[j];
      سېټ->psl[i] = (uint8_t)(سېټ->psl[j] - 1);
      سېټ->معلومات[j] = عدم;
      سېټ->ids[j] = سېټ->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_خونه_IDX(سېټ, i + 1);
    }
  }
}

فراغ Curl_u8_strset_unset(بنية u8_strset *سېټ, uint8_t id)
{
  u8_strset_unset(سېټ, id, FALSE);
}

فراغ Curl_u8_strset_unset0(بنية u8_strset *سېټ, uint8_t id)
{
  u8_strset_unset(سېټ, id, TRUE);
}

CURLcode Curl_u8_strset_copy(بنية u8_strset *dest, بنية u8_strset *src)
{
  CURLcode پایله = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  لكل(i = 0; !پایله && (i < CURL_U8_STRSET_خونه_CNT(src)); ++i) {
    إذا(src->معلومات[i])
      پایله = u8_strset_سېټ(dest, src->ids[i], src->معلومات[i]);
  }
  أرجع پایله;
}
