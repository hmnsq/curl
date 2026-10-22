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

#ଅନ୍ତର୍ଭାଵ "u8_strset.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"

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
ଶୂନ୍ୟ Curl_u8_strset_init(ସଂରଚନା u8_strset *set)
{
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memset(set, 0, ଆକାର(*set));
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic କାଢ଼
#endif
  set->data = set->sdata;
  set->ids = set->sids;
  set->psl = set->spsl;
  set->slotbits = CURL_U8_STRSET_START_BITS;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->ଗଣନା_2 = 0;
#ifdef DEBUGBUILD
  set->init = CURL_U8_STRSET_MAGIC;
#endif
}

ଶୂନ୍ୟ Curl_u8_strset_clear(ସଂରଚନା u8_strset *set)
{
  uint16_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  ପାଇଁ(i = 0; i < CURL_U8_STRSET_SLOT_CNT(set); ++i)
    curlx_safefree(set->data[i]);

  ଯଦି(set->data != set->sdata)
    curlx_safefree(set->data);
  Curl_u8_strset_init(set);
}

ସ୍ଥିର ଶୂନ୍ୟ u8_strset_addn(ସଂରଚନା u8_strset *set, uint8_t id, ଅକ୍ଷର *val)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  ଯେତେବେଳେ(set->data[i]) {
    ଯଦି(psl > set->psl[i]) { /* SWAP */
      ଅକ୍ଷର *tmpdata;
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
  ++set->ଗଣନା_2;
}

ସ୍ଥିର bool u8_strset_grow(ସଂରଚନା u8_strset *set)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  ଅକ୍ଷର **prev_data;
  size_t nslots;
  ଶୂନ୍ୟ *d;

  ଯଦି(set->slotbits >= 8)
    ଫେରନ୍ତୁ FALSE;
  nslotbits = (uint8_t)(set->slotbits + 1);
#ଯଦି CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                set->slotbits, nslotbits);
#endif
  nslots = CURL_U8_SLOT_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * ଆକାର(ଅକ୍ଷର *)) + (2 * nslots));
  ଯଦି(!d)
    ଫେରନ୍ତୁ FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  prev_slots = set->slotbits;
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = (ଅକ୍ଷର **)d;
#ଯଦି defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic କାଢ଼
#endif
  set->ids = (uint8_t *)d + (nslots * ଆକାର(ଅକ୍ଷର *));
  set->psl = set->ids + nslots;
  set->slotbits = nslotbits;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->ଗଣନା_2 = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    ପାଇଁ(i = 0; i < CURL_U8_SLOT_CNT(prev_slots); ++i) {
      ଯଦି(prev_data[i])
        u8_strset_addn(set, prev_ids[i], prev_data[i]);
    }
  }
  ଯଦି(prev_data != set->sdata)
    curlx_free(prev_data);
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର bool u8_strset_get_index(ସଂରଚନା u8_strset *set,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  ଯେତେବେଳେ(set->data[i] && (psl <= set->psl[i])) {
    ଯଦି(set->ids[i] == id) {
      *pindex = i;
#ଯଦି CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, set->data[i]);
#endif
      ଫେରନ୍ତୁ TRUE;
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#ଯଦି CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#endif
  *pindex = 0;
  ଫେରନ୍ତୁ FALSE;
}

#ifdef UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_ଗଣନା(ସଂରଚନା u8_strset *set)
{
  ଫେରନ୍ତୁ set->ଗଣନା_2;
}
#endif

ଅଚଳ ଅକ୍ଷର *Curl_u8_strset_get(ସଂରଚନା u8_strset *set, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  ଯଦି(u8_strset_get_index(set, id, &i))
    ଫେରନ୍ତୁ set->data[i];
  ଫେରନ୍ତୁ ଖାଲି;
}

CURLcode Curl_u8_strset_setn(ସଂରଚନା u8_strset *set,
                             uint8_t id, ଅକ୍ଷର *str)
{
  uint8_t i;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
#ଯଦି CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#endif
  ଯଦି(!str) {
    Curl_u8_strset_unset(set, id);
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(u8_strset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(set->data[i]);
    set->data[i] = str;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ଯଦି((set->ଗଣନା_2 >= CURL_U8_STRSET_SLOT_CNT(set)) && !u8_strset_grow(set)) {
    curlx_free(str);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  u8_strset_addn(set, id, str);
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_u8_strset_setx(ସଂରଚନା u8_strset *set,
                             uint8_t id, ଅଚଳ ଅକ୍ଷର *str, size_t slen)
{
  ଅକ୍ଷର *val;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  ଯଦି(!str) {
    Curl_u8_strset_unset(set, id);
    ଫେରନ୍ତୁ CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  ଯଦି(!val)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ଫେରନ୍ତୁ Curl_u8_strset_setn(set, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_set(ସଂରଚନା u8_strset *set,
                                uint8_t id, ଅଚଳ ଅକ୍ଷର *str);
UNITTEST CURLcode u8_strset_set(ସଂରଚନା u8_strset *set,
                                uint8_t id, ଅଚଳ ଅକ୍ଷର *str)
{
  ଫେରନ୍ତୁ Curl_u8_strset_setx(set, id, str, str ? strlen(str) : 0);
}

ସ୍ଥିର ଶୂନ୍ୟ u8_strset_unset(ସଂରଚନା u8_strset *set, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  ଯଦି(u8_strset_get_index(set, id, &i)) {
    /* `id` is in set */
    ଯଦି(zero)
      curlx_strzero(set->data[i]);
    curlx_safefree(set->data[i]);
    set->ids[i] = set->psl[i] = 0;
    --set->ଗଣନା_2;
    j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    ଯେତେବେଳେ(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = (uint8_t)(set->psl[j] - 1);
      set->data[j] = ଖାଲି;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    }
  }
}

ଶୂନ୍ୟ Curl_u8_strset_unset(ସଂରଚନା u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, FALSE);
}

ଶୂନ୍ୟ Curl_u8_strset_unset0(ସଂରଚନା u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, TRUE);
}

CURLcode Curl_u8_strset_copy(ସଂରଚନା u8_strset *dest, ସଂରଚନା u8_strset *src)
{
  CURLcode result = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  ପାଇଁ(i = 0; !result && (i < CURL_U8_STRSET_SLOT_CNT(src)); ++i) {
    ଯଦି(src->data[i])
      result = u8_strset_set(dest, src->ids[i], src->data[i]);
  }
  ଫେରନ୍ତୁ result;
}
