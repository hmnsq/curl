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

#ضمن "u32_ptrset.h"

/* random patterns for API verification */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو CURL_U32_PTRSET_MAGIC 0x75703272
#جيڪڏهن_پورو

#وصف_ڪريو CURL_U32_PTRSET_DEBUG      0

#جيڪڏهن_وصف_نه_ٿيل CURL_مٽا_سٽا
#وصف_ڪريو CURL_مٽا_سٽا(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#جيڪڏهن_پورو

#وصف_ڪريو CURL_U32_جڳهه_CNT(بٽ)        ((بٽ) ? (1U << (بٽ)) : 0)
#وصف_ڪريو CURL_U32_PTRSET_جڳهه_CNT(s)    CURL_U32_جڳهه_CNT((s)->slotbits)
#وصف_ڪريو CURL_U32_PTRSET_جڳهه_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
فراغ Curl_u32_ptrset_init(بنية u32_ptrset *سيٽ,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(سيٽ, 0, حجم(*سيٽ));
  سيٽ->dtor = dtor;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  سيٽ->init = CURL_U32_PTRSET_MAGIC;
#جيڪڏهن_پورو
}

فراغ Curl_u32_ptrset_clear(بنية u32_ptrset *سيٽ)
{
  uint32_t i;
  DEBUGASSERT(سيٽ->init == CURL_U32_PTRSET_MAGIC);
  إذا(سيٽ->dtor)
    لكل(i = 0; i < CURL_U32_PTRSET_جڳهه_CNT(سيٽ); ++i)
      إذا(سيٽ->معلومات[i])
        سيٽ->dtor(سيٽ->ids[i], سيٽ->معلومات[i]);

  curlx_safefree(سيٽ->معلومات);
  Curl_u32_ptrset_init(سيٽ, سيٽ->dtor);
}

ساكن فراغ u32_ptrset_جوڙيو(بنية u32_ptrset *سيٽ, uint32_t id, فراغ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_جڳهه_IDX(سيٽ, id);
  uint32_t psl = 0;
  طالما(سيٽ->معلومات[i]) {
    إذا(psl > سيٽ->psl[i]) { /*مٽا_سٽا*/
      فراغ *tmpdata;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#جيڪڏهن_پورو
      tmpdata = سيٽ->معلومات[i];
      سيٽ->معلومات[i] = ptr;
      ptr = tmpdata;
      CURL_مٽا_سٽا(سيٽ->psl[i], psl);
      CURL_مٽا_سٽا(سيٽ->ids[i], id);
    }
    i = CURL_U32_PTRSET_جڳهه_IDX(سيٽ, i + 1);
    ++psl;
  }
  سيٽ->ids[i] = id;
  سيٽ->معلومات[i] = ptr;
  سيٽ->psl[i] = psl;
  ++سيٽ->ڳڻپ;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#جيڪڏهن_پورو
}

ساكن bool u32_ptrset_grow(بنية u32_ptrset *سيٽ)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  فراغ **prev_معلومات;
  size_t nslots;
  ثابت size_t جڳهه_سائيز = (حجم(فراغ *) + (2 * حجم(uint32_t)));
  فراغ *d;

  إذا(سيٽ->slotbits >= 31)
    أرجع FALSE;
  nslotbits = سيٽ->slotbits ? (uint8_t)(سيٽ->slotbits + 1) : 5;
  nslots = CURL_U32_جڳهه_CNT(nslotbits);
  إذا(nslots > (SIZE_MAX / جڳهه_سائيز)) /* 32-bit arch may trigger here */
    أرجع FALSE;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                سيٽ->slotbits, nslotbits, nslots);
#جيڪڏهن_پورو
  d = curlx_calloc(1, nslots * جڳهه_سائيز);
  إذا(!d)
    أرجع FALSE;

  prev_معلومات = سيٽ->معلومات;
  prev_ids = سيٽ->ids;
  pslotbits = سيٽ->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wanalyzer-allocation-size"
#جيڪڏهن_پورو
  سيٽ->معلومات = d;
  سيٽ->ids = (uint32_t *)(سيٽ->معلومات + nslots);
  سيٽ->psl = (uint32_t *)(سيٽ->ids + nslots);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

  سيٽ->slotbits = nslotbits;
  سيٽ->slotmask = (uint32_t)((1U << سيٽ->slotbits) - 1);
  سيٽ->ڳڻپ = 0;
  /* re-add previous entries */
  لكل(i = 0; i < CURL_U32_جڳهه_CNT(pslotbits); ++i) {
    إذا(prev_معلومات[i])
      u32_ptrset_جوڙيو(سيٽ, prev_ids[i], prev_معلومات[i]);
  }
  إذا(prev_معلومات)
    curlx_free(prev_معلومات);
  أرجع TRUE;
}

