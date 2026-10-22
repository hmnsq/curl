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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "u32_ptrset.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define CURL_U32_PTRSET_MAGIC 0x75703272
#endif

#define CURL_U32_PTRSET_DEBUG      0

#ifndef CURL_SWAP
#define CURL_SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#endif

#define CURL_U32_SLOT_CNT(bits)        ((bits) ? (1U << (bits)) : 0)
#define CURL_U32_PTRSET_SLOT_CNT(s)    CURL_U32_SLOT_CNT((s)->slotbits)
#define CURL_U32_PTRSET_SLOT_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
শূন্য Curl_u32_ptrset_init(কাঠামো u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(set, 0, আকার(*set));
  set->dtor = dtor;
#ifdef DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#endif
}

শূন্য Curl_u32_ptrset_clear(কাঠামো u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(set->dtor)
    জন্য(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      যদি(set->তথ্য[i])
        set->dtor(set->ids[i], set->তথ্য[i]);

  curlx_safefree(set->তথ্য);
  Curl_u32_ptrset_init(set, set->dtor);
}

স্থির শূন্য u32_ptrset_add(কাঠামো u32_ptrset *set, uint32_t id, শূন্য *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  যখন(set->তথ্য[i]) {
    যদি(psl > set->psl[i]) { /* SWAP */
      শূন্য *tmpdata;
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
      tmpdata = set->তথ্য[i];
      set->তথ্য[i] = ptr;
      ptr = tmpdata;
      CURL_SWAP(set->psl[i], psl);
      CURL_SWAP(set->ids[i], id);
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
  set->ids[i] = id;
  set->তথ্য[i] = ptr;
  set->psl[i] = psl;
  ++set->গুনতি;
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
}

স্থির bool u32_ptrset_grow(কাঠামো u32_ptrset *set)
{
  uint32_t i, *পূর্ব_ids;
  uint8_t nslotbits, pslotbits;
  শূন্য **পূর্ব_তথ্য;
  size_t nslots;
  ধ্রুবক size_t slot_আকার = (আকার(শূন্য *) + (2 * আকার(uint32_t)));
  শূন্য *d;

  যদি(set->slotbits >= 31)
    ফেরত FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  যদি(nslots > (SIZE_MAX / slot_আকার)) /* 32-bit arch may trigger here */
    ফেরত FALSE;
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#endif
  d = curlx_calloc(1, nslots * slot_আকার);
  যদি(!d)
    ফেরত FALSE;

  পূর্ব_তথ্য = set->তথ্য;
  পূর্ব_ids = set->ids;
  pslotbits = set->slotbits;
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->তথ্য = d;
  set->ids = (uint32_t *)(set->তথ্য + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic বের_করো
#endif

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->গুনতি = 0;
  /* re-add previous entries */
  জন্য(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    যদি(পূর্ব_তথ্য[i])
      u32_ptrset_add(set, পূর্ব_ids[i], পূর্ব_তথ্য[i]);
  }
  যদি(পূর্ব_তথ্য)
    curlx_free(পূর্ব_তথ্য);
  ফেরত TRUE;
}

স্থির bool u32_ptrset_get_সূচক(কাঠামো u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  যদি(!set->তথ্য)
    ফেরত FALSE;
  যখন(set->তথ্য[i] && (psl <= set->psl[i])) {
    যদি(set->ids[i] == id) {
      *pindex = i;
#যদি CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->তথ্য[i], i);
#endif
      ফেরত TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#endif
  *pindex = 0;
  ফেরত FALSE;
}

uint32_t Curl_u32_ptrset_গুনতি(কাঠামো u32_ptrset *set)
{
  ফেরত set->গুনতি;
}

শূন্য *Curl_u32_ptrset_get(কাঠামো u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(u32_ptrset_get_সূচক(set, id, &i))
    ফেরত set->তথ্য[i];
  ফেরত ফাঁকা;
}

CURLcode Curl_u32_ptrset_set(কাঠামো u32_ptrset *set,
                             uint32_t id, শূন্য *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    ফেরত CURLE_OK;
  }

  যদি(u32_ptrset_get_সূচক(set, id, &i)) {
    /* `id` is in set, replace value */
    যদি(set->dtor && (set->তথ্য[i] != ptr))
      set->dtor(set->ids[i], set->তথ্য[i]);
#যদি CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#endif
    set->তথ্য[i] = ptr;
    ফেরত CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  যদি((set->গুনতি >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  ফেরত CURLE_OK;
}

স্থির শূন্য u32_ptrset_unset(কাঠামো u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(u32_ptrset_get_সূচক(set, id, &i)) {
    /* `id` is in set */
#যদি CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#endif
    যদি(set->dtor)
      set->dtor(set->ids[i], set->তথ্য[i]);
    set->তথ্য[i] = ফাঁকা;
    set->ids[i] = set->psl[i] = 0;
    --set->গুনতি;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    যখন(set->তথ্য[j] && set->psl[j]) {
      set->তথ্য[i] = set->তথ্য[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->তথ্য[j] = ফাঁকা;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

শূন্য Curl_u32_ptrset_unset(কাঠামো u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

শূন্য Curl_u32_ptrset_visit(কাঠামো u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            শূন্য *user_তথ্য)
{
  uint32_t i;
  জন্য(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    যদি(set->তথ্য[i] && !cb(set->ids[i], set->তথ্য[i], user_তথ্য))
      বিরতি;
  }
}
