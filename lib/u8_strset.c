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
#شامل "curl_setup.h"

#شامل "u8_strset.h"
#شامل "curlx/strdup.h"

/* random patterns for API verification */
#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_U8_STRSET_MAGIC 0x7117e783
#اگر_ختم

#تعریف_کرو CURL_U8_STRSET_DEBUG      0

#تعریف_کرو CURL_تبادلہ(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#تعریف_کرو CURL_U8_STRSET_خانہ_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#تعریف_کرو CURL_U8_خانہ_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#تعریف_کرو CURL_U8_خانہ_نقاب(i)             (uint8_t)((1U << (i)) - 1)
#تعریف_کرو CURL_U8_STRSET_خانہ_CNT(s)       CURL_U8_خانہ_CNT((s)->slotbits)

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
خالی Curl_u8_strset_init(ساخت u8_strset *سیٹ)
{
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Warray-bounds"
#اگر_ختم
  memset(سیٹ, 0, حجم(*سیٹ));
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#اگر_ختم
  سیٹ->ڈیٹا = سیٹ->sdata;
  سیٹ->ids = سیٹ->sids;
  سیٹ->psl = سیٹ->spsl;
  سیٹ->slotbits = CURL_U8_STRSET_آغاز_بٹ;
  سیٹ->slotmask = CURL_U8_خانہ_نقاب(سیٹ->slotbits);
  سیٹ->گنتی = 0;
#اگر_تعریف_شدہ DEBUGBUILD
  سیٹ->init = CURL_U8_STRSET_MAGIC;
#اگر_ختم
}

خالی Curl_u8_strset_clear(ساخت u8_strset *سیٹ)
{
  uint16_t i;
  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  برائے(i = 0; i < CURL_U8_STRSET_خانہ_CNT(سیٹ); ++i)
    curlx_safefree(سیٹ->ڈیٹا[i]);

  اگر(سیٹ->ڈیٹا != سیٹ->sdata)
    curlx_safefree(سیٹ->ڈیٹا);
  Curl_u8_strset_init(سیٹ);
}

ساکن خالی u8_strset_addn(ساخت u8_strset *سیٹ, uint8_t id, حرف *val)
{
  uint8_t i = CURL_U8_STRSET_خانہ_IDX(سیٹ, id);
  uint8_t psl = 0;
  جب(سیٹ->ڈیٹا[i]) {
    اگر(psl > سیٹ->psl[i]) { /*تبادلہ*/
      حرف *tmpdata;
      tmpdata = سیٹ->ڈیٹا[i];
      سیٹ->ڈیٹا[i] = val;
      val = tmpdata;
      CURL_تبادلہ(سیٹ->psl[i], psl);
      CURL_تبادلہ(سیٹ->ids[i], id);
    }
    i = CURL_U8_STRSET_خانہ_IDX(سیٹ, i + 1);
    ++psl;
  }
  سیٹ->ids[i] = id;
  سیٹ->ڈیٹا[i] = val;
  سیٹ->psl[i] = psl;
  ++سیٹ->گنتی;
}

ساکن bool u8_strset_grow(ساخت u8_strset *سیٹ)
{
  uint8_t *پچھلا_ids, nslotbits;
  uint16_t پچھلا_slots;
  حرف **پچھلا_ڈیٹا;
  size_t nslots;
  خالی *d;

  اگر(سیٹ->slotbits >= 8)
    واپس FALSE;
  nslotbits = (uint8_t)(سیٹ->slotbits + 1);
#اگر CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                سیٹ->slotbits, nslotbits);
#اگر_ختم
  nslots = CURL_U8_خانہ_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * حجم(حرف *)) + (2 * nslots));
  اگر(!d)
    واپس FALSE;

  پچھلا_ڈیٹا = سیٹ->ڈیٹا;
  پچھلا_ids = سیٹ->ids;
  پچھلا_slots = سیٹ->slotbits;
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wanalyzer-allocation-size"
#اگر_ختم
  سیٹ->ڈیٹا = (حرف **)d;
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#اگر_ختم
  سیٹ->ids = (uint8_t *)d + (nslots * حجم(حرف *));
  سیٹ->psl = سیٹ->ids + nslots;
  سیٹ->slotbits = nslotbits;
  سیٹ->slotmask = CURL_U8_خانہ_نقاب(سیٹ->slotbits);
  سیٹ->گنتی = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    برائے(i = 0; i < CURL_U8_خانہ_CNT(پچھلا_slots); ++i) {
      اگر(پچھلا_ڈیٹا[i])
        u8_strset_addn(سیٹ, پچھلا_ids[i], پچھلا_ڈیٹا[i]);
    }
  }
  اگر(پچھلا_ڈیٹا != سیٹ->sdata)
    curlx_free(پچھلا_ڈیٹا);
  واپس TRUE;
}

