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
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو CURL_U8_STRSET_MAGIC 0x7117e783
#جيڪڏهن_پورو

#وصف_ڪريو CURL_U8_STRSET_DEBUG      0

#وصف_ڪريو CURL_مٽا_سٽا(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#وصف_ڪريو CURL_U8_STRSET_جڳهه_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#وصف_ڪريو CURL_U8_جڳهه_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#وصف_ڪريو CURL_U8_جڳهه_نقاب(i)             (uint8_t)((1U << (i)) - 1)
#وصف_ڪريو CURL_U8_STRSET_جڳهه_CNT(s)       CURL_U8_جڳهه_CNT((s)->slotbits)

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
فراغ Curl_u8_strset_init(بنية u8_strset *سيٽ)
{
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Warray-bounds"
#جيڪڏهن_پورو
  memset(سيٽ, 0, حجم(*سيٽ));
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
  سيٽ->معلومات = سيٽ->sdata;
  سيٽ->ids = سيٽ->sids;
  سيٽ->psl = سيٽ->spsl;
  سيٽ->slotbits = CURL_U8_STRSET_شروعات_بٽ;
  سيٽ->slotmask = CURL_U8_جڳهه_نقاب(سيٽ->slotbits);
  سيٽ->ڳڻپ = 0;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  سيٽ->init = CURL_U8_STRSET_MAGIC;
#جيڪڏهن_پورو
}

فراغ Curl_u8_strset_clear(بنية u8_strset *سيٽ)
{
  uint16_t i;
  DEBUGASSERT(سيٽ->init == CURL_U8_STRSET_MAGIC);
  لكل(i = 0; i < CURL_U8_STRSET_جڳهه_CNT(سيٽ); ++i)
    curlx_safefree(سيٽ->معلومات[i]);

  إذا(سيٽ->معلومات != سيٽ->sdata)
    curlx_safefree(سيٽ->معلومات);
  Curl_u8_strset_init(سيٽ);
}

ساكن فراغ u8_strset_addn(بنية u8_strset *سيٽ, uint8_t id, حرف *val)
{
  uint8_t i = CURL_U8_STRSET_جڳهه_IDX(سيٽ, id);
  uint8_t psl = 0;
  طالما(سيٽ->معلومات[i]) {
    إذا(psl > سيٽ->psl[i]) { /*مٽا_سٽا*/
      حرف *tmpdata;
      tmpdata = سيٽ->معلومات[i];
      سيٽ->معلومات[i] = val;
      val = tmpdata;
      CURL_مٽا_سٽا(سيٽ->psl[i], psl);
      CURL_مٽا_سٽا(سيٽ->ids[i], id);
    }
    i = CURL_U8_STRSET_جڳهه_IDX(سيٽ, i + 1);
    ++psl;
  }
  سيٽ->ids[i] = id;
  سيٽ->معلومات[i] = val;
  سيٽ->psl[i] = psl;
  ++سيٽ->ڳڻپ;
}

ساكن bool u8_strset_grow(بنية u8_strset *سيٽ)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  حرف **prev_معلومات;
  size_t nslots;
  فراغ *d;

  إذا(سيٽ->slotbits >= 8)
    أرجع FALSE;
  nslotbits = (uint8_t)(سيٽ->slotbits + 1);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                سيٽ->slotbits, nslotbits);
#جيڪڏهن_پورو
  nslots = CURL_U8_جڳهه_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * حجم(حرف *)) + (2 * nslots));
  إذا(!d)
    أرجع FALSE;

  prev_معلومات = سيٽ->معلومات;
  prev_ids = سيٽ->ids;
  prev_slots = سيٽ->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wanalyzer-allocation-size"
#جيڪڏهن_پورو
  سيٽ->معلومات = (حرف **)d;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
  سيٽ->ids = (uint8_t *)d + (nslots * حجم(حرف *));
  سيٽ->psl = سيٽ->ids + nslots;
  سيٽ->slotbits = nslotbits;
  سيٽ->slotmask = CURL_U8_جڳهه_نقاب(سيٽ->slotbits);
  سيٽ->ڳڻپ = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    لكل(i = 0; i < CURL_U8_جڳهه_CNT(prev_slots); ++i) {
      إذا(prev_معلومات[i])
        u8_strset_addn(سيٽ, prev_ids[i], prev_معلومات[i]);
    }
  }
  إذا(prev_معلومات != سيٽ->sdata)
    curlx_free(prev_معلومات);
  أرجع TRUE;
}

