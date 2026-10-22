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

#ਅਨ੍ਤਰ੍ਭਾਵ "u32_ptrset.h"

/* random patterns for API verification */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U32_PTRSET_MAGIC 0x75703272
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U32_PTRSET_DEBUG      0

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_ਅਦਲਾ_ਬਦਲੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਅਦਲਾ_ਬਦਲੀ(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U32_ਥਾਂ_CNT(ਬਿੱਟ)        ((ਬਿੱਟ) ? (1U << (ਬਿੱਟ)) : 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U32_PTRSET_ਥਾਂ_CNT(s)    CURL_U32_ਥਾਂ_CNT((s)->slotbits)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_U32_PTRSET_ਥਾਂ_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

/* A hashset for tuples (id, ptr) using Robin Hood Hashing.
 * <https://www.cs.cornell.edu/courses/JavaAndDS/files/hashing_RobinHood.pdf>
 * The basic idea here to handle collisions by robbing "rich" entries and
 * giving to the "poor":
 * - We have an array: (id, ptr) are ideally placed at index "id % size".
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
ਖਾਲੀ Curl_u32_ptrset_init(ਬਣਤਰ u32_ptrset *ਸੈੱਟ,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(ਸੈੱਟ, 0, ਆਕਾਰ(*ਸੈੱਟ));
  ਸੈੱਟ->dtor = dtor;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਸੈੱਟ->init = CURL_U32_PTRSET_MAGIC;
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Curl_u32_ptrset_clear(ਬਣਤਰ u32_ptrset *ਸੈੱਟ)
{
  uint32_t i;
  DEBUGASSERT(ਸੈੱਟ->init == CURL_U32_PTRSET_MAGIC);
  ਜੇ(ਸੈੱਟ->dtor)
    ਲਈ(i = 0; i < CURL_U32_PTRSET_ਥਾਂ_CNT(ਸੈੱਟ); ++i)
      ਜੇ(ਸੈੱਟ->ਅੰਕੜੇ[i])
        ਸੈੱਟ->dtor(ਸੈੱਟ->ids[i], ਸੈੱਟ->ਅੰਕੜੇ[i]);

  curlx_safefree(ਸੈੱਟ->ਅੰਕੜੇ);
  Curl_u32_ptrset_init(ਸੈੱਟ, ਸੈੱਟ->dtor);
}

ਸਥਿਰ ਖਾਲੀ u32_ptrset_ਜੋੜੋ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ, uint32_t id, ਖਾਲੀ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_ਥਾਂ_IDX(ਸੈੱਟ, id);
  uint32_t psl = 0;
  ਜਦੋਂ(ਸੈੱਟ->ਅੰਕੜੇ[i]) {
    ਜੇ(psl > ਸੈੱਟ->psl[i]) { /*ਅਦਲਾ_ਬਦਲੀ*/
      ਖਾਲੀ *tmpdata;
#ਜੇ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#ਜੇ_ਅੰਤ
      tmpdata = ਸੈੱਟ->ਅੰਕੜੇ[i];
      ਸੈੱਟ->ਅੰਕੜੇ[i] = ptr;
      ptr = tmpdata;
      CURL_ਅਦਲਾ_ਬਦਲੀ(ਸੈੱਟ->psl[i], psl);
      CURL_ਅਦਲਾ_ਬਦਲੀ(ਸੈੱਟ->ids[i], id);
    }
    i = CURL_U32_PTRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    ++psl;
  }
  ਸੈੱਟ->ids[i] = id;
  ਸੈੱਟ->ਅੰਕੜੇ[i] = ptr;
  ਸੈੱਟ->psl[i] = psl;
  ++ਸੈੱਟ->ਗਿਣਤੀ_2;
