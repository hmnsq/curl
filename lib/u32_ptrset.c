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
#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه CURL_U32_PTRSET_MAGIC 0x75703272
#د_که_پای

#تعریف_کړه CURL_U32_PTRSET_DEBUG      0

#که_نه_تعریف_شوی CURL_بدلون
#تعریف_کړه CURL_بدلون(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#د_که_پای

#تعریف_کړه CURL_U32_خونه_CNT(بټونه)        ((بټونه) ? (1U << (بټونه)) : 0)
#تعریف_کړه CURL_U32_PTRSET_خونه_CNT(s)    CURL_U32_خونه_CNT((s)->slotbits)
#تعریف_کړه CURL_U32_PTRSET_خونه_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
فراغ Curl_u32_ptrset_init(بنية u32_ptrset *سېټ,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(سېټ, 0, حجم(*سېټ));
  سېټ->dtor = dtor;
#که_تعریف_شوی DEBUGBUILD
  سېټ->init = CURL_U32_PTRSET_MAGIC;
#د_که_پای
}

فراغ Curl_u32_ptrset_clear(بنية u32_ptrset *سېټ)
{
  uint32_t i;
  DEBUGASSERT(سېټ->init == CURL_U32_PTRSET_MAGIC);
  إذا(سېټ->dtor)
    لكل(i = 0; i < CURL_U32_PTRSET_خونه_CNT(سېټ); ++i)
      إذا(سېټ->معلومات[i])
        سېټ->dtor(سېټ->ids[i], سېټ->معلومات[i]);

  curlx_safefree(سېټ->معلومات);
  Curl_u32_ptrset_init(سېټ, سېټ->dtor);
}

ساكن فراغ u32_ptrset_جمع_کړه(بنية u32_ptrset *سېټ, uint32_t id, فراغ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_خونه_IDX(سېټ, id);
  uint32_t psl = 0;
  طالما(سېټ->معلومات[i]) {
    إذا(psl > سېټ->psl[i]) { /*بدلون*/
      فراغ *tmpdata;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#د_که_پای
      tmpdata = سېټ->معلومات[i];
      سېټ->معلومات[i] = ptr;
      ptr = tmpdata;
      CURL_بدلون(سېټ->psl[i], psl);
      CURL_بدلون(سېټ->ids[i], id);
    }
    i = CURL_U32_PTRSET_خونه_IDX(سېټ, i + 1);
    ++psl;
  }
  سېټ->ids[i] = id;
  سېټ->معلومات[i] = ptr;
  سېټ->psl[i] = psl;
  ++سېټ->شمېر;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#د_که_پای
}

ساكن bool u32_ptrset_grow(بنية u32_ptrset *سېټ)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  فراغ **prev_معلومات;
  size_t nslots;
  ثابت size_t خونه_اندازه = (حجم(فراغ *) + (2 * حجم(uint32_t)));
  فراغ *d;

  إذا(سېټ->slotbits >= 31)
    أرجع FALSE;
  nslotbits = سېټ->slotbits ? (uint8_t)(سېټ->slotbits + 1) : 5;
  nslots = CURL_U32_خونه_CNT(nslotbits);
  إذا(nslots > (SIZE_MAX / خونه_اندازه)) /* 32-bit arch may trigger here */
    أرجع FALSE;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                سېټ->slotbits, nslotbits, nslots);
#د_که_پای
  d = curlx_calloc(1, nslots * خونه_اندازه);
  إذا(!d)
    أرجع FALSE;

  prev_معلومات = سېټ->معلومات;
  prev_ids = سېټ->ids;
  pslotbits = سېټ->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wanalyzer-allocation-size"
#د_که_پای
  سېټ->معلومات = d;
  سېټ->ids = (uint32_t *)(سېټ->معلومات + nslots);
  سېټ->psl = (uint32_t *)(سېټ->ids + nslots);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

  سېټ->slotbits = nslotbits;
  سېټ->slotmask = (uint32_t)((1U << سېټ->slotbits) - 1);
  سېټ->شمېر = 0;
  /* re-add previous entries */
  لكل(i = 0; i < CURL_U32_خونه_CNT(pslotbits); ++i) {
    إذا(prev_معلومات[i])
      u32_ptrset_جمع_کړه(سېټ, prev_ids[i], prev_معلومات[i]);
  }
  إذا(prev_معلومات)
    curlx_free(prev_معلومات);
  أرجع TRUE;
}

