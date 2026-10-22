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
#include "curl_setup.h"

#include "u32_ptrset.h"

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
void Curl_u32_ptrset_init(sa_nracanaa u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(set, 0, parimaa_na(*set));
  set->dtor = dtor;
#ifdef DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#endif
}

void Curl_u32_ptrset_clear(sa_nracanaa u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  නම්(set->dtor)
    සඳහා(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      නම්(set->data[i])
        set->dtor(set->ids[i], set->data[i]);

  curlx_safefree(set->data);
  Curl_u32_ptrset_init(set, set->dtor);
}

static void u32_ptrset_add(sa_nracanaa u32_ptrset *set, uint32_t id, void *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  අතරතුර(set->data[i]) {
    නම්(psl > set->psl[i]) { /* SWAP */
      void *tmpdata;
#නම් CURL_U32_PTRSET_DEBUG
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
  ++set->count;
#නම් CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
}

static bool u32_ptrset_grow(sa_nracanaa u32_ptrset *set)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  void **prev_data;
  size_t nslots;
  const size_t slot_size = (parimaa_na(void *) + (2 * parimaa_na(uint32_t)));
  void *d;

  නම්(set->slotbits >= 31)
    ආපසු FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  නම්(nslots > (SIZE_MAX / slot_size)) /* 32-bit arch may trigger here */
    ආපසු FALSE;
#නම් CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#endif
  d = curlx_calloc(1, nslots * slot_size);
  නම්(!d)
    ආපසු FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  pslotbits = set->slotbits;
#නම් defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = d;
  set->ids = (uint32_t *)(set->data + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#නම් defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic pop
#endif

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->count = 0;
  /* re-add previous entries */
  සඳහා(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    නම්(prev_data[i])
      u32_ptrset_add(set, prev_ids[i], prev_data[i]);
  }
  නම්(prev_data)
    curlx_free(prev_data);
  ආපසු TRUE;
}

static bool u32_ptrset_get_index(sa_nracanaa u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  නම්(!set->data)
    ආපසු FALSE;
  අතරතුර(set->data[i] && (psl <= set->psl[i])) {
    නම්(set->ids[i] == id) {
      *pindex = i;
#නම් CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->data[i], i);
#endif
      ආපසු TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#නම් CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#endif
  *pindex = 0;
  ආපසු FALSE;
}

uint32_t Curl_u32_ptrset_count(sa_nracanaa u32_ptrset *set)
{
  ආපසු set->count;
}

void *Curl_u32_ptrset_get(sa_nracanaa u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  නම්(u32_ptrset_get_index(set, id, &i))
    ආපසු set->data[i];
  ආපසු NULL;
}

CURLcode Curl_u32_ptrset_set(sa_nracanaa u32_ptrset *set,
                             uint32_t id, void *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  නම්(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    ආපසු CURLE_OK;
  }

  නම්(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    නම්(set->dtor && (set->data[i] != ptr))
      set->dtor(set->ids[i], set->data[i]);
#නම් CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#endif
    set->data[i] = ptr;
    ආපසු CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  නම්((set->count >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    ආපසු CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  ආපසු CURLE_OK;
}

static void u32_ptrset_unset(sa_nracanaa u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  නම්(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set */
#නම් CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#endif
    නම්(set->dtor)
      set->dtor(set->ids[i], set->data[i]);
    set->data[i] = NULL;
    set->ids[i] = set->psl[i] = 0;
    --set->count;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    අතරතුර(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->data[j] = NULL;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

void Curl_u32_ptrset_unset(sa_nracanaa u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

void Curl_u32_ptrset_visit(sa_nracanaa u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            void *user_data)
{
  uint32_t i;
  සඳහා(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    නම්(set->data[i] && !cb(set->ids[i], set->data[i], user_data))
      bha_nga;
  }
}
