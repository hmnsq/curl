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
#اگر_تعریف_شده DEBUGBUILD
#تعریف_کن CURL_U32_PTRSET_MAGIC 0x75703272
#پایان_اگر

#تعریف_کن CURL_U32_PTRSET_DEBUG      0

#اگر_تعریف_نشده CURL_جابجا
#تعریف_کن CURL_جابجا(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#پایان_اگر

#تعریف_کن CURL_U32_جایگاه_CNT(بیتها)        ((بیتها) ? (1U << (بیتها)) : 0)
#تعریف_کن CURL_U32_PTRSET_جایگاه_CNT(s)    CURL_U32_جایگاه_CNT((s)->slotbits)
#تعریف_کن CURL_U32_PTRSET_جایگاه_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
فراغ Curl_u32_ptrset_init(بنية u32_ptrset *مجموعه,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(مجموعه, 0, حجم(*مجموعه));
  مجموعه->dtor = dtor;
#اگر_تعریف_شده DEBUGBUILD
  مجموعه->init = CURL_U32_PTRSET_MAGIC;
#پایان_اگر
}

فراغ Curl_u32_ptrset_clear(بنية u32_ptrset *مجموعه)
{
  uint32_t i;
  DEBUGASSERT(مجموعه->init == CURL_U32_PTRSET_MAGIC);
  إذا(مجموعه->dtor)
    لكل(i = 0; i < CURL_U32_PTRSET_جایگاه_CNT(مجموعه); ++i)
      إذا(مجموعه->داده[i])
        مجموعه->dtor(مجموعه->ids[i], مجموعه->داده[i]);

  curlx_safefree(مجموعه->داده);
  Curl_u32_ptrset_init(مجموعه, مجموعه->dtor);
}

ساكن فراغ u32_ptrset_جمع_کن(بنية u32_ptrset *مجموعه, uint32_t id, فراغ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_جایگاه_IDX(مجموعه, id);
  uint32_t psl = 0;
  طالما(مجموعه->داده[i]) {
    إذا(psl > مجموعه->psl[i]) { /*جابجا*/
      فراغ *tmpdata;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#پایان_اگر
      tmpdata = مجموعه->داده[i];
      مجموعه->داده[i] = ptr;
      ptr = tmpdata;
      CURL_جابجا(مجموعه->psl[i], psl);
      CURL_جابجا(مجموعه->ids[i], id);
    }
    i = CURL_U32_PTRSET_جایگاه_IDX(مجموعه, i + 1);
    ++psl;
  }
  مجموعه->ids[i] = id;
  مجموعه->داده[i] = ptr;
  مجموعه->psl[i] = psl;
  ++مجموعه->شمار;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#پایان_اگر
}

ساكن bool u32_ptrset_grow(بنية u32_ptrset *مجموعه)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  فراغ **prev_داده;
  size_t nslots;
  ثابت size_t جایگاه_اندازه = (حجم(فراغ *) + (2 * حجم(uint32_t)));
  فراغ *d;

  إذا(مجموعه->slotbits >= 31)
    أرجع FALSE;
  nslotbits = مجموعه->slotbits ? (uint8_t)(مجموعه->slotbits + 1) : 5;
  nslots = CURL_U32_جایگاه_CNT(nslotbits);
  إذا(nslots > (SIZE_MAX / جایگاه_اندازه)) /* 32-bit arch may trigger here */
    أرجع FALSE;
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                مجموعه->slotbits, nslotbits, nslots);
#پایان_اگر
  d = curlx_calloc(1, nslots * جایگاه_اندازه);
  إذا(!d)
    أرجع FALSE;

  prev_داده = مجموعه->داده;
  prev_ids = مجموعه->ids;
  pslotbits = مجموعه->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wanalyzer-allocation-size"
#پایان_اگر
  مجموعه->داده = d;
  مجموعه->ids = (uint32_t *)(مجموعه->داده + nslots);
  مجموعه->psl = (uint32_t *)(مجموعه->ids + nslots);
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic برداشت
#پایان_اگر

  مجموعه->slotbits = nslotbits;
  مجموعه->slotmask = (uint32_t)((1U << مجموعه->slotbits) - 1);
  مجموعه->شمار = 0;
  /* re-add previous entries */
  لكل(i = 0; i < CURL_U32_جایگاه_CNT(pslotbits); ++i) {
    إذا(prev_داده[i])
      u32_ptrset_جمع_کن(مجموعه, prev_ids[i], prev_داده[i]);
  }
  إذا(prev_داده)
    curlx_free(prev_داده);
  أرجع TRUE;
}

