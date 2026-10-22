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

#ضمن "u8_strset.h"
#ضمن "curlx/strdup.h"

/* random patterns for API verification */
#اگر_تعریف_شده DEBUGBUILD
#تعریف_کن CURL_U8_STRSET_MAGIC 0x7117e783
#پایان_اگر

#تعریف_کن CURL_U8_STRSET_DEBUG      0

#تعریف_کن CURL_جابجا(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#تعریف_کن CURL_U8_STRSET_جایگاه_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#تعریف_کن CURL_U8_جایگاه_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#تعریف_کن CURL_U8_جایگاه_ماسک(i)             (uint8_t)((1U << (i)) - 1)
#تعریف_کن CURL_U8_STRSET_جایگاه_CNT(s)       CURL_U8_جایگاه_CNT((s)->slotbits)

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
فراغ Curl_u8_strset_init(بنية u8_strset *مجموعه)
{
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Warray-bounds"
#پایان_اگر
  memset(مجموعه, 0, حجم(*مجموعه));
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic برداشت
#پایان_اگر
  مجموعه->داده = مجموعه->sdata;
  مجموعه->ids = مجموعه->sids;
  مجموعه->psl = مجموعه->spsl;
  مجموعه->slotbits = CURL_U8_STRSET_شروع_بیتها;
  مجموعه->slotmask = CURL_U8_جایگاه_ماسک(مجموعه->slotbits);
  مجموعه->شمار = 0;
#اگر_تعریف_شده DEBUGBUILD
  مجموعه->init = CURL_U8_STRSET_MAGIC;
#پایان_اگر
}

فراغ Curl_u8_strset_clear(بنية u8_strset *مجموعه)
{
  uint16_t i;
  DEBUGASSERT(مجموعه->init == CURL_U8_STRSET_MAGIC);
  لكل(i = 0; i < CURL_U8_STRSET_جایگاه_CNT(مجموعه); ++i)
    curlx_safefree(مجموعه->داده[i]);

  إذا(مجموعه->داده != مجموعه->sdata)
    curlx_safefree(مجموعه->داده);
  Curl_u8_strset_init(مجموعه);
}

ساكن فراغ u8_strset_addn(بنية u8_strset *مجموعه, uint8_t id, حرف *val)
{
  uint8_t i = CURL_U8_STRSET_جایگاه_IDX(مجموعه, id);
  uint8_t psl = 0;
  طالما(مجموعه->داده[i]) {
    إذا(psl > مجموعه->psl[i]) { /*جابجا*/
      حرف *tmpdata;
      tmpdata = مجموعه->داده[i];
      مجموعه->داده[i] = val;
      val = tmpdata;
      CURL_جابجا(مجموعه->psl[i], psl);
      CURL_جابجا(مجموعه->ids[i], id);
    }
    i = CURL_U8_STRSET_جایگاه_IDX(مجموعه, i + 1);
    ++psl;
  }
  مجموعه->ids[i] = id;
  مجموعه->داده[i] = val;
  مجموعه->psl[i] = psl;
  ++مجموعه->شمار;
}

ساكن bool u8_strset_grow(بنية u8_strset *مجموعه)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  حرف **prev_داده;
  size_t nslots;
  فراغ *d;

  إذا(مجموعه->slotbits >= 8)
    أرجع FALSE;
  nslotbits = (uint8_t)(مجموعه->slotbits + 1);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                مجموعه->slotbits, nslotbits);
#پایان_اگر
  nslots = CURL_U8_جایگاه_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * حجم(حرف *)) + (2 * nslots));
  إذا(!d)
    أرجع FALSE;

  prev_داده = مجموعه->داده;
  prev_ids = مجموعه->ids;
  prev_slots = مجموعه->slotbits;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wanalyzer-allocation-size"
#پایان_اگر
  مجموعه->داده = (حرف **)d;
#إذا defined(__GNUC__) && __GNUC__ >= 13
#رهنمود GCC diagnostic برداشت
#پایان_اگر
  مجموعه->ids = (uint8_t *)d + (nslots * حجم(حرف *));
  مجموعه->psl = مجموعه->ids + nslots;
  مجموعه->slotbits = nslotbits;
  مجموعه->slotmask = CURL_U8_جایگاه_ماسک(مجموعه->slotbits);
  مجموعه->شمار = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    لكل(i = 0; i < CURL_U8_جایگاه_CNT(prev_slots); ++i) {
      إذا(prev_داده[i])
        u8_strset_addn(مجموعه, prev_ids[i], prev_داده[i]);
    }
  }
  إذا(prev_داده != مجموعه->sdata)
    curlx_free(prev_داده);
  أرجع TRUE;
}

