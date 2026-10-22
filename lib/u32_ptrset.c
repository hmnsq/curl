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
#شامل "curl_setup.h"

#شامل "u32_ptrset.h"

/* random patterns for API verification */
#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_U32_PTRSET_MAGIC 0x75703272
#اگر_ختم

#تعریف_کرو CURL_U32_PTRSET_DEBUG      0

#اگر_غیر_تعریف_شدہ CURL_تبادلہ
#تعریف_کرو CURL_تبادلہ(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#اگر_ختم

#تعریف_کرو CURL_U32_خانہ_CNT(بٹ)        ((بٹ) ? (1U << (بٹ)) : 0)
#تعریف_کرو CURL_U32_PTRSET_خانہ_CNT(s)    CURL_U32_خانہ_CNT((s)->slotbits)
#تعریف_کرو CURL_U32_PTRSET_خانہ_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
خالی Curl_u32_ptrset_init(ساخت u32_ptrset *سیٹ,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(سیٹ, 0, حجم(*سیٹ));
  سیٹ->dtor = dtor;
#اگر_تعریف_شدہ DEBUGBUILD
  سیٹ->init = CURL_U32_PTRSET_MAGIC;
#اگر_ختم
}

خالی Curl_u32_ptrset_clear(ساخت u32_ptrset *سیٹ)
{
  uint32_t i;
  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  اگر(سیٹ->dtor)
    برائے(i = 0; i < CURL_U32_PTRSET_خانہ_CNT(سیٹ); ++i)
      اگر(سیٹ->ڈیٹا[i])
        سیٹ->dtor(سیٹ->ids[i], سیٹ->ڈیٹا[i]);

  curlx_safefree(سیٹ->ڈیٹا);
  Curl_u32_ptrset_init(سیٹ, سیٹ->dtor);
}

ساکن خالی u32_ptrset_جوڑو(ساخت u32_ptrset *سیٹ, uint32_t id, خالی *ptr)
{
  uint32_t i = CURL_U32_PTRSET_خانہ_IDX(سیٹ, id);
  uint32_t psl = 0;
  جب(سیٹ->ڈیٹا[i]) {
    اگر(psl > سیٹ->psl[i]) { /*تبادلہ*/
      خالی *tmpdata;
#اگر CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#اگر_ختم
      tmpdata = سیٹ->ڈیٹا[i];
      سیٹ->ڈیٹا[i] = ptr;
      ptr = tmpdata;
      CURL_تبادلہ(سیٹ->psl[i], psl);
      CURL_تبادلہ(سیٹ->ids[i], id);
    }
    i = CURL_U32_PTRSET_خانہ_IDX(سیٹ, i + 1);
    ++psl;
  }
  سیٹ->ids[i] = id;
  سیٹ->ڈیٹا[i] = ptr;
  سیٹ->psl[i] = psl;
  ++سیٹ->گنتی;
#اگر CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#اگر_ختم
}

ساکن bool u32_ptrset_grow(ساخت u32_ptrset *سیٹ)
{
  uint32_t i, *پچھلا_ids;
  uint8_t nslotbits, pslotbits;
  خالی **پچھلا_ڈیٹا;
  size_t nslots;
  ثابت size_t خانہ_سائز = (حجم(خالی *) + (2 * حجم(uint32_t)));
  خالی *d;

  اگر(سیٹ->slotbits >= 31)
    واپس FALSE;
  nslotbits = سیٹ->slotbits ? (uint8_t)(سیٹ->slotbits + 1) : 5;
  nslots = CURL_U32_خانہ_CNT(nslotbits);
  اگر(nslots > (SIZE_MAX / خانہ_سائز)) /* 32-bit arch may trigger here */
    واپس FALSE;
#اگر CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                سیٹ->slotbits, nslotbits, nslots);
#اگر_ختم
  d = curlx_calloc(1, nslots * خانہ_سائز);
  اگر(!d)
    واپس FALSE;

  پچھلا_ڈیٹا = سیٹ->ڈیٹا;
  پچھلا_ids = سیٹ->ids;
  pslotbits = سیٹ->slotbits;
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wanalyzer-allocation-size"
#اگر_ختم
  سیٹ->ڈیٹا = d;
  سیٹ->ids = (uint32_t *)(سیٹ->ڈیٹا + nslots);
  سیٹ->psl = (uint32_t *)(سیٹ->ids + nslots);
#اگر defined(__GNUC__) && __GNUC__ >= 13
#ہدایت GCC diagnostic نکالو
#اگر_ختم

  سیٹ->slotbits = nslotbits;
  سیٹ->slotmask = (uint32_t)((1U << سیٹ->slotbits) - 1);
  سیٹ->گنتی = 0;
  /* re-add previous entries */
  برائے(i = 0; i < CURL_U32_خانہ_CNT(pslotbits); ++i) {
    اگر(پچھلا_ڈیٹا[i])
      u32_ptrset_جوڑو(سیٹ, پچھلا_ids[i], پچھلا_ڈیٹا[i]);
  }
  اگر(پچھلا_ڈیٹا)
    curlx_free(پچھلا_ڈیٹا);
  واپس TRUE;
}

