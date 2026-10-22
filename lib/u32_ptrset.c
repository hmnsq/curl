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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "u32_ptrset.h"

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
शून्य Curl_u32_ptrset_init(संरचना u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(set, 0, परिमाण(*set));
  set->dtor = dtor;
#ifdef DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#endif
}

शून्य Curl_u32_ptrset_clear(संरचना u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  सचे(set->dtor)
    अत्थाय(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      सचे(set->data[i])
        set->dtor(set->ids[i], set->data[i]);

  curlx_safefree(set->data);
  Curl_u32_ptrset_init(set, set->dtor);
}

स्थिर शून्य u32_ptrset_add(संरचना u32_ptrset *set, uint32_t id, शून्य *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  याव(set->data[i]) {
    सचे(psl > set->psl[i]) { /* SWAP */
      शून्य *tmpdata;
#सचे CURL_U32_PTRSET_DEBUG
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
  ++set->गणनम्;
#सचे CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#endif
}

स्थिर bool u32_ptrset_grow(संरचना u32_ptrset *set)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  शून्य **prev_data;
  size_t nslots;
  ध्रुव size_t slot_size = (परिमाण(शून्य *) + (2 * परिमाण(uint32_t)));
  शून्य *d;

  सचे(set->slotbits >= 31)
    पटिनिवत्त FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  सचे(nslots > (SIZE_MAX / slot_size)) /* 32-bit arch may trigger here */
    पटिनिवत्त FALSE;
#सचे CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#endif
  d = curlx_calloc(1, nslots * slot_size);
  सचे(!d)
    पटिनिवत्त FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  pslotbits = set->slotbits;
#सचे defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic प्रक्षिप
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = d;
  set->ids = (uint32_t *)(set->data + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#सचे defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic निष्कासय
#endif

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->गणनम् = 0;
  /* re-add previous entries */
  अत्थाय(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    सचे(prev_data[i])
      u32_ptrset_add(set, prev_ids[i], prev_data[i]);
  }
  सचे(prev_data)
    curlx_free(prev_data);
  पटिनिवत्त TRUE;
}

स्थिर bool u32_ptrset_get_index(संरचना u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  सचे(!set->data)
    पटिनिवत्त FALSE;
  याव(set->data[i] && (psl <= set->psl[i])) {
    सचे(set->ids[i] == id) {
      *pindex = i;
#सचे CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->data[i], i);
#endif
      पटिनिवत्त TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#सचे CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#endif
  *pindex = 0;
  पटिनिवत्त FALSE;
}

uint32_t Curl_u32_ptrset_गणनम्(संरचना u32_ptrset *set)
{
  पटिनिवत्त set->गणनम्;
}

शून्य *Curl_u32_ptrset_get(संरचना u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  सचे(u32_ptrset_get_index(set, id, &i))
    पटिनिवत्त set->data[i];
  पटिनिवत्त रिक्त;
}

CURLcode Curl_u32_ptrset_set(संरचना u32_ptrset *set,
                             uint32_t id, शून्य *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  सचे(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    पटिनिवत्त CURLE_OK;
  }

  सचे(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    सचे(set->dtor && (set->data[i] != ptr))
      set->dtor(set->ids[i], set->data[i]);
#सचे CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#endif
    set->data[i] = ptr;
    पटिनिवत्त CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  सचे((set->गणनम् >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य u32_ptrset_unset(संरचना u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  सचे(u32_ptrset_get_index(set, id, &i)) {
    /* `id` is in set */
#सचे CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#endif
    सचे(set->dtor)
      set->dtor(set->ids[i], set->data[i]);
    set->data[i] = रिक्त;
    set->ids[i] = set->psl[i] = 0;
    --set->गणनम्;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    याव(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->data[j] = रिक्त;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

शून्य Curl_u32_ptrset_unset(संरचना u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

शून्य Curl_u32_ptrset_visit(संरचना u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            शून्य *user_data)
{
  uint32_t i;
  अत्थाय(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    सचे(set->data[i] && !cb(set->ids[i], set->data[i], user_data))
      भङ्ग;
  }
}