ساكن bool u32_ptrset_get_اشاريو(بنية u32_ptrset *سيٽ,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_جڳهه_IDX(سيٽ, id);
  uint32_t psl = 0;
  إذا(!سيٽ->معلومات)
    أرجع FALSE;
  طالما(سيٽ->معلومات[i] && (psl <= سيٽ->psl[i])) {
    إذا(سيٽ->ids[i] == id) {
      *pindex = i;
#إذا CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, سيٽ->معلومات[i], i);
#جيڪڏهن_پورو
      أرجع TRUE;
    }
    i = CURL_U32_PTRSET_جڳهه_IDX(سيٽ, i + 1);
    ++psl;
  }
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#جيڪڏهن_پورو
  *pindex = 0;
  أرجع FALSE;
}

uint32_t Curl_u32_ptrset_ڳڻپ(بنية u32_ptrset *سيٽ)
{
  أرجع سيٽ->ڳڻپ;
}

فراغ *Curl_u32_ptrset_get(بنية u32_ptrset *سيٽ, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(سيٽ->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_اشاريو(سيٽ, id, &i))
    أرجع سيٽ->معلومات[i];
  أرجع عدم;
}

CURLcode Curl_u32_ptrset_سيٽ(بنية u32_ptrset *سيٽ,
                             uint32_t id, فراغ *ptr)
{
  uint32_t i;

  DEBUGASSERT(سيٽ->init == CURL_U32_PTRSET_MAGIC);
  إذا(!ptr) {
    Curl_u32_ptrset_unset(سيٽ, id);
    أرجع CURLE_OK;
  }

  إذا(u32_ptrset_get_اشاريو(سيٽ, id, &i)) {
    /* `id` is in set, replace value */
    إذا(سيٽ->dtor && (سيٽ->معلومات[i] != ptr))
      سيٽ->dtor(سيٽ->ids[i], سيٽ->معلومات[i]);
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#جيڪڏهن_پورو
    سيٽ->معلومات[i] = ptr;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((سيٽ->ڳڻپ >= CURL_U32_PTRSET_جڳهه_CNT(سيٽ)) &&
     !u32_ptrset_grow(سيٽ)) {
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  u32_ptrset_جوڙيو(سيٽ, id, ptr);
  أرجع CURLE_OK;
}

ساكن فراغ u32_ptrset_unset(بنية u32_ptrset *سيٽ, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(سيٽ->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_اشاريو(سيٽ, id, &i)) {
    /* `id` is in set */
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#جيڪڏهن_پورو
    إذا(سيٽ->dtor)
      سيٽ->dtor(سيٽ->ids[i], سيٽ->معلومات[i]);
    سيٽ->معلومات[i] = عدم;
    سيٽ->ids[i] = سيٽ->psl[i] = 0;
    --سيٽ->ڳڻپ;
    j = CURL_U32_PTRSET_جڳهه_IDX(سيٽ, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(سيٽ->معلومات[j] && سيٽ->psl[j]) {
      سيٽ->معلومات[i] = سيٽ->معلومات[j];
      سيٽ->ids[i] = سيٽ->ids[j];
      سيٽ->psl[i] = سيٽ->psl[j] - 1;
      سيٽ->معلومات[j] = عدم;
      سيٽ->ids[j] = سيٽ->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_جڳهه_IDX(سيٽ, i + 1);
    }
  }
}

فراغ Curl_u32_ptrset_unset(بنية u32_ptrset *سيٽ, uint32_t id)
{
  u32_ptrset_unset(سيٽ, id);
}

فراغ Curl_u32_ptrset_گهمو(بنية u32_ptrset *سيٽ,
                            Curl_u32_ptrset_گهمو_cb *cb,
                            فراغ *user_معلومات)
{
  uint32_t i;
  لكل(i = 0; i < CURL_U32_PTRSET_جڳهه_CNT(سيٽ); ++i) {
    إذا(سيٽ->معلومات[i] && !cb(سيٽ->ids[i], سيٽ->معلومات[i], user_معلومات))
      اكسر;
  }
}