ساکن bool u32_ptrset_get_اشاریہ(ساخت u32_ptrset *سیٹ,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_خانہ_IDX(سیٹ, id);
  uint32_t psl = 0;
  اگر(!سیٹ->ڈیٹا)
    واپس FALSE;
  جب(سیٹ->ڈیٹا[i] && (psl <= سیٹ->psl[i])) {
    اگر(سیٹ->ids[i] == id) {
      *pindex = i;
#اگر CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, سیٹ->ڈیٹا[i], i);
#اگر_ختم
      واپس TRUE;
    }
    i = CURL_U32_PTRSET_خانہ_IDX(سیٹ, i + 1);
    ++psl;
  }
#اگر CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#اگر_ختم
  *pindex = 0;
  واپس FALSE;
}

uint32_t Curl_u32_ptrset_گنتی(ساخت u32_ptrset *سیٹ)
{
  واپس سیٹ->گنتی;
}

خالی *Curl_u32_ptrset_get(ساخت u32_ptrset *سیٹ, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  اگر(u32_ptrset_get_اشاریہ(سیٹ, id, &i))
    واپس سیٹ->ڈیٹا[i];
  واپس تہی;
}

CURLcode Curl_u32_ptrset_سیٹ(ساخت u32_ptrset *سیٹ,
                             uint32_t id, خالی *ptr)
{
  uint32_t i;

  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  اگر(!ptr) {
    Curl_u32_ptrset_unset(سیٹ, id);
    واپس CURLE_OK;
  }

  اگر(u32_ptrset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set, replace value */
    اگر(سیٹ->dtor && (سیٹ->ڈیٹا[i] != ptr))
      سیٹ->dtor(سیٹ->ids[i], سیٹ->ڈیٹا[i]);
#اگر CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#اگر_ختم
    سیٹ->ڈیٹا[i] = ptr;
    واپس CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  اگر((سیٹ->گنتی >= CURL_U32_PTRSET_خانہ_CNT(سیٹ)) &&
     !u32_ptrset_grow(سیٹ)) {
    واپس CURLE_برآمد_کا_حافظہ;
  }

  u32_ptrset_جوڑو(سیٹ, id, ptr);
  واپس CURLE_OK;
}

ساکن خالی u32_ptrset_unset(ساخت u32_ptrset *سیٹ, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(سیٹ->init == CURL_U32_PTRSET_MAGIC);
  اگر(u32_ptrset_get_اشاریہ(سیٹ, id, &i)) {
    /* `id` is in set */
#اگر CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#اگر_ختم
    اگر(سیٹ->dtor)
      سیٹ->dtor(سیٹ->ids[i], سیٹ->ڈیٹا[i]);
    سیٹ->ڈیٹا[i] = تہی;
    سیٹ->ids[i] = سیٹ->psl[i] = 0;
    --سیٹ->گنتی;
    j = CURL_U32_PTRSET_خانہ_IDX(سیٹ, i + 1);
    /* shift all entries with positive psl "down" */
    جب(سیٹ->ڈیٹا[j] && سیٹ->psl[j]) {
      سیٹ->ڈیٹا[i] = سیٹ->ڈیٹا[j];
      سیٹ->ids[i] = سیٹ->ids[j];
      سیٹ->psl[i] = سیٹ->psl[j] - 1;
      سیٹ->ڈیٹا[j] = تہی;
      سیٹ->ids[j] = سیٹ->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_خانہ_IDX(سیٹ, i + 1);
    }
  }
}

خالی Curl_u32_ptrset_unset(ساخت u32_ptrset *سیٹ, uint32_t id)
{
  u32_ptrset_unset(سیٹ, id);
}

خالی Curl_u32_ptrset_دورہ(ساخت u32_ptrset *سیٹ,
                            Curl_u32_ptrset_دورہ_cb *cb,
                            خالی *user_ڈیٹا)
{
  uint32_t i;
  برائے(i = 0; i < CURL_U32_PTRSET_خانہ_CNT(سیٹ); ++i) {
    اگر(سیٹ->ڈیٹا[i] && !cb(سیٹ->ids[i], سیٹ->ڈیٹا[i], user_ڈیٹا))
      توڑو;
  }
}
