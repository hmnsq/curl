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
#समावेश "curl_setup.h"

#समावेश "u32_ptrset.h"

/* random patterns for API verification */
#अगर_घोषित DEBUGBUILD
#घोषणा CURL_U32_PTRSET_MAGIC 0x75703272
#पूर्ण_अगर

#घोषणा CURL_U32_PTRSET_DEBUG      0

#अगर_अघोषित CURL_SWAP
#घोषणा CURL_SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#पूर्ण_अगर

#घोषणा CURL_U32_SLOT_CNT(bits)        ((bits) ? (1U << (bits)) : 0)
#घोषणा CURL_U32_PTRSET_SLOT_CNT(s)    CURL_U32_SLOT_CNT((s)->slotbits)
#घोषणा CURL_U32_PTRSET_SLOT_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
व्योम Curl_u32_ptrset_init(काष्ठा u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor)
{
  स_रखो(set, 0, माप(*set));
  set->dtor = dtor;
#अगर_घोषित DEBUGBUILD
  set->init = CURL_U32_PTRSET_MAGIC;
#पूर्ण_अगर
}

व्योम Curl_u32_ptrset_clear(काष्ठा u32_ptrset *set)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  अगर(set->dtor)
    क्रम(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i)
      अगर(set->तथ्य[i])
        set->dtor(set->ids[i], set->तथ्य[i]);

  curlx_safefree(set->तथ्य);
  Curl_u32_ptrset_init(set, set->dtor);
}