#ਜੇ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ bool u32_ptrset_grow(ਬਣਤਰ u32_ptrset *ਸੈੱਟ)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  ਖਾਲੀ **prev_ਅੰਕੜੇ;
  size_t nslots;
  ਅਚਲ size_t ਥਾਂ_ਮਾਪ = (ਆਕਾਰ(ਖਾਲੀ *) + (2 * ਆਕਾਰ(uint32_t)));
  ਖਾਲੀ *d;

  ਜੇ(ਸੈੱਟ->slotbits >= 31)
    ਵਾਪਸ FALSE;
  nslotbits = ਸੈੱਟ->slotbits ? (uint8_t)(ਸੈੱਟ->slotbits + 1) : 5;
  nslots = CURL_U32_ਥਾਂ_CNT(nslotbits);
  ਜੇ(nslots > (SIZE_MAX / ਥਾਂ_ਮਾਪ)) /* 32-bit arch may trigger here */
    ਵਾਪਸ FALSE;
#ਜੇ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                ਸੈੱਟ->slotbits, nslotbits, nslots);
#ਜੇ_ਅੰਤ
  d = curlx_calloc(1, nslots * ਥਾਂ_ਮਾਪ);
  ਜੇ(!d)
    ਵਾਪਸ FALSE;

  prev_ਅੰਕੜੇ = ਸੈੱਟ->ਅੰਕੜੇ;
  prev_ids = ਸੈੱਟ->ids;
  pslotbits = ਸੈੱਟ->slotbits;
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wanalyzer-allocation-size"
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->ਅੰਕੜੇ = d;
  ਸੈੱਟ->ids = (uint32_t *)(ਸੈੱਟ->ਅੰਕੜੇ + nslots);
  ਸੈੱਟ->psl = (uint32_t *)(ਸੈੱਟ->ids + nslots);
#ਜੇ defined(__GNUC__) && __GNUC__ >= 13
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

  ਸੈੱਟ->slotbits = nslotbits;
  ਸੈੱਟ->slotmask = (uint32_t)((1U << ਸੈੱਟ->slotbits) - 1);
  ਸੈੱਟ->ਗਿਣਤੀ_2 = 0;
  /* re-add previous entries */
  ਲਈ(i = 0; i < CURL_U32_ਥਾਂ_CNT(pslotbits); ++i) {
    ਜੇ(prev_ਅੰਕੜੇ[i])
      u32_ptrset_ਜੋੜੋ(ਸੈੱਟ, prev_ids[i], prev_ਅੰਕੜੇ[i]);
  }
  ਜੇ(prev_ਅੰਕੜੇ)
    curlx_free(prev_ਅੰਕੜੇ);
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool u32_ptrset_get_ਸੂਚਕਾਂਕ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_ਥਾਂ_IDX(ਸੈੱਟ, id);
  uint32_t psl = 0;
  ਜੇ(!ਸੈੱਟ->ਅੰਕੜੇ)
    ਵਾਪਸ FALSE;
  ਜਦੋਂ(ਸੈੱਟ->ਅੰਕੜੇ[i] && (psl <= ਸੈੱਟ->psl[i])) {
    ਜੇ(ਸੈੱਟ->ids[i] == id) {
      *pindex = i;
#ਜੇ CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, ਸੈੱਟ->ਅੰਕੜੇ[i], i);
#ਜੇ_ਅੰਤ
      ਵਾਪਸ TRUE;
    }
    i = CURL_U32_PTRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    ++psl;
  }
#ਜੇ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#ਜੇ_ਅੰਤ
  *pindex = 0;
  ਵਾਪਸ FALSE;
}

uint32_t Curl_u32_ptrset_ਗਿਣਤੀ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ)
{
  ਵਾਪਸ ਸੈੱਟ->ਗਿਣਤੀ_2;
}

ਖਾਲੀ *Curl_u32_ptrset_get(ਬਣਤਰ u32_ptrset *ਸੈੱਟ, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(ਸੈੱਟ->init == CURL_U32_PTRSET_MAGIC);
  ਜੇ(u32_ptrset_get_ਸੂਚਕਾਂਕ(ਸੈੱਟ, id, &i))
    ਵਾਪਸ ਸੈੱਟ->ਅੰਕੜੇ[i];
  ਵਾਪਸ ਨਿਰਾ;
}