ساكن bool u8_strset_get_شاخص(بنية u8_strset *مجموعه,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_جایگاه_IDX(مجموعه, id);
  uint8_t psl = 0;
  طالما(مجموعه->داده[i] && (psl <= مجموعه->psl[i])) {
    إذا(مجموعه->ids[i] == id) {
      *pindex = i;
#إذا CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, مجموعه->داده[i]);
#پایان_اگر
      أرجع TRUE;
    }
    i = CURL_U8_STRSET_جایگاه_IDX(مجموعه, i + 1);
    ++psl;
  }
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#پایان_اگر
  *pindex = 0;
  أرجع FALSE;
}

#اگر_تعریف_شده UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_شمار(بنية u8_strset *مجموعه)
{
  أرجع مجموعه->شمار;
}
#پایان_اگر

ثابت حرف *Curl_u8_strset_get(بنية u8_strset *مجموعه, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(مجموعه->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_شاخص(مجموعه, id, &i))
    أرجع مجموعه->داده[i];
  أرجع عدم;
}

CURLcode Curl_u8_strset_setn(بنية u8_strset *مجموعه,
                             uint8_t id, حرف *str)
{
  uint8_t i;

  DEBUGASSERT(مجموعه->init == CURL_U8_STRSET_MAGIC);
#إذا CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#پایان_اگر
  إذا(!str) {
    Curl_u8_strset_unset(مجموعه, id);
    أرجع CURLE_OK;
  }

  إذا(u8_strset_get_شاخص(مجموعه, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(مجموعه->داده[i]);
    مجموعه->داده[i] = str;
    أرجع CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  إذا((مجموعه->شمار >= CURL_U8_STRSET_جایگاه_CNT(مجموعه)) && !u8_strset_grow(مجموعه)) {
    curlx_free(str);
    أرجع CURLE_بیرون_از_حافظه;
  }

  u8_strset_addn(مجموعه, id, str);
  أرجع CURLE_OK;
}

CURLcode Curl_u8_strset_setx(بنية u8_strset *مجموعه,
                             uint8_t id, ثابت حرف *str, size_t slen)
{
  حرف *val;

  DEBUGASSERT(مجموعه->init == CURL_U8_STRSET_MAGIC);
  إذا(!str) {
    Curl_u8_strset_unset(مجموعه, id);
    أرجع CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  إذا(!val)
    أرجع CURLE_بیرون_از_حافظه;
  أرجع Curl_u8_strset_setn(مجموعه, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_مجموعه(بنية u8_strset *مجموعه,
                                uint8_t id, ثابت حرف *str);
UNITTEST CURLcode u8_strset_مجموعه(بنية u8_strset *مجموعه,
                                uint8_t id, ثابت حرف *str)
{
  أرجع Curl_u8_strset_setx(مجموعه, id, str, str ? طول_رشته(str) : 0);
}

ساكن فراغ u8_strset_unset(بنية u8_strset *مجموعه, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(مجموعه->init == CURL_U8_STRSET_MAGIC);
  إذا(u8_strset_get_شاخص(مجموعه, id, &i)) {
    /* `id` is in set */
    إذا(zero)
      curlx_strzero(مجموعه->داده[i]);
    curlx_safefree(مجموعه->داده[i]);
    مجموعه->ids[i] = مجموعه->psl[i] = 0;
    --مجموعه->شمار;
    j = CURL_U8_STRSET_جایگاه_IDX(مجموعه, i + 1);
    /* shift all entries with positive psl "down" */
    طالما(مجموعه->داده[j] && مجموعه->psl[j]) {
      مجموعه->داده[i] = مجموعه->داده[j];
      مجموعه->ids[i] = مجموعه->ids[j];
      مجموعه->psl[i] = (uint8_t)(مجموعه->psl[j] - 1);
      مجموعه->داده[j] = عدم;
      مجموعه->ids[j] = مجموعه->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_جایگاه_IDX(مجموعه, i + 1);
    }
  }
}

فراغ Curl_u8_strset_unset(بنية u8_strset *مجموعه, uint8_t id)
{
  u8_strset_unset(مجموعه, id, FALSE);
}

فراغ Curl_u8_strset_unset0(بنية u8_strset *مجموعه, uint8_t id)
{
  u8_strset_unset(مجموعه, id, TRUE);
}

CURLcode Curl_u8_strset_copy(بنية u8_strset *dest, بنية u8_strset *src)
{
  CURLcode نتیجه = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  لكل(i = 0; !نتیجه && (i < CURL_U8_STRSET_جایگاه_CNT(src)); ++i) {
    إذا(src->داده[i])
      نتیجه = u8_strset_مجموعه(dest, src->ids[i], src->داده[i]);
  }
  أرجع نتیجه;
}