अटल व्योम u32_ptrset_add(काष्ठा u32_ptrset *set, uint32_t id, व्योम *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  जबतक(set->तथ्य[i]) {
    अगर(psl > set->psl[i]) { /* SWAP */
      व्योम *tmpdata;
#अगर CURL_U32_PTRSET_DEBUG
  curl_mfprintf(मानक_त्रुटि, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#पूर्ण_अगर
      tmpdata = set->तथ्य[i];
      set->तथ्य[i] = ptr;
      ptr = tmpdata;
      CURL_SWAP(set->psl[i], psl);
      CURL_SWAP(set->ids[i], id);
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
  set->ids[i] = id;
  set->तथ्य[i] = ptr;
  set->psl[i] = psl;
  ++set->गिनती;
#अगर CURL_U32_PTRSET_DEBUG
  curl_mfprintf(मानक_त्रुटि, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#पूर्ण_अगर
}

अटल bool u32_ptrset_grow(काष्ठा u32_ptrset *set)
{
  uint32_t i, *पूर्व_ids;
  uint8_t nslotbits, pslotbits;
  व्योम **पूर्व_तथ्य;
  माप_प्रकार nslots;
  स्थिर माप_प्रकार slot_आकार = (माप(व्योम *) + (2 * माप(uint32_t)));
  व्योम *d;

  अगर(set->slotbits >= 31)
    वापस FALSE;
  nslotbits = set->slotbits ? (uint8_t)(set->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  अगर(nslots > (SIZE_MAX / slot_आकार)) /* 32-bit arch may trigger here */
    वापस FALSE;
#अगर CURL_U32_PTRSET_DEBUG
  curl_mfprintf(मानक_त्रुटि, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                set->slotbits, nslotbits, nslots);
#पूर्ण_अगर
  d = curlx_calloc(1, nslots * slot_आकार);
  अगर(!d)
    वापस FALSE;

  पूर्व_तथ्य = set->तथ्य;
  पूर्व_ids = set->ids;
  pslotbits = set->slotbits;
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wanalyzer-allocation-size"
#पूर्ण_अगर
  set->तथ्य = d;
  set->ids = (uint32_t *)(set->तथ्य + nslots);
  set->psl = (uint32_t *)(set->ids + nslots);
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

  set->slotbits = nslotbits;
  set->slotmask = (uint32_t)((1U << set->slotbits) - 1);
  set->गिनती = 0;
  /* re-add previous entries */
  क्रम(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    अगर(पूर्व_तथ्य[i])
      u32_ptrset_add(set, पूर्व_ids[i], पूर्व_तथ्य[i]);
  }
  अगर(पूर्व_तथ्य)
    curlx_free(पूर्व_तथ्य);
  वापस TRUE;
}

अटल bool u32_ptrset_get_सूचकांक(काष्ठा u32_ptrset *set,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(set, id);
  uint32_t psl = 0;
  अगर(!set->तथ्य)
    वापस FALSE;
  जबतक(set->तथ्य[i] && (psl <= set->psl[i])) {
    अगर(set->ids[i] == id) {
      *pindex = i;
#अगर CURL_U32_PTRSET_DEBUG
      curl_mfprintf(मानक_त्रुटि, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, set->तथ्य[i], i);
#पूर्ण_अगर
      वापस TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#अगर CURL_U32_PTRSET_DEBUG
  curl_mfprintf(मानक_त्रुटि, "u32_ptrset_index %u not found\n", id);
#पूर्ण_अगर
  *pindex = 0;
  वापस FALSE;
}

uint32_t Curl_u32_ptrset_गिनती(काष्ठा u32_ptrset *set)
{
  वापस set->गिनती;
}

व्योम *Curl_u32_ptrset_get(काष्ठा u32_ptrset *set, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  अगर(u32_ptrset_get_सूचकांक(set, id, &i))
    वापस set->तथ्य[i];
  वापस शून्य;
}

CURLcode Curl_u32_ptrset_set(काष्ठा u32_ptrset *set,
                             uint32_t id, व्योम *ptr)
{
  uint32_t i;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  अगर(!ptr) {
    Curl_u32_ptrset_unset(set, id);
    वापस CURLE_OK;
  }

  अगर(u32_ptrset_get_सूचकांक(set, id, &i)) {
    /* `id` is in set, replace value */
    अगर(set->dtor && (set->तथ्य[i] != ptr))
      set->dtor(set->ids[i], set->तथ्य[i]);
#अगर CURL_U32_PTRSET_DEBUG
    curl_mfprintf(मानक_त्रुटि, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#पूर्ण_अगर
    set->तथ्य[i] = ptr;
    वापस CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  अगर((set->गिनती >= CURL_U32_PTRSET_SLOT_CNT(set)) &&
     !u32_ptrset_grow(set)) {
    वापस CURLE_OUT_OF_MEMORY;
  }

  u32_ptrset_add(set, id, ptr);
  वापस CURLE_OK;
}

अटल व्योम u32_ptrset_unset(काष्ठा u32_ptrset *set, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(set->init == CURL_U32_PTRSET_MAGIC);
  अगर(u32_ptrset_get_सूचकांक(set, id, &i)) {
    /* `id` is in set */
#अगर CURL_U32_PTRSET_DEBUG
    curl_mfprintf(मानक_त्रुटि, "u32_ptrset_unset %u at idx=%u\n", id, i);
#पूर्ण_अगर
    अगर(set->dtor)
      set->dtor(set->ids[i], set->तथ्य[i]);
    set->तथ्य[i] = शून्य;
    set->ids[i] = set->psl[i] = 0;
    --set->गिनती;
    j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    जबतक(set->तथ्य[j] && set->psl[j]) {
      set->तथ्य[i] = set->तथ्य[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = set->psl[j] - 1;
      set->तथ्य[j] = शून्य;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(set, i + 1);
    }
  }
}

व्योम Curl_u32_ptrset_unset(काष्ठा u32_ptrset *set, uint32_t id)
{
  u32_ptrset_unset(set, id);
}

व्योम Curl_u32_ptrset_visit(काष्ठा u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            व्योम *user_तथ्य)
{
  uint32_t i;
  क्रम(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(set); ++i) {
    अगर(set->तथ्य[i] && !cb(set->ids[i], set->तथ्य[i], user_तथ्य))
      अवरोध;
  }
}
