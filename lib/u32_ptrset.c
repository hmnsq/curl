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
فراغ Curl_u32_ptrset_init(بنية u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(set, 0, حجم(*set));
  set->dtor = dtor;
#ifdef DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#endif
}

فراغ Curl_u32_ptrset_clear(بنية u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  إذا(set->dtor)
    لكل(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      إذا(set->data[i])
        set->dtor(set->ids[i], set->data[i]);

  curlx_safefree(set->data);
  Curl_u32_ptrset_init(set, set->dtor);
}

ساكن فراغ u32_ptrset_add(بنية u32_ptrset *set, uint32_t id, فراغ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  طالما(set->data[i]) {
    إذا(psl > set->psl[i]) { /* SWAP */
      فراغ *tmpdata;
#إذا CURL_U32_PTRSET_DEBUG
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
  ++set->ژماردن;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
}

ساكن bool u32_ptrset_grow(بنية u32_ptrset *set)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  فراغ **prev_data;
  size_t nslots;
  ثابت size_t slot_size = (حجم(فراغ *) + (2 * حجم(uint32_t)));
  فراغ *d;

  إذا(set->slotbits >= 31)
    أرجع FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  إذا(nslots > (SIZE_MAX / slot_size)) /* 32-bit arch may trigger here */
    أرجع FALSE;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#endif
  d = curlx_calloc(1, nslots * slot_size);
  إذا(!d)
    أرجع FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  pslotbits = set->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic پاڵنان
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = d;
  set->ids = (uint32_t *)(set->data + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic دەرهێنان
#endif

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->ژماردن = 0;
  /* re-add previous entries */
  لكل(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    إذا(prev_data[i])
      u32_ptrset_add(set, prev_ids[i], prev_data[i]);
  }
  إذا(prev_data)
    curlx_free(prev_data);
  أرجع TRUE;
}

ساكن bool u32_ptrset_get_index(بنية u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  إذا(!set->data)
    أرجع FALSE;
  طالما(set->data[i] && (psl <= set->psl[i])) {
    إذا(set->ids[i] == id) {
      *pindex = i;
#إذا CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->data[i], i);
#endif
      أرجع TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#endif
  *pindex = 0;
  أرجع FALSE;
}

uint32_t Curl_u32_ptrset_ژماردن(بنية u32_ptrset *set)
{
  أرجع set->ژماردن;
}

فراغ *Curl_u32_ptrset_get(بنية u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_index(set, id, &i))
    أرجع set->data[i];
  أرجع عدم;
}

CURLcode Curl_u32_ptrset_set(بنية u32_ptrset *set,
                             uint32_t id, فراغ *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  إذا(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    أرجع CURLE_OK;
  }

  إذا(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    إذا(set->dtor && (set->data[i] != ptr))
      set->dtor(set->ids[i], set->data[i]);
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#endif
    set->data[i] = ptr;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((set->ژماردن >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    أرجع CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  أرجع CURLE_OK;
}

ساكن فراغ u32_ptrset_unset(بنية u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set */
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#endif
    إذا(set->dtor)
      set->dtor(set->ids[i], set->data[i]);
    set->data[i] = عدم;
    set->ids[i] = set->psl[i] = 0;
    --set->ژماردن;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->data[j] = عدم;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

فراغ Curl_u32_ptrset_unset(بنية u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

فراغ Curl_u32_ptrset_visit(بنية u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            فراغ *user_data)
{
  uint32_t i;
  لكل(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    إذا(set->data[i] && !cb(set->ids[i], set->data[i], user_data))
      اكسر;
  }
}
