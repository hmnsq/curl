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

#অন্তর্ভাব "u8_strset.h"
#অন্তর্ভাব "curlx/strdup.h"

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
শূন্য Curl_u8_strset_init(কাঠামো u8_strset *set)
{
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  memset(set, 0, আকার(*set));
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic বের_করো
#endif
  set->তথ্য = set->sdata;
  set->ids = set->sids;
  set->psl = set->spsl;
  set->slotbits = CURL_U8_STRSET_START_BITS;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->গুনতি = 0;
#ifdef DEBUGBUILD
  set->init = CURL_U8_STRSET_MAGIC;
#endif
}

শূন্য Curl_u8_strset_clear(কাঠামো u8_strset *set)
{
  uint16_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  জন্য(i = 0; i < CURL_U8_STRSET_SLOT_CNT(set); ++i)
    curlx_safefree(set->তথ্য[i]);

  যদি(set->তথ্য != set->sdata)
    curlx_safefree(set->তথ্য);
  Curl_u8_strset_init(set);
}

স্থির শূন্য u8_strset_addn(কাঠামো u8_strset *set, uint8_t id, অক্ষর *val)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  যখন(set->তথ্য[i]) {
    যদি(psl > set->psl[i]) { /* SWAP */
      অক্ষর *tmpdata;
      tmpdata = set->তথ্য[i];
      set->তথ্য[i] = val;
      val = tmpdata;
      CURL_SWAP(set->psl[i], psl);
      CURL_SWAP(set->ids[i], id);
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
  set->ids[i] = id;
  set->তথ্য[i] = val;
  set->psl[i] = psl;
  ++set->গুনতি;
}

স্থির bool u8_strset_grow(কাঠামো u8_strset *set)
{
  uint8_t *পূর্ব_ids, nslotbits;
  uint16_t পূর্ব_slots;
  অক্ষর **পূর্ব_তথ্য;
  size_t nslots;
  শূন্য *d;

  যদি(set->slotbits >= 8)
    ফেরত FALSE;
  nslotbits = (uint8_t)(set->slotbits + 1);
#যদি CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                set->slotbits, nslotbits);
#endif
  nslots = CURL_U8_SLOT_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * আকার(অক্ষর *)) + (2 * nslots));
  যদি(!d)
    ফেরত FALSE;

  পূর্ব_তথ্য = set->তথ্য;
  পূর্ব_ids = set->ids;
  পূর্ব_slots = set->slotbits;
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"
#endif
  set->তথ্য = (অক্ষর **)d;
#যদি defined(__GNUC__) && __GNUC__ >= 13
#pragma GCC diagnostic বের_করো
#endif
  set->ids = (uint8_t *)d + (nslots * আকার(অক্ষর *));
  set->psl = set->ids + nslots;
  set->slotbits = nslotbits;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->গুনতি = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    জন্য(i = 0; i < CURL_U8_SLOT_CNT(পূর্ব_slots); ++i) {
      যদি(পূর্ব_তথ্য[i])
        u8_strset_addn(set, পূর্ব_ids[i], পূর্ব_তথ্য[i]);
    }
  }
  যদি(পূর্ব_তথ্য != set->sdata)
    curlx_free(পূর্ব_তথ্য);
  ফেরত TRUE;
}

স্থির bool u8_strset_get_সূচক(কাঠামো u8_strset *set,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  যখন(set->তথ্য[i] && (psl <= set->psl[i])) {
    যদি(set->ids[i] == id) {
      *pindex = i;
#যদি CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, set->তথ্য[i]);
#endif
      ফেরত TRUE;
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#যদি CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#endif
  *pindex = 0;
  ফেরত FALSE;
}

#ifdef UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_গুনতি(কাঠামো u8_strset *set)
{
  ফেরত set->গুনতি;
}
#endif

ধ্রুবক অক্ষর *Curl_u8_strset_get(কাঠামো u8_strset *set, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  যদি(u8_strset_get_সূচক(set, id, &i))
    ফেরত set->তথ্য[i];
  ফেরত ফাঁকা;
}

CURLcode Curl_u8_strset_setn(কাঠামো u8_strset *set,
                             uint8_t id, অক্ষর *str)
{
  uint8_t i;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
#যদি CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#endif
  যদি(!str) {
    Curl_u8_strset_unset(set, id);
    ফেরত CURLE_OK;
  }

  যদি(u8_strset_get_সূচক(set, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(set->তথ্য[i]);
    set->তথ্য[i] = str;
    ফেরত CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  যদি((set->গুনতি >= CURL_U8_STRSET_SLOT_CNT(set)) && !u8_strset_grow(set)) {
    curlx_free(str);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  u8_strset_addn(set, id, str);
  ফেরত CURLE_OK;
}

CURLcode Curl_u8_strset_setx(কাঠামো u8_strset *set,
                             uint8_t id, ধ্রুবক অক্ষর *str, size_t slen)
{
  অক্ষর *val;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  যদি(!str) {
    Curl_u8_strset_unset(set, id);
    ফেরত CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  যদি(!val)
    ফেরত CURLE_OUT_OF_MEMORY;
  ফেরত Curl_u8_strset_setn(set, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_set(কাঠামো u8_strset *set,
                                uint8_t id, ধ্রুবক অক্ষর *str);
UNITTEST CURLcode u8_strset_set(কাঠামো u8_strset *set,
                                uint8_t id, ধ্রুবক অক্ষর *str)
{
  ফেরত Curl_u8_strset_setx(set, id, str, str ? strlen(str) : 0);
}

স্থির শূন্য u8_strset_unset(কাঠামো u8_strset *set, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  যদি(u8_strset_get_সূচক(set, id, &i)) {
    /* `id` is in set */
    যদি(zero)
      curlx_strzero(set->তথ্য[i]);
    curlx_safefree(set->তথ্য[i]);
    set->ids[i] = set->psl[i] = 0;
    --set->গুনতি;
    j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    যখন(set->তথ্য[j] && set->psl[j]) {
      set->তথ্য[i] = set->তথ্য[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = (uint8_t)(set->psl[j] - 1);
      set->তথ্য[j] = ফাঁকা;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    }
  }
}

শূন্য Curl_u8_strset_unset(কাঠামো u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, FALSE);
}

শূন্য Curl_u8_strset_unset0(কাঠামো u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, TRUE);
}

CURLcode Curl_u8_strset_copy(কাঠামো u8_strset *dest, কাঠামো u8_strset *src)
{
  CURLcode ফলাফল = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  জন্য(i = 0; !ফলাফল && (i < CURL_U8_STRSET_SLOT_CNT(src)); ++i) {
    যদি(src->তথ্য[i])
      ফলাফল = u8_strset_set(dest, src->ids[i], src->তথ্য[i]);
  }
  ফেরত ফলাফল;
}
