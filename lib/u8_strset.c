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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "u8_strset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

/* random patterns for API verification */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U8_STRSET_MAGIC 0x7117e783
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U8_STRSET_DEBUG      0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਅਦਲਾ_ਬਦਲੀ(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U8_STRSET_ਥਾਂ_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U8_ਥਾਂ_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U8_ਥਾਂ_ਨਕਾਬ(i)             (uint8_t)((1U << (i)) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U8_STRSET_ਥਾਂ_CNT(s)       CURL_U8_ਥਾਂ_CNT((s)->slotbits)

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
ਖਾਲੀ Curl_u8_strset_init(ਬਣਤਰ u8_strset *ਸੈੱਟ)
{
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Warray-bounds"
#ਜੇ_ਅੰਤ
  memset(ਸੈੱਟ, 0, ਆਕਾਰ(*ਸੈੱਟ));
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->ਅੰਕੜੇ = ਸੈੱਟ->sdata;
  ਸੈੱਟ->ids = ਸੈੱਟ->sids;
  ਸੈੱਟ->psl = ਸੈੱਟ->spsl;
  ਸੈੱਟ->slotbits = CURL_U8_STRSET_ਸ਼ੁਰੂ_ਬਿੱਟ;
  ਸੈੱਟ->slotmask = CURL_U8_ਥਾਂ_ਨਕਾਬ(ਸੈੱਟ->slotbits);
  ਸੈੱਟ->ਗਿਣਤੀ_2 = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਸੈੱਟ->init = CURL_U8_STRSET_MAGIC;
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Curl_u8_strset_clear(ਬਣਤਰ u8_strset *ਸੈੱਟ)
{
  uint16_t i;
  DEBUGASSERT(ਸੈੱਟ->init == CURL_U8_STRSET_MAGIC);
  ਲਈ(i = 0; i < CURL_U8_STRSET_ਥਾਂ_CNT(ਸੈੱਟ); ++i)
    curlx_safefree(ਸੈੱਟ->ਅੰਕੜੇ[i]);

  ਜੇ(ਸੈੱਟ->ਅੰਕੜੇ != ਸੈੱਟ->sdata)
    curlx_safefree(ਸੈੱਟ->ਅੰਕੜੇ);
  Curl_u8_strset_init(ਸੈੱਟ);
}

ਸਥਿਰ ਖਾਲੀ u8_strset_addn(ਬਣਤਰ u8_strset *ਸੈੱਟ, uint8_t id, ਅੱਖਰ *val)
{
  uint8_t i = CURL_U8_STRSET_ਥਾਂ_IDX(ਸੈੱਟ, id);
  uint8_t psl = 0;
  ਜਦੋਂ(ਸੈੱਟ->ਅੰਕੜੇ[i]) {
    ਜੇ(psl > ਸੈੱਟ->psl[i]) { /*ਅਦਲਾ_ਬਦਲੀ*/
      ਅੱਖਰ *tmpdata;
      tmpdata = ਸੈੱਟ->ਅੰਕੜੇ[i];
      ਸੈੱਟ->ਅੰਕੜੇ[i] = val;
      val = tmpdata;
      CURL_ਅਦਲਾ_ਬਦਲੀ(ਸੈੱਟ->psl[i], psl);
      CURL_ਅਦਲਾ_ਬਦਲੀ(ਸੈੱਟ->ids[i], id);
    }
    i = CURL_U8_STRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    ++psl;
  }
  ਸੈੱਟ->ids[i] = id;
  ਸੈੱਟ->ਅੰਕੜੇ[i] = val;
  ਸੈੱਟ->psl[i] = psl;
  ++ਸੈੱਟ->ਗਿਣਤੀ_2;
}

ਸਥਿਰ bool u8_strset_grow(ਬਣਤਰ u8_strset *ਸੈੱਟ)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  ਅੱਖਰ **prev_ਅੰਕੜੇ;
  size_t nslots;
  ਖਾਲੀ *d;

  ਜੇ(ਸੈੱਟ->slotbits >= 8)
    ਵਾਪਸ FALSE;
  nslotbits = (uint8_t)(ਸੈੱਟ->slotbits + 1);
#ਜੇ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                ਸੈੱਟ->slotbits, nslotbits);
#ਜੇ_ਅੰਤ
  nslots = CURL_U8_ਥਾਂ_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * ਆਕਾਰ(ਅੱਖਰ *)) + (2 * nslots));
  ਜੇ(!d)
    ਵਾਪਸ FALSE;

  prev_ਅੰਕੜੇ = ਸੈੱਟ->ਅੰਕੜੇ;
  prev_ids = ਸੈੱਟ->ids;
  prev_slots = ਸੈੱਟ->slotbits;
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wanalyzer-allocation-size"
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->ਅੰਕੜੇ = (ਅੱਖਰ **)d;
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->ids = (uint8_t *)d + (nslots * ਆਕਾਰ(ਅੱਖਰ *));
  ਸੈੱਟ->psl = ਸੈੱਟ->ids + nslots;
  ਸੈੱਟ->slotbits = nslotbits;
  ਸੈੱਟ->slotmask = CURL_U8_ਥਾਂ_ਨਕਾਬ(ਸੈੱਟ->slotbits);
  ਸੈੱਟ->ਗਿਣਤੀ_2 = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    ਲਈ(i = 0; i < CURL_U8_ਥਾਂ_CNT(prev_slots); ++i) {
      ਜੇ(prev_ਅੰਕੜੇ[i])
        u8_strset_addn(ਸੈੱਟ, prev_ids[i], prev_ਅੰਕੜੇ[i]);
    }
  }
  ਜੇ(prev_ਅੰਕੜੇ != ਸੈੱਟ->sdata)
    curlx_free(prev_ਅੰਕੜੇ);
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool u8_strset_get_ਸੂਚਕਾਂਕ(ਬਣਤਰ u8_strset *ਸੈੱਟ,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_ਥਾਂ_IDX(ਸੈੱਟ, id);
  uint8_t psl = 0;
  ਜਦੋਂ(ਸੈੱਟ->ਅੰਕੜੇ[i] && (psl <= ਸੈੱਟ->psl[i])) {
    ਜੇ(ਸੈੱਟ->ids[i] == id) {
      *pindex = i;
#ਜੇ CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, ਸੈੱਟ->ਅੰਕੜੇ[i]);
#ਜੇ_ਅੰਤ
      ਵਾਪਸ TRUE;
    }
    i = CURL_U8_STRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    ++psl;
  }
#ਜੇ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#ਜੇ_ਅੰਤ
  *pindex = 0;
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_ਗਿਣਤੀ(ਬਣਤਰ u8_strset *ਸੈੱਟ)
{
  ਵਾਪਸ ਸੈੱਟ->ਗਿਣਤੀ_2;
}
#ਜੇ_ਅੰਤ

ਅਚਲ ਅੱਖਰ *Curl_u8_strset_get(ਬਣਤਰ u8_strset *ਸੈੱਟ, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(ਸੈੱਟ->init == CURL_U8_STRSET_MAGIC);
  ਜੇ(u8_strset_get_ਸੂਚਕਾਂਕ(ਸੈੱਟ, id, &i))
    ਵਾਪਸ ਸੈੱਟ->ਅੰਕੜੇ[i];
  ਵਾਪਸ ਨਿਰਾ;
}

CURLcode Curl_u8_strset_setn(ਬਣਤਰ u8_strset *ਸੈੱਟ,
                             uint8_t id, ਅੱਖਰ *str)
{
  uint8_t i;

  DEBUGASSERT(ਸੈੱਟ->init == CURL_U8_STRSET_MAGIC);
#ਜੇ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#ਜੇ_ਅੰਤ
  ਜੇ(!str) {
    Curl_u8_strset_unset(ਸੈੱਟ, id);
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(u8_strset_get_ਸੂਚਕਾਂਕ(ਸੈੱਟ, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(ਸੈੱਟ->ਅੰਕੜੇ[i]);
    ਸੈੱਟ->ਅੰਕੜੇ[i] = str;
    ਵਾਪਸ CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ਜੇ((ਸੈੱਟ->ਗਿਣਤੀ_2 >= CURL_U8_STRSET_ਥਾਂ_CNT(ਸੈੱਟ)) && !u8_strset_grow(ਸੈੱਟ)) {
    curlx_free(str);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  u8_strset_addn(ਸੈੱਟ, id, str);
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_u8_strset_setx(ਬਣਤਰ u8_strset *ਸੈੱਟ,
                             uint8_t id, ਅਚਲ ਅੱਖਰ *str, size_t slen)
{
  ਅੱਖਰ *val;

  DEBUGASSERT(ਸੈੱਟ->init == CURL_U8_STRSET_MAGIC);
  ਜੇ(!str) {
    Curl_u8_strset_unset(ਸੈੱਟ, id);
    ਵਾਪਸ CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  ਜੇ(!val)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਵਾਪਸ Curl_u8_strset_setn(ਸੈੱਟ, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_ਸੈੱਟ(ਬਣਤਰ u8_strset *ਸੈੱਟ,
                                uint8_t id, ਅਚਲ ਅੱਖਰ *str);
UNITTEST CURLcode u8_strset_ਸੈੱਟ(ਬਣਤਰ u8_strset *ਸੈੱਟ,
                                uint8_t id, ਅਚਲ ਅੱਖਰ *str)
{
  ਵਾਪਸ Curl_u8_strset_setx(ਸੈੱਟ, id, str, str ? ਲੜੀ_ਲੰਬਾਈ(str) : 0);
}

ਸਥਿਰ ਖਾਲੀ u8_strset_unset(ਬਣਤਰ u8_strset *ਸੈੱਟ, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(ਸੈੱਟ->init == CURL_U8_STRSET_MAGIC);
  ਜੇ(u8_strset_get_ਸੂਚਕਾਂਕ(ਸੈੱਟ, id, &i)) {
    /* `id` is in set */
    ਜੇ(zero)
      curlx_strzero(ਸੈੱਟ->ਅੰਕੜੇ[i]);
    curlx_safefree(ਸੈੱਟ->ਅੰਕੜੇ[i]);
    ਸੈੱਟ->ids[i] = ਸੈੱਟ->psl[i] = 0;
    --ਸੈੱਟ->ਗਿਣਤੀ_2;
    j = CURL_U8_STRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    /* shift all entries with positive psl "down" */
    ਜਦੋਂ(ਸੈੱਟ->ਅੰਕੜੇ[j] && ਸੈੱਟ->psl[j]) {
      ਸੈੱਟ->ਅੰਕੜੇ[i] = ਸੈੱਟ->ਅੰਕੜੇ[j];
      ਸੈੱਟ->ids[i] = ਸੈੱਟ->ids[j];
      ਸੈੱਟ->psl[i] = (uint8_t)(ਸੈੱਟ->psl[j] - 1);
      ਸੈੱਟ->ਅੰਕੜੇ[j] = ਨਿਰਾ;
      ਸੈੱਟ->ids[j] = ਸੈੱਟ->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    }
  }
}

ਖਾਲੀ Curl_u8_strset_unset(ਬਣਤਰ u8_strset *ਸੈੱਟ, uint8_t id)
{
  u8_strset_unset(ਸੈੱਟ, id, FALSE);
}

ਖਾਲੀ Curl_u8_strset_unset0(ਬਣਤਰ u8_strset *ਸੈੱਟ, uint8_t id)
{
  u8_strset_unset(ਸੈੱਟ, id, TRUE);
}

CURLcode Curl_u8_strset_copy(ਬਣਤਰ u8_strset *dest, ਬਣਤਰ u8_strset *src)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  ਲਈ(i = 0; !ਨਤੀਜਾ && (i < CURL_U8_STRSET_ਥਾਂ_CNT(src)); ++i) {
    ਜੇ(src->ਅੰਕੜੇ[i])
      ਨਤੀਜਾ = u8_strset_ਸੈੱਟ(dest, src->ids[i], src->ਅੰਕੜੇ[i]);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