ساكن bool u8_strset_get_اشاريو(بنية u8_strset *سيٽ,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_جڳهه_IDX(سيٽ, id);
  uint8_t psl = 0;
  طالما(سيٽ->معلومات[i] && (psl <= سيٽ->psl[i])) {
    إذا(سيٽ->ids[i] == id) {
      *pindex = i;
#إذا CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, سيٽ->معلومات[i]);
#جيڪڏهن_پورو
      أرجع TRUE;
    }
    i = CURL_U8_STRSET_جڳهه_IDX(سيٽ, i + 1);
    ++psl;
  }
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#جيڪڏهن_پورو
  *pindex = 0;
  أرجع FALSE;
}

#جيڪڏهن_وصف_ٿيل UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_ڳڻپ(بنية u8_strset *سيٽ)
{
  أرجع سيٽ->ڳڻپ;
}
#جيڪڏهن_پورو

ثابت حرف *Curl_u8_strset_get(بنية u8_strset *سيٽ, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(سيٽ->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_اشاريو(سيٽ, id, &i))
    أرجع سيٽ->معلومات[i];
  أرجع عدم;
}

CURLcode Curl_u8_strset_setn(بنية u8_strset *سيٽ,
                             uint8_t id, حرف *str)
{
  uint8_t i;

  DEBUGASSERT(سيٽ->init == CURL_U8_STRSET_MAGIC);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#جيڪڏهن_پورو
  إذا(!str) {
    Curl_u8_strset_unset(سيٽ, id);
    أرجع CURLE_OK;
  }

  إذا(u8_strset_get_اشاريو(سيٽ, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(سيٽ->معلومات[i]);
    سيٽ->معلومات[i] = str;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((سيٽ->ڳڻپ >= CURL_U8_STRSET_جڳهه_CNT(سيٽ)) && !u8_strset_grow(سيٽ)) {
    curlx_free(str);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  u8_strset_addn(سيٽ, id, str);
  أرجع CURLE_OK;
}

CURLcode Curl_u8_strset_setx(بنية u8_strset *سيٽ,
                             uint8_t id, ثابت حرف *str, size_t slen)
{
  حرف *val;

  DEBUGASSERT(سيٽ->init == CURL_U8_STRSET_MAGIC);
  إذا(!str) {
    Curl_u8_strset_unset(سيٽ, id);
    أرجع CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  إذا(!val)
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  أرجع Curl_u8_strset_setn(سيٽ, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_سيٽ(بنية u8_strset *سيٽ,
                                uint8_t id, ثابت حرف *str);
UNITTEST CURLcode u8_strset_سيٽ(بنية u8_strset *سيٽ,
                                uint8_t id, ثابت حرف *str)
{
  أرجع Curl_u8_strset_setx(سيٽ, id, str, str ? لڙي_ڊيگهه(str) : 0);
}

ساكن فراغ u8_strset_unset(بنية u8_strset *سيٽ, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(سيٽ->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_اشاريو(سيٽ, id, &i)) {
    /* `id` is in set */
    إذا(zero)
      curlx_strzero(سيٽ->معلومات[i]);
    curlx_safefree(سيٽ->معلومات[i]);
    سيٽ->ids[i] = سيٽ->psl[i] = 0;
    --سيٽ->ڳڻپ;
    j = CURL_U8_STRSET_جڳهه_IDX(سيٽ, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(سيٽ->معلومات[j] && سيٽ->psl[j]) {
      سيٽ->معلومات[i] = سيٽ->معلومات[j];
      سيٽ->ids[i] = سيٽ->ids[j];
      سيٽ->psl[i] = (uint8_t)(سيٽ->psl[j] - 1);
      سيٽ->معلومات[j] = عدم;
      سيٽ->ids[j] = سيٽ->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_جڳهه_IDX(سيٽ, i + 1);
    }
  }
}

فراغ Curl_u8_strset_unset(بنية u8_strset *سيٽ, uint8_t id)
{
  u8_strset_unset(سيٽ, id, FALSE);
}

فراغ Curl_u8_strset_unset0(بنية u8_strset *سيٽ, uint8_t id)
{
  u8_strset_unset(سيٽ, id, TRUE);
}

CURLcode Curl_u8_strset_copy(بنية u8_strset *dest, بنية u8_strset *src)
{
  CURLcode نتيجو = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  لكل(i = 0; !نتيجو && (i < CURL_U8_STRSET_جڳهه_CNT(src)); ++i) {
    إذا(src->معلومات[i])
      نتيجو = u8_strset_سيٽ(dest, src->ids[i], src->معلومات[i]);
  }
  أرجع نتيجو;
}
