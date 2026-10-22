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
#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_U32_PTRSET_MAGIC 0x75703272
#جے_انت

#تعریف_کرو CURL_U32_PTRSET_DEBUG      0

#جے_بنا_تعریف CURL_وٹاندرا
#تعریف_کرو CURL_وٹاندرا(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#جے_انت

#تعریف_کرو CURL_U32_تھاں_CNT(بٹ)        ((بٹ) ? (1U << (بٹ)) : 0)
#تعریف_کرو CURL_U32_PTRSET_تھاں_CNT(s)    CURL_U32_تھاں_CNT((s)->slotbits)
#تعریف_کرو CURL_U32_PTRSET_تھاں_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
فراغ Curl_u32_ptrset_init(بنية u32_ptrset *سیٹ,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(سیٹ, 0, حجم(*سیٹ));
  سیٹ->dtor = dtor;
#جے_تعریف_شدہ DEBUGBUILD
  سیٹ->init = CURL_U32_PTRSET_MAGIC;
#جے_انت
}

فراغ Curl_u32_ptrset_clear(بنية u32_ptrset *سیٹ)
{
  uint32_t i;
  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  إذا(سیٹ->dtor)
    لكل(i = 0; i < CURL_U32_PTRSET_تھاں_CNT(سیٹ); ++i)
      إذا(سیٹ->انکڑے[i])
        سیٹ->dtor(سیٹ->ids[i], سیٹ->انکڑے[i]);

  curlx_safefree(سیٹ->انکڑے);
  Curl_u32_ptrset_init(سیٹ, سیٹ->dtor);
}

ساكن فراغ u32_ptrset_جوڑو(بنية u32_ptrset *سیٹ, uint32_t id, فراغ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_تھاں_IDX(سیٹ, id);
  uint32_t psl = 0;
  طالما(سیٹ->انکڑے[i]) {
    إذا(psl > سیٹ->psl[i]) { /*وٹاندرا*/
      فراغ *tmpdata;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#جے_انت
      tmpdata = سیٹ->انکڑے[i];
      سیٹ->انکڑے[i] = ptr;
      ptr = tmpdata;
      CURL_وٹاندرا(سیٹ->psl[i], psl);
      CURL_وٹاندرا(سیٹ->ids[i], id);
    }
    i = CURL_U32_PTRSET_تھاں_IDX(سیٹ, i + 1);
    ++psl;
  }
  سیٹ->ids[i] = id;
  سیٹ->انکڑے[i] = ptr;
  سیٹ->psl[i] = psl;
  ++سیٹ->گنتی;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#جے_انت
}

ساكن bool u32_ptrset_grow(بنية u32_ptrset *سیٹ)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  فراغ **prev_انکڑے;
  size_t nslots;
  ثابت size_t تھاں_ناپ = (حجم(فراغ *) + (2 * حجم(uint32_t)));
  فراغ *d;

  إذا(سیٹ->slotbits >= 31)
    أرجع FALSE;
  nslotbits = سیٹ->slotbits ? (uint8_t)(سیٹ->slotbits + 1) : 5;
  nslots = CURL_U32_تھاں_CNT(nslotbits);
  إذا(nslots > (SIZE_MAX / تھاں_ناپ)) /* 32-bit arch may trigger here */
    أرجع FALSE;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                سیٹ->slotbits, nslotbits, nslots);
#جے_انت
  d = curlx_calloc(1, nslots * تھاں_ناپ);
  إذا(!d)
    أرجع FALSE;

  prev_انکڑے = سیٹ->انکڑے;
  prev_ids = سیٹ->ids;
  pslotbits = سیٹ->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wanalyzer-allocation-size"
#جے_انت
  سیٹ->انکڑے = d;
  سیٹ->ids = (uint32_t *)(سیٹ->انکڑے + nslots);
  سیٹ->psl = (uint32_t *)(سیٹ->ids + nslots);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#جے_انت

  سیٹ->slotbits = nslotbits;
  سیٹ->slotmask = (uint32_t)((1U << سیٹ->slotbits) - 1);
  سیٹ->گنتی = 0;
  /* re-add previous entries */
  لكل(i = 0; i < CURL_U32_تھاں_CNT(pslotbits); ++i) {
    إذا(prev_انکڑے[i])
      u32_ptrset_جوڑو(سیٹ, prev_ids[i], prev_انکڑے[i]);
  }
  إذا(prev_انکڑے)
    curlx_free(prev_انکڑے);
  أرجع TRUE;
}

