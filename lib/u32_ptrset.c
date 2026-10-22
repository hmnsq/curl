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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "u32_ptrset.h"

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
ଶୂନ୍ୟ Curl_u32_ptrset_init(ସଂରଚନା u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(set, 0, ଆକାର(*set));
  set->dtor = dtor;
#ifdef DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#endif
}

ଶୂନ୍ୟ Curl_u32_ptrset_clear(ସଂରଚନା u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  ଯଦି(set->dtor)
    ପାଇଁ(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      ଯଦି(set->data[i])
        set->dtor(set->ids[i], set->data[i]);

  curlx_safefree(set->data);
  Curl_u32_ptrset_init(set, set->dtor);
}

ସ୍ଥିର ଶୂନ୍ୟ u32_ptrset_add(ସଂରଚନା u32_ptrset *set, uint32_t id, ଶୂନ୍ୟ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  ଯେତେବେଳେ(set->data[i]) {
    ଯଦି(psl > set->psl[i]) { /* SWAP */
      ଶୂନ୍ୟ *tmpdata;
#ଯଦି CURL_U32_PTRSET_DEBUG
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
  ++set->ଗଣନା_2;
#ଯଦି CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
}

ସ୍ଥିର bool u32_ptrset_grow(ସଂରଚନା u32_ptrset *set)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  ଶୂନ୍ୟ **prev_data;
  size_t nslots;
  ଅଚଳ size_t slot_size = (ଆକାର(ଶୂନ୍ୟ *) + (2 * ଆକାର(uint32_t)));
  ଶୂନ୍ୟ *d;

  ଯଦି(set->slotbits >= 31)
    ଫେରନ୍ତୁ FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  ଯଦି(nslots > (SIZE_MAX / slot_size)) /* 32-bit arch may trigger here */
    ଫେରନ୍ତୁ FALSE;
#ଯଦି CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#endif
  d = curlx_calloc(1, nslots * slot_size);
  ଯଦି(!d)
    ଫେରନ୍ତୁ FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  pslotbits = set->slotbits;
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = d;
  set->ids = (uint32_t *)(set->data + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic କାଢ଼
#endif

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->ଗଣନା_2 = 0;
  /* re-add previous entries */
  ପାଇଁ(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    ଯଦି(prev_data[i])
      u32_ptrset_add(set, prev_ids[i], prev_data[i]);
  }
  ଯଦି(prev_data)
    curlx_free(prev_data);
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର bool u32_ptrset_get_index(ସଂରଚନା u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  ଯଦି(!set->data)
    ଫେରନ୍ତୁ FALSE;
  ଯେତେବେଳେ(set->data[i] && (psl <= set->psl[i])) {
    ଯଦି(set->ids[i] == id) {
      *pindex = i;
#ଯଦି CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->data[i], i);
#endif
      ଫେରନ୍ତୁ TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#ଯଦି CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#endif
  *pindex = 0;
  ଫେରନ୍ତୁ FALSE;
}

uint32_t Curl_u32_ptrset_ଗଣନା(ସଂରଚନା u32_ptrset *set)
{
  ଫେରନ୍ତୁ set->ଗଣନା_2;
}

ଶୂନ୍ୟ *Curl_u32_ptrset_get(ସଂରଚନା u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  ଯଦି(u32_ptrset_get_index(set, id, &i))
    ଫେରନ୍ତୁ set->data[i];
  ଫେରନ୍ତୁ ଖାଲି;
}

CURLcode Curl_u32_ptrset_set(ସଂରଚନା u32_ptrset *set,
                             uint32_t id, ଶୂନ୍ୟ *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  ଯଦି(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    ଯଦି(set->dtor && (set->data[i] != ptr))
      set->dtor(set->ids[i], set->data[i]);
#ଯଦି CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#endif
    set->data[i] = ptr;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ଯଦି((set->ଗଣନା_2 >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ u32_ptrset_unset(ସଂରଚନା u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  ଯଦି(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set */
#ଯଦି CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#endif
    ଯଦି(set->dtor)
      set->dtor(set->ids[i], set->data[i]);
    set->data[i] = ଖାଲି;
    set->ids[i] = set->psl[i] = 0;
    --set->ଗଣନା_2;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    ଯେତେବେଳେ(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->data[j] = ଖାଲି;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

ଶୂନ୍ୟ Curl_u32_ptrset_unset(ସଂରଚନା u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

ଶୂନ୍ୟ Curl_u32_ptrset_visit(ସଂରଚନା u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            ଶୂନ୍ୟ *user_data)
{
  uint32_t i;
  ପାଇଁ(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    ଯଦି(set->data[i] && !cb(set->ids[i], set->data[i], user_data))
      ଭାଙ୍ଗ;
  }
}