ساکن bool u8_strset_get_اشاریہ(ساخت u8_strset *سیٹ,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_خانہ_IDX(سیٹ, id);
  uint8_t psl = 0;
  جب(سیٹ->ڈیٹا[i] && (psl <= سیٹ->psl[i])) {
    اگر(سیٹ->ids[i] == id) {
      *pindex = i;
#اگر CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, سیٹ->ڈیٹا[i]);
#اگر_ختم
      واپس TRUE;
    }
    i = CURL_U8_STRSET_خانہ_IDX(سیٹ, i + 1);
    ++psl;
  }
#اگر CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#اگر_ختم
  *pindex = 0;
  واپس FALSE;
}

#اگر_تعریف_شدہ UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_گنتی(ساخت u8_strset *سیٹ)
{
  واپس سیٹ->گنتی;
}
#اگر_ختم

ثابت حرف *Curl_u8_strset_get(ساخت u8_strset *سیٹ, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  اگر(u8_strset_get_اشاریہ(سیٹ, id, &i))
    واپس سیٹ->ڈیٹا[i];
  واپس تہی;
}

CURLcode Curl_u8_strset_setn(ساخت u8_strset *سیٹ,
                             uint8_t id, حرف *str)
{
  uint8_t i;

  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
#اگر CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#اگر_ختم
  اگر(!str) {
    Curl_u8_strset_unset(سیٹ, id);
    واپس CURLE_OK;
  }

  اگر(u8_strset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(سیٹ->ڈیٹا[i]);
    سیٹ->ڈیٹا[i] = str;
    واپس CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  اگر((سیٹ->گنتی >= CURL_U8_STRSET_خانہ_CNT(سیٹ)) && !u8_strset_grow(سیٹ)) {
    curlx_free(str);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  u8_strset_addn(سیٹ, id, str);
  واپس CURLE_OK;
}

CURLcode Curl_u8_strset_setx(ساخت u8_strset *سیٹ,
                             uint8_t id, ثابت حرف *str, size_t slen)
{
  حرف *val;

  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  اگر(!str) {
    Curl_u8_strset_unset(سیٹ, id);
    واپس CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  اگر(!val)
    واپس CURLE_برآمد_کا_حافظہ;
  واپس Curl_u8_strset_setn(سیٹ, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_سیٹ(ساخت u8_strset *سیٹ,
                                uint8_t id, ثابت حرف *str);
UNITTEST CURLcode u8_strset_سیٹ(ساخت u8_strset *سیٹ,
                                uint8_t id, ثابت حرف *str)
{
  واپس Curl_u8_strset_setx(سیٹ, id, str, str ? لڑی_لمبائی(str) : 0);
}

ساکن خالی u8_strset_unset(ساخت u8_strset *سیٹ, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(سیٹ->init == CURL_U8_STRSET_MAGIC);
  اگر(u8_strset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set */
    اگر(zero)
      curlx_strzero(سیٹ->ڈیٹا[i]);
    curlx_safefree(سیٹ->ڈیٹا[i]);
    سیٹ->ids[i] = سیٹ->psl[i] = 0;
    --سیٹ->گنتی;
    j = CURL_U8_STRSET_خانہ_IDX(سیٹ, i + 1);
    /* shift all entries with positive psl "down" */
    جب(سیٹ->ڈیٹا[j] && سیٹ->psl[j]) {
      سیٹ->ڈیٹا[i] = سیٹ->ڈیٹا[j];
      سیٹ->ids[i] = سیٹ->ids[j];
      سیٹ->psl[i] = (uint8_t)(سیٹ->psl[j] - 1);
      سیٹ->ڈیٹا[j] = تہی;
      سیٹ->ids[j] = سیٹ->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_خانہ_IDX(سیٹ, i + 1);
    }
  }
}

خالی Curl_u8_strset_unset(ساخت u8_strset *سیٹ, uint8_t id)
{
  u8_strset_unset(سیٹ, id, FALSE);
}

خالی Curl_u8_strset_unset0(ساخت u8_strset *سیٹ, uint8_t id)
{
  u8_strset_unset(سیٹ, id, TRUE);
}

CURLcode Curl_u8_strset_copy(ساخت u8_strset *dest, ساخت u8_strset *src)
{
  CURLcode نتیجہ = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  برائے(i = 0; !نتیجہ && (i < CURL_U8_STRSET_خانہ_CNT(src)); ++i) {
    اگر(src->ڈیٹا[i])
      نتیجہ = u8_strset_سیٹ(dest, src->ids[i], src->ڈیٹا[i]);
  }
  واپس نتیجہ;
}