CURLcode Curl_u32_ptrset_ਸੈੱਟ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ,
                             uint32_t id, ਖਾਲੀ *ptr)
{
  uint32_t i;

  DEBUGASSERT(ਸੈੱਟ->init == CURL_U32_PTRSET_MAGIC);
  ਜੇ(!ptr) {
    Curl_u32_ptrset_unset(ਸੈੱਟ, id);
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(u32_ptrset_get_ਸੂਚਕਾਂਕ(ਸੈੱਟ, id, &i)) {
    /* `id` is in set, replace value */
    ਜੇ(ਸੈੱਟ->dtor && (ਸੈੱਟ->ਅੰਕੜੇ[i] != ptr))
      ਸੈੱਟ->dtor(ਸੈੱਟ->ids[i], ਸੈੱਟ->ਅੰਕੜੇ[i]);
#ਜੇ CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#ਜੇ_ਅੰਤ
    ਸੈੱਟ->ਅੰਕੜੇ[i] = ptr;
    ਵਾਪਸ CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ਜੇ((ਸੈੱਟ->ਗਿਣਤੀ_2 >= CURL_U32_PTRSET_ਥਾਂ_CNT(ਸੈੱਟ)) &&
     !u32_ptrset_grow(ਸੈੱਟ)) {
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  u32_ptrset_ਜੋੜੋ(ਸੈੱਟ, id, ptr);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ u32_ptrset_unset(ਬਣਤਰ u32_ptrset *ਸੈੱਟ, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(ਸੈੱਟ->init == CURL_U32_PTRSET_MAGIC);
  ਜੇ(u32_ptrset_get_ਸੂਚਕਾਂਕ(ਸੈੱਟ, id, &i)) {
    /* `id` is in set */
#ਜੇ CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#ਜੇ_ਅੰਤ
    ਜੇ(ਸੈੱਟ->dtor)
      ਸੈੱਟ->dtor(ਸੈੱਟ->ids[i], ਸੈੱਟ->ਅੰਕੜੇ[i]);
    ਸੈੱਟ->ਅੰਕੜੇ[i] = ਨਿਰਾ;
    ਸੈੱਟ->ids[i] = ਸੈੱਟ->psl[i] = 0;
    --ਸੈੱਟ->ਗਿਣਤੀ_2;
    j = CURL_U32_PTRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    /* shift all entries with positive psl "down" */
    ਜਦੋਂ(ਸੈੱਟ->ਅੰਕੜੇ[j] && ਸੈੱਟ->psl[j]) {
      ਸੈੱਟ->ਅੰਕੜੇ[i] = ਸੈੱਟ->ਅੰਕੜੇ[j];
      ਸੈੱਟ->ids[i] = ਸੈੱਟ->ids[j];
      ਸੈੱਟ->psl[i] = ਸੈੱਟ->psl[j] - 1;
      ਸੈੱਟ->ਅੰਕੜੇ[j] = ਨਿਰਾ;
      ਸੈੱਟ->ids[j] = ਸੈੱਟ->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_ਥਾਂ_IDX(ਸੈੱਟ, i + 1);
    }
  }
}

ਖਾਲੀ Curl_u32_ptrset_unset(ਬਣਤਰ u32_ptrset *ਸੈੱਟ, uint32_t id)
{
  u32_ptrset_unset(ਸੈੱਟ, id);
}

ਖਾਲੀ Curl_u32_ptrset_ਦੇਖੋ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ,
                            Curl_u32_ptrset_ਦੇਖੋ_cb *cb,
                            ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  uint32_t i;
  ਲਈ(i = 0; i < CURL_U32_PTRSET_ਥਾਂ_CNT(ਸੈੱਟ); ++i) {
    ਜੇ(ਸੈੱਟ->ਅੰਕੜੇ[i] && !cb(ਸੈੱਟ->ids[i], ਸੈੱਟ->ਅੰਕੜੇ[i], user_ਅੰਕੜੇ))
      ਰੋਕੋ;
  }
}
