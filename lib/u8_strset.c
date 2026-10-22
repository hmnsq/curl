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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "u8_strset.h"
#অন্তর্ভাৱ "curlx/strdup.h"

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
শূন্য Curl_u8_strset_init(সংরচনা u8_strset *set)
{
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memset(set, 0, পরিমাণ(*set));
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic বেৰ_কৰো
#endif
  set->data = set->sdata;
  set->ids = set->sids;
  set->psl = set->spsl;
  set->slotbits = CURL_U8_STRSET_START_BITS;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->গুনতি = 0;
#ifdef DEBUGBUILD
  set->init = CURL_U8_STRSET_MAGIC;
#endif
}

শূন্য Curl_u8_strset_clear(সংরচনা u8_strset *set)
{
  uint16_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  বাবে(i = 0; i < CURL_U8_STRSET_SLOT_CNT(set); ++i)
    curlx_safefree(set->data[i]);

  যদি(set->data != set->sdata)
    curlx_safefree(set->data);
  Curl_u8_strset_init(set);
}

স্থির শূন্য u8_strset_addn(সংরচনা u8_strset *set, uint8_t id, ৱর্ণ *val)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  যেতিয়ালৈ(set->data[i]) {
    যদি(psl > set->psl[i]) { /* SWAP */
      ৱর্ণ *tmpdata;
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
  ++set->গুনতি;
}

স্থির bool u8_strset_grow(সংরচনা u8_strset *set)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  ৱর্ণ **prev_data;
  size_t nslots;
  শূন্য *d;

  যদি(set->slotbits >= 8)
    ঘূৰাই FALSE;
  nslotbits = (uint8_t)(set->slotbits + 1);
#যদি CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                set->slotbits, nslotbits);
#endif
  nslots = CURL_U8_SLOT_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * পরিমাণ(ৱর্ণ *)) + (2 * nslots));
  যদি(!d)
    ঘূৰাই FALSE;

  prev_data = set->data;
  prev_ids = set->ids;
  prev_slots = set->slotbits;
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->data = (ৱর্ণ **)d;
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic বেৰ_কৰো
#endif
  set->ids = (uint8_t *)d + (nslots * পরিমাণ(ৱর্ণ *));
  set->psl = set->ids + nslots;
  set->slotbits = nslotbits;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->গুনতি = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    বাবে(i = 0; i < CURL_U8_SLOT_CNT(prev_slots); ++i) {
      যদি(prev_data[i])
        u8_strset_addn(set, prev_ids[i], prev_data[i]);
    }
  }
  যদি(prev_data != set->sdata)
    curlx_free(prev_data);
  ঘূৰাই TRUE;
}

স্থির bool u8_strset_get_index(সংরচনা u8_strset *set,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  যেতিয়ালৈ(set->data[i] && (psl <= set->psl[i])) {
    যদি(set->ids[i] == id) {
      *pindex = i;
#যদি CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, set->data[i]);
#endif
      ঘূৰাই TRUE;
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#যদি CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#endif
  *pindex = 0;
  ঘূৰাই FALSE;
}

#ifdef UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_গুনতি(সংরচনা u8_strset *set)
{
  ঘূৰাই set->গুনতি;
}
#endif

ধ্রুৱ ৱর্ণ *Curl_u8_strset_get(সংরচনা u8_strset *set, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  যদি(u8_strset_get_index(set, id, &i))
    ঘূৰাই set->data[i];
  ঘূৰাই রিক্ত;
}

CURLcode Curl_u8_strset_setn(সংরচনা u8_strset *set,
                             uint8_t id, ৱর্ণ *str)
{
  uint8_t i;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
#যদি CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#endif
  যদি(!str) {
    Curl_u8_strset_unset(set, id);
    ঘূৰাই CURLE_OK;
  }

  যদি(u8_strset_get_index(set, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(set->data[i]);
    set->data[i] = str;
    ঘূৰাই CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  যদি((set->গুনতি >= CURL_U8_STRSET_SLOT_CNT(set)) && !u8_strset_grow(set)) {
    curlx_free(str);
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  u8_strset_addn(set, id, str);
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_u8_strset_setx(সংরচনা u8_strset *set,
                             uint8_t id, ধ্রুৱ ৱর্ণ *str, size_t slen)
{
  ৱর্ণ *val;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  যদি(!str) {
    Curl_u8_strset_unset(set, id);
    ঘূৰাই CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  যদি(!val)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  ঘূৰাই Curl_u8_strset_setn(set, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_set(সংরচনা u8_strset *set,
                                uint8_t id, ধ্রুৱ ৱর্ণ *str);
UNITTEST CURLcode u8_strset_set(সংরচনা u8_strset *set,
                                uint8_t id, ধ্রুৱ ৱর্ণ *str)
{
  ঘূৰাই Curl_u8_strset_setx(set, id, str, str ? strlen(str) : 0);
}

স্থির শূন্য u8_strset_unset(সংরচনা u8_strset *set, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  যদি(u8_strset_get_index(set, id, &i)) {
    /* `id` is in set */
    যদি(zero)
      curlx_strzero(set->data[i]);
    curlx_safefree(set->data[i]);
    set->ids[i] = set->psl[i] = 0;
    --set->গুনতি;
    j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    যেতিয়ালৈ(set->data[j] && set->psl[j]) {
      set->data[i] = set->data[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = (uint8_t)(set->psl[j] - 1);
      set->data[j] = রিক্ত;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    }
  }
}

শূন্য Curl_u8_strset_unset(সংরচনা u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, FALSE);
}

শূন্য Curl_u8_strset_unset0(সংরচনা u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, TRUE);
}

CURLcode Curl_u8_strset_copy(সংরচনা u8_strset *dest, সংরচনা u8_strset *src)
{
  CURLcode result = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  বাবে(i = 0; !result && (i < CURL_U8_STRSET_SLOT_CNT(src)); ++i) {
    যদি(src->data[i])
      result = u8_strset_set(dest, src->ids[i], src->data[i]);
  }
  ঘূৰাই result;
}