ساكن bool u32_ptrset_get_شاخص(بنية u32_ptrset *سېټ,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_خونه_IDX(سېټ, id);
  uint32_t psl = 0;
  إذا(!سېټ->معلومات)
    أرجع FALSE;
  طالما(سېټ->معلومات[i] && (psl <= سېټ->psl[i])) {
    إذا(سېټ->ids[i] == id) {
      *pindex = i;
#إذا CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, سېټ->معلومات[i], i);
#د_که_پای
      أرجع TRUE;
    }
    i = CURL_U32_PTRSET_خونه_IDX(سېټ, i + 1);
    ++psl;
  }
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#د_که_پای
  *pindex = 0;
  أرجع FALSE;
}

uint32_t Curl_u32_ptrset_شمېر(بنية u32_ptrset *سېټ)
{
  أرجع سېټ->شمېر;
}

فراغ *Curl_u32_ptrset_get(بنية u32_ptrset *سېټ, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(سېټ->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_شاخص(سېټ, id, &i))
    أرجع سېټ->معلومات[i];
  أرجع عدم;
}

CURLcode Curl_u32_ptrset_سېټ(بنية u32_ptrset *سېټ,
                             uint32_t id, فراغ *ptr)
{
  uint32_t i;

  DEBUGASSERT(سېټ->init == CURL_U32_PTRSET_MAGIC);
  إذا(!ptr) {
    Curl_u32_ptrset_unset(سېټ, id);
    أرجع CURLE_OK;
  }

  إذا(u32_ptrset_get_شاخص(سېټ, id, &i)) {
    /* `id` is in set, replace value */
    إذا(سېټ->dtor && (سېټ->معلومات[i] != ptr))
      سېټ->dtor(سېټ->ids[i], سېټ->معلومات[i]);
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#د_که_پای
    سېټ->معلومات[i] = ptr;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((سېټ->شمېر >= CURL_U32_PTRSET_خونه_CNT(سېټ)) &&
     !u32_ptrset_grow(سېټ)) {
    أرجع CURLE_بهر_د_حافظه;
  }

  u32_ptrset_جمع_کړه(سېټ, id, ptr);
  أرجع CURLE_OK;
}

ساكن فراغ u32_ptrset_unset(بنية u32_ptrset *سېټ, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(سېټ->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_شاخص(سېټ, id, &i)) {
    /* `id` is in set */
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#د_که_پای
    إذا(سېټ->dtor)
      سېټ->dtor(سېټ->ids[i], سېټ->معلومات[i]);
    سېټ->معلومات[i] = عدم;
    سېټ->ids[i] = سېټ->psl[i] = 0;
    --سېټ->شمېر;
    j = CURL_U32_PTRSET_خونه_IDX(سېټ, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(سېټ->معلومات[j] && سېټ->psl[j]) {
      سېټ->معلومات[i] = سېټ->معلومات[j];
      سېټ->ids[i] = سېټ->ids[j];
      سېټ->psl[i] = سېټ->psl[j] - 1;
      سېټ->معلومات[j] = عدم;
      سېټ->ids[j] = سېټ->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_خونه_IDX(سېټ, i + 1);
    }
  }
}

فراغ Curl_u32_ptrset_unset(بنية u32_ptrset *سېټ, uint32_t id)
{
  u32_ptrset_unset(سېټ, id);
}

فراغ Curl_u32_ptrset_لیدنه(بنية u32_ptrset *سېټ,
                            Curl_u32_ptrset_لیدنه_cb *cb,
                            فراغ *user_معلومات)
{
  uint32_t i;
  لكل(i = 0; i < CURL_U32_PTRSET_خونه_CNT(سېټ); ++i) {
    إذا(سېټ->معلومات[i] && !cb(سېټ->ids[i], سېټ->معلومات[i], user_معلومات))
      اكسر;
  }
}