ساكن bool u32_ptrset_get_شاخص(بنية u32_ptrset *مجموعه,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_جایگاه_IDX(مجموعه, id);
  uint32_t psl = 0;
  إذا(!مجموعه->داده)
    أرجع FALSE;
  طالما(مجموعه->داده[i] && (psl <= مجموعه->psl[i])) {
    إذا(مجموعه->ids[i] == id) {
      *pindex = i;
#إذا CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, مجموعه->داده[i], i);
#پایان_اگر
      أرجع TRUE;
    }
    i = CURL_U32_PTRSET_جایگاه_IDX(مجموعه, i + 1);
    ++psl;
  }
#إذا CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#پایان_اگر
  *pindex = 0;
  أرجع FALSE;
}

uint32_t Curl_u32_ptrset_شمار(بنية u32_ptrset *مجموعه)
{
  أرجع مجموعه->شمار;
}

فراغ *Curl_u32_ptrset_get(بنية u32_ptrset *مجموعه, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(مجموعه->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_شاخص(مجموعه, id, &i))
    أرجع مجموعه->داده[i];
  أرجع عدم;
}

CURLcode Curl_u32_ptrset_مجموعه(بنية u32_ptrset *مجموعه,
                             uint32_t id, فراغ *ptr)
{
  uint32_t i;

  DEBUGASSERT(مجموعه->init == CURL_U32_PTRSET_MAGIC);
  إذا(!ptr) {
    Curl_u32_ptrset_unset(مجموعه, id);
    أرجع CURLE_OK;
  }

  إذا(u32_ptrset_get_شاخص(مجموعه, id, &i)) {
    /* `id` is in set, replace value */
    إذا(مجموعه->dtor && (مجموعه->داده[i] != ptr))
      مجموعه->dtor(مجموعه->ids[i], مجموعه->داده[i]);
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#پایان_اگر
    مجموعه->داده[i] = ptr;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((مجموعه->شمار >= CURL_U32_PTRSET_جایگاه_CNT(مجموعه)) &&
     !u32_ptrset_grow(مجموعه)) {
    أرجع CURLE_بیرون_از_حافظه;
  }

  u32_ptrset_جمع_کن(مجموعه, id, ptr);
  أرجع CURLE_OK;
}

ساكن فراغ u32_ptrset_unset(بنية u32_ptrset *مجموعه, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(مجموعه->init == CURL_U32_PTRSET_MAGIC);
  إذا(u32_ptrset_get_شاخص(مجموعه, id, &i)) {
    /* `id` is in set */
#إذا CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#پایان_اگر
    إذا(مجموعه->dtor)
      مجموعه->dtor(مجموعه->ids[i], مجموعه->داده[i]);
    مجموعه->داده[i] = عدم;
    مجموعه->ids[i] = مجموعه->psl[i] = 0;
    --مجموعه->شمار;
    j = CURL_U32_PTRSET_جایگاه_IDX(مجموعه, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(مجموعه->داده[j] && مجموعه->psl[j]) {
      مجموعه->داده[i] = مجموعه->داده[j];
      مجموعه->ids[i] = مجموعه->ids[j];
      مجموعه->psl[i] = مجموعه->psl[j] - 1;
      مجموعه->داده[j] = عدم;
      مجموعه->ids[j] = مجموعه->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_جایگاه_IDX(مجموعه, i + 1);
    }
  }
}

فراغ Curl_u32_ptrset_unset(بنية u32_ptrset *مجموعه, uint32_t id)
{
  u32_ptrset_unset(مجموعه, id);
}

فراغ Curl_u32_ptrset_بازدید(بنية u32_ptrset *مجموعه,
                            Curl_u32_ptrset_بازدید_cb *cb,
                            فراغ *user_داده)
{
  uint32_t i;
  لكل(i = 0; i < CURL_U32_PTRSET_جایگاه_CNT(مجموعه); ++i) {
    إذا(مجموعه->داده[i] && !cb(مجموعه->ids[i], مجموعه->داده[i], user_داده))
      اكسر;
  }
}