ساكن bool u32_ptrset_get_اشاریہ(بنية u32_ptrset *سیٹ,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_تھاں_IDX(سیٹ, id);
  uint32_t psl = 0;
  إذا(!سیٹ->انکڑے)
    أرجع FALSE;
  طالما(سیٹ->انکڑے[i] && (psl <= سیٹ->psl[i])) {
    إذا(سیٹ->ids[i] == id) {
      *pindex = i;
#إذا CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, سیٹ->انکڑے[i], i);
#جے_انت
      أرجع TRUE;
    }
    i = CURL_U32_PTRSET_تھاں_IDX(سیٹ, i + 1);
    ++psl;
  }
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#جے_انت
  *pindex = 0;
  أرجع FALSE;
}

uint32_t Curl_u32_ptrset_گنتی(بنية u32_ptrset *سیٹ)
{
  أرجع سیٹ->گنتی;
}

فراغ *Curl_u32_ptrset_get(بنية u32_ptrset *سیٹ, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_اشاریہ(سیٹ, id, &i))
    أرجع سیٹ->انکڑے[i];
  أرجع عدم;
}

CURLcode Curl_u32_ptrset_سیٹ(بنية u32_ptrset *سیٹ,
                             uint32_t id, فراغ *ptr)
{
  uint32_t i;

  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  إذا(!ptr) {
    Curl_u32_ptrset_unset(سیٹ, id);
    أرجع CURLE_OK;
  }

  إذا(u32_ptrset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set, replace value */
    إذا(سیٹ->dtor && (سیٹ->انکڑے[i] != ptr))
      سیٹ->dtor(سیٹ->ids[i], سیٹ->انکڑے[i]);
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#جے_انت
    سیٹ->انکڑے[i] = ptr;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((سیٹ->گنتی >= CURL_U32_PTRSET_تھاں_CNT(سیٹ)) &&
     !u32_ptrset_grow(سیٹ)) {
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  u32_ptrset_جوڑو(سیٹ, id, ptr);
  أرجع CURLE_OK;
}

ساكن فراغ u32_ptrset_unset(بنية u32_ptrset *سیٹ, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set */
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#جے_انت
    إذا(سیٹ->dtor)
      سیٹ->dtor(سیٹ->ids[i], سیٹ->انکڑے[i]);
    سیٹ->انکڑے[i] = عدم;
    سیٹ->ids[i] = سیٹ->psl[i] = 0;
    --سیٹ->گنتی;
    j = CURL_U32_PTRSET_تھاں_IDX(سیٹ, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(سیٹ->انکڑے[j] && سیٹ->psl[j]) {
      سیٹ->انکڑے[i] = سیٹ->انکڑے[j];
      سیٹ->ids[i] = سیٹ->ids[j];
      سیٹ->psl[i] = سیٹ->psl[j] - 1;
      سیٹ->انکڑے[j] = عدم;
      سیٹ->ids[j] = سیٹ->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_تھاں_IDX(سیٹ, i + 1);
    }
  }
}

فراغ Curl_u32_ptrset_unset(بنية u32_ptrset *سیٹ, uint32_t id)
{
  u32_ptrset_unset(سیٹ, id);
}

فراغ Curl_u32_ptrset_ویکھو(بنية u32_ptrset *سیٹ,
                            Curl_u32_ptrset_ویکھو_cb *cb,
                            فراغ *user_انکڑے)
{
  uint32_t i;
  لكل(i = 0; i < CURL_U32_PTRSET_تھاں_CNT(سیٹ); ++i) {
    إذا(سیٹ->انکڑے[i] && !cb(سیٹ->ids[i], سیٹ->انکڑے[i], user_انکڑے))
      اكسر;
  }
}
