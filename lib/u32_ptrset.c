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
শূন্য Curl_u32_ptrset_init(সংরচনা u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(set, 0, পরিমাণ(*set));
  set->dtor = dtor;
#ifdef DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#endif
}

শূন্য Curl_u32_ptrset_clear(সংরচনা u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(set->dtor)
    প্রতি(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      যদি(set->data[i])
        set->dtor(set->ids[i], set->data[i]);

  curlx_safefree(set->data);
  Curl_u32_ptrset_init(set, set->dtor);
}

স্থির শূন্য u32_ptrset_add(সংরচনা u32_ptrset *set, uint32_t id, শূন্য *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  যাবত্(set->data[i]) {
    যদি(psl > set->psl[i]) { /* SWAP */
      শূন্য *tmpdata;
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
      tmpdata = set->data[i];
      set->data[i] = ptr;
      ptr = tmpdata;
      CURL_SWAP(set->psl[i], psl);
      CURL_SWAP(set->ids[i], id);
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
  set->ids[i] = id;
  set->data[i] = ptr;
  set->psl[i] = psl;
  ++set->গুনতি;
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
}

স্থির bool u32_ptrset_grow(সংরচনা u32_ptrset *set)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  শূন্য **prev_data;
  size_t nslots;
  ধ্রুব size_t slot_size = (পরিমাণ(শূন্য *) + (2 * পরিমাণ(uint32_t)));
  শূন্য *d;

  যদি(set->slotbits >= 31)
    প্রত্যাগম FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  যদি(nslots > (SIZE_MAX / slot_size)) /* 32-bit arch may trigger here */
    প্রত্যাগম FALSE;
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#endif
  d = curlx_calloc(1, nslots * slot_size);
  যদি(!d)
    প্রত্যাগম FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  pslotbits = set->slotbits;
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = d;
  set->ids = (uint32_t *)(set->data + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic বের_করো
#endif

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->গুনতি = 0;
  /* re-add previous entries */
  প্রতি(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    যদি(prev_data[i])
      u32_ptrset_add(set, prev_ids[i], prev_data[i]);
  }
  যদি(prev_data)
    curlx_free(prev_data);
  প্রত্যাগম TRUE;
}

স্থির bool u32_ptrset_get_index(সংরচনা u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  যদি(!set->data)
    প্রত্যাগম FALSE;
  যাবত্(set->data[i] && (psl <= set->psl[i])) {
    যদি(set->ids[i] == id) {
      *pindex = i;
#যদি CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->data[i], i);
#endif
      প্রত্যাগম TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#যদি CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#endif
  *pindex = 0;
  প্রত্যাগম FALSE;
}

uint32_t Curl_u32_ptrset_গুনতি(সংরচনা u32_ptrset *set)
{
  প্রত্যাগম set->গুনতি;
}

শূন্য *Curl_u32_ptrset_get(সংরচনা u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(u32_ptrset_get_index(set, id, &i))
    প্রত্যাগম set->data[i];
  প্রত্যাগম রিক্ত;
}

CURLcode Curl_u32_ptrset_set(সংরচনা u32_ptrset *set,
                             uint32_t id, শূন্য *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    প্রত্যাগম CURLE_OK;
  }

  যদি(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    যদি(set->dtor && (set->data[i] != ptr))
      set->dtor(set->ids[i], set->data[i]);
#যদি CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#endif
    set->data[i] = ptr;
    প্রত্যাগম CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  যদি((set->গুনতি >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য u32_ptrset_unset(সংরচনা u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  যদি(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set */
#যদি CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#endif
    যদি(set->dtor)
      set->dtor(set->ids[i], set->data[i]);
    set->data[i] = রিক্ত;
    set->ids[i] = set->psl[i] = 0;
    --set->গুনতি;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    যাবত্(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->data[j] = রিক্ত;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

শূন্য Curl_u32_ptrset_unset(সংরচনা u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

শূন্য Curl_u32_ptrset_visit(সংরচনা u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            শূন্য *user_data)
{
  uint32_t i;
  প্রতি(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    যদি(set->data[i] && !cb(set->ids[i], set->data[i], user_data))
      ভঙ্গ;
  }
}
