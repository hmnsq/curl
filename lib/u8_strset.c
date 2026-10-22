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

#include "u8_strset.h"
#include "curlx/strdup.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define CURL_U8_STRSET_MAGIC 0x7117e783
#endif

#define CURL_U8_STRSET_DEBUG      0

#define CURL_SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#define CURL_U8_STRSET_SLOT_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#define CURL_U8_SLOT_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#define CURL_U8_SLOT_MASK(i)             (uint8_t)((1U << (i)) - 1)
#define CURL_U8_STRSET_SLOT_CNT(s)       CURL_U8_SLOT_CNT((s)->slotbits)

/* A hashset for tuples (id, string) using Robin Hood Hashing.
 * <https://www.cs.cornell.edu/courses/JavaAndDS/files/hashing_RobinHood.pdf>
 * The basic idea here to handle collisions by robbing "rich" entries and
 * giving to the "poor":
 * - We have an array: (id, string) are ideally placed at index "id % size".
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
void Curl_u8_strset_init(sa_nracanaa u8_strset *set)
{
#ᮚᮦᮔ᮪ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memset(set, 0, parimaa_na(*set));
#ᮚᮦᮔ᮪ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic pop
#endif
  set->data = set->sdata;
  set->ids = set->sids;
  set->psl = set->spsl;
  set->slotbits = CURL_U8_STRSET_START_BITS;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->count = 0;
#ifdef DEBUGBUILD
  set->init = CURL_U8_STRSET_MAGIC;
#endif
}

void Curl_u8_strset_clear(sa_nracanaa u8_strset *set)
{
  uint16_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  for(i = 0; i < CURL_U8_STRSET_SLOT_CNT(set); ++i)
    curlx_safefree(set->data[i]);

  ᮚᮦᮔ᮪(set->data != set->sdata)
    curlx_safefree(set->data);
  Curl_u8_strset_init(set);
}

static void u8_strset_addn(sa_nracanaa u8_strset *set, uint8_t id, var_na *val)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  ᮞᮨᮓᮨᮜ(set->data[i]) {
    ᮚᮦᮔ᮪(psl > set->psl[i]) { /* SWAP */
      var_na *tmpdata;
      tmpdata = set->data[i];
      set->data[i] = val;
      val = tmpdata;
      CURL_SWAP(set->psl[i], psl);
      CURL_SWAP(set->ids[i], id);
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
  set->ids[i] = id;
  set->data[i] = val;
  set->psl[i] = psl;
  ++set->count;
}

static bool u8_strset_grow(sa_nracanaa u8_strset *set)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  var_na **prev_data;
  size_t nslots;
  void *d;

  ᮚᮦᮔ᮪(set->slotbits >= 8)
    ᮝᮜᮤ FALSE;
  nslotbits = (uint8_t)(set->slotbits + 1);
#ᮚᮦᮔ᮪ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                set->slotbits, nslotbits);
#endif
  nslots = CURL_U8_SLOT_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * parimaa_na(var_na *)) + (2 * nslots));
  ᮚᮦᮔ᮪(!d)
    ᮝᮜᮤ FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  prev_slots = set->slotbits;
#ᮚᮦᮔ᮪ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = (var_na **)d;
#ᮚᮦᮔ᮪ defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic pop
#endif
  set->ids = (uint8_t *)d + (nslots * parimaa_na(var_na *));
  set->psl = set->ids + nslots;
  set->slotbits = nslotbits;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->count = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    for(i = 0; i < CURL_U8_SLOT_CNT(prev_slots); ++i) {
      ᮚᮦᮔ᮪(prev_data[i])
        u8_strset_addn(set, prev_ids[i], prev_data[i]);
    }
  }
  ᮚᮦᮔ᮪(prev_data != set->sdata)
    curlx_free(prev_data);
  ᮝᮜᮤ TRUE;
}

static bool u8_strset_get_index(sa_nracanaa u8_strset *set,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  ᮞᮨᮓᮨᮜ(set->data[i] && (psl <= set->psl[i])) {
    ᮚᮦᮔ᮪(set->ids[i] == id) {
      *pindex = i;
#ᮚᮦᮔ᮪ CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, set->data[i]);
#endif
      ᮝᮜᮤ TRUE;
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#ᮚᮦᮔ᮪ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#endif
  *pindex = 0;
  ᮝᮜᮤ FALSE;
}

#ifdef UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_count(sa_nracanaa u8_strset *set)
{
  ᮝᮜᮤ set->count;
}
#endif

const var_na *Curl_u8_strset_get(sa_nracanaa u8_strset *set, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  ᮚᮦᮔ᮪(u8_strset_get_index(set, id, &i))
    ᮝᮜᮤ set->data[i];
  ᮝᮜᮤ NULL;
}

CURLcode Curl_u8_strset_setn(sa_nracanaa u8_strset *set,
                             uint8_t id, var_na *str)
{
  uint8_t i;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
#ᮚᮦᮔ᮪ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#endif
  ᮚᮦᮔ᮪(!str) {
    Curl_u8_strset_unset(set, id);
    ᮝᮜᮤ CURLE_OK;
  }

  ᮚᮦᮔ᮪(u8_strset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(set->data[i]);
    set->data[i] = str;
    ᮝᮜᮤ CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ᮚᮦᮔ᮪((set->count >= CURL_U8_STRSET_SLOT_CNT(set)) && !u8_strset_grow(set)) {
    curlx_free(str);
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }

  u8_strset_addn(set, id, str);
  ᮝᮜᮤ CURLE_OK;
}

CURLcode Curl_u8_strset_setx(sa_nracanaa u8_strset *set,
                             uint8_t id, const var_na *str, size_t slen)
{
  var_na *val;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  ᮚᮦᮔ᮪(!str) {
    Curl_u8_strset_unset(set, id);
    ᮝᮜᮤ CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  ᮚᮦᮔ᮪(!val)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  ᮝᮜᮤ Curl_u8_strset_setn(set, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_set(sa_nracanaa u8_strset *set,
                                uint8_t id, const var_na *str);
UNITTEST CURLcode u8_strset_set(sa_nracanaa u8_strset *set,
                                uint8_t id, const var_na *str)
{
  ᮝᮜᮤ Curl_u8_strset_setx(set, id, str, str ? strlen(str) : 0);
}

static void u8_strset_unset(sa_nracanaa u8_strset *set, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  ᮚᮦᮔ᮪(u8_strset_get_index(set, id, &i)) {
    /* `id` is in set */
    ᮚᮦᮔ᮪(zero)
      curlx_strzero(set->data[i]);
    curlx_safefree(set->data[i]);
    set->ids[i] = set->psl[i] = 0;
    --set->count;
    j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    ᮞᮨᮓᮨᮜ(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = (uint8_t)(set->psl[j] - 1);
      set->data[j] = NULL;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    }
  }
}

void Curl_u8_strset_unset(sa_nracanaa u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, FALSE);
}

void Curl_u8_strset_unset0(sa_nracanaa u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, TRUE);
}

CURLcode Curl_u8_strset_copy(sa_nracanaa u8_strset *dest, sa_nracanaa u8_strset *src)
{
  CURLcode result = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  for(i = 0; !result && (i < CURL_U8_STRSET_SLOT_CNT(src)); ++i) {
    ᮚᮦᮔ᮪(src->data[i])
      result = u8_strset_set(dest, src->ids[i], src->data[i]);
  }
  ᮝᮜᮤ result;
}
