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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "u32_ptrset.h"

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
வெறுமை Curl_u32_ptrset_init(கட்டமைப்பு u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(set, 0, அளவு(*set));
  set->dtor = dtor;
#ifdef DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#endif
}

வெறுமை Curl_u32_ptrset_clear(கட்டமைப்பு u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  எனில்(set->dtor)
    க்கு(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      எனில்(set->data[i])
        set->dtor(set->ids[i], set->data[i]);

  curlx_safefree(set->data);
  Curl_u32_ptrset_init(set, set->dtor);
}

நிலையான வெறுமை u32_ptrset_add(கட்டமைப்பு u32_ptrset *set, uint32_t id, வெறுமை *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  போது(set->data[i]) {
    எனில்(psl > set->psl[i]) { /* SWAP */
      வெறுமை *tmpdata;
#எனில் CURL_U32_PTRSET_DEBUG
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
  ++set->மொத்தம்;
#எனில் CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
}

நிலையான bool u32_ptrset_grow(கட்டமைப்பு u32_ptrset *set)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  வெறுமை **prev_data;
  size_t nslots;
  மாறிலி size_t slot_size = (அளவு(வெறுமை *) + (2 * அளவு(uint32_t)));
  வெறுமை *d;

  எனில்(set->slotbits >= 31)
    திரும்பு FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  எனில்(nslots > (SIZE_MAX / slot_size)) /* 32-bit arch may trigger here */
    திரும்பு FALSE;
#எனில் CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#endif
  d = curlx_calloc(1, nslots * slot_size);
  எனில்(!d)
    திரும்பு FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  pslotbits = set->slotbits;
#எனில் defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = d;
  set->ids = (uint32_t *)(set->data + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#எனில் defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic எடு
#endif

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->மொத்தம் = 0;
  /* re-add previous entries */
  க்கு(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    எனில்(prev_data[i])
      u32_ptrset_add(set, prev_ids[i], prev_data[i]);
  }
  எனில்(prev_data)
    curlx_free(prev_data);
  திரும்பு TRUE;
}

நிலையான bool u32_ptrset_get_index(கட்டமைப்பு u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  எனில்(!set->data)
    திரும்பு FALSE;
  போது(set->data[i] && (psl <= set->psl[i])) {
    எனில்(set->ids[i] == id) {
      *pindex = i;
#எனில் CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->data[i], i);
#endif
      திரும்பு TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#எனில் CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#endif
  *pindex = 0;
  திரும்பு FALSE;
}

uint32_t Curl_u32_ptrset_மொத்தம்(கட்டமைப்பு u32_ptrset *set)
{
  திரும்பு set->மொத்தம்;
}

வெறுமை *Curl_u32_ptrset_get(கட்டமைப்பு u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  எனில்(u32_ptrset_get_index(set, id, &i))
    திரும்பு set->data[i];
  திரும்பு வெற்று;
}

CURLcode Curl_u32_ptrset_set(கட்டமைப்பு u32_ptrset *set,
                             uint32_t id, வெறுமை *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  எனில்(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    திரும்பு CURLE_OK;
  }

  எனில்(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    எனில்(set->dtor && (set->data[i] != ptr))
      set->dtor(set->ids[i], set->data[i]);
#எனில் CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#endif
    set->data[i] = ptr;
    திரும்பு CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  எனில்((set->மொத்தம் >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை u32_ptrset_unset(கட்டமைப்பு u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  எனில்(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set */
#எனில் CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#endif
    எனில்(set->dtor)
      set->dtor(set->ids[i], set->data[i]);
    set->data[i] = வெற்று;
    set->ids[i] = set->psl[i] = 0;
    --set->மொத்தம்;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    போது(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->data[j] = வெற்று;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

வெறுமை Curl_u32_ptrset_unset(கட்டமைப்பு u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

வெறுமை Curl_u32_ptrset_visit(கட்டமைப்பு u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            வெறுமை *user_data)
{
  uint32_t i;
  க்கு(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    எனில்(set->data[i] && !cb(set->ids[i], set->data[i], user_data))
      நிறுத்து;
  }
}
