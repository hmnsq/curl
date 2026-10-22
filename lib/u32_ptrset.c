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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"

/* random patterns for API verification */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U32_PTRSET_MAGIC 0x75703272
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U32_PTRSET_DEBUG      0

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_ಅದಲುಬದಲು
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಅದಲುಬದಲು(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U32_ಸ್ಥಾನ_CNT(ಬಿಟ್ಟುಗಳು)        ((ಬಿಟ್ಟುಗಳು) ? (1U << (ಬಿಟ್ಟುಗಳು)) : 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U32_PTRSET_ಸ್ಥಾನ_CNT(s)    CURL_U32_ಸ್ಥಾನ_CNT((s)->slotbits)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U32_PTRSET_ಸ್ಥಾನ_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
ಶೂನ್ಯ Curl_u32_ptrset_init(ರಚನೆ u32_ptrset *ಗಣ,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(ಗಣ, 0, ಗಾತ್ರ(*ಗಣ));
  ಗಣ->dtor = dtor;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಗಣ->init = CURL_U32_PTRSET_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Curl_u32_ptrset_clear(ರಚನೆ u32_ptrset *ಗಣ)
{
  uint32_t i;
  DEBUGASSERT(ಗಣ->init == CURL_U32_PTRSET_MAGIC);
  ಆದರೆ(ಗಣ->dtor)
    ಗಾಗಿ(i = 0; i < CURL_U32_PTRSET_ಸ್ಥಾನ_CNT(ಗಣ); ++i)
      ಆದರೆ(ಗಣ->ದತ್ತಾಂಶ[i])
        ಗಣ->dtor(ಗಣ->ids[i], ಗಣ->ದತ್ತಾಂಶ[i]);

  curlx_safefree(ಗಣ->ದತ್ತಾಂಶ);
  Curl_u32_ptrset_init(ಗಣ, ಗಣ->dtor);
}

ಸ್ಥಿರ ಶೂನ್ಯ u32_ptrset_ಕೂಡಿಸು(ರಚನೆ u32_ptrset *ಗಣ, uint32_t id, ಶೂನ್ಯ *ptr)
{
  uint32_t i = CURL_U32_PTRSET_ಸ್ಥಾನ_IDX(ಗಣ, id);
  uint32_t psl = 0;
  ಆಗ(ಗಣ->ದತ್ತಾಂಶ[i]) {
    ಆದರೆ(psl > ಗಣ->psl[i]) { /*ಅದಲುಬದಲು*/
      ಶೂನ್ಯ *tmpdata;
#ಆದರೆ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#ಆದರೆ_ಅಂತ್ಯ
      tmpdata = ಗಣ->ದತ್ತಾಂಶ[i];
      ಗಣ->ದತ್ತಾಂಶ[i] = ptr;
      ptr = tmpdata;
      CURL_ಅದಲುಬದಲು(ಗಣ->psl[i], psl);
      CURL_ಅದಲುಬದಲು(ಗಣ->ids[i], id);
    }
    i = CURL_U32_PTRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    ++psl;
  }
  ಗಣ->ids[i] = id;
  ಗಣ->ದತ್ತಾಂಶ[i] = ptr;
  ಗಣ->psl[i] = psl;
  ++ಗಣ->ಎಣಿಕೆ_2;
#ಆದರೆ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ bool u32_ptrset_grow(ರಚನೆ u32_ptrset *ಗಣ)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  ಶೂನ್ಯ **prev_ದತ್ತಾಂಶ;
  size_t nslots;
  ಸ್ಥಿರಾಂಕ size_t ಸ್ಥಾನ_ಗಾತ್ರ = (ಗಾತ್ರ(ಶೂನ್ಯ *) + (2 * ಗಾತ್ರ(uint32_t)));
  ಶೂನ್ಯ *d;

  ಆದರೆ(ಗಣ->slotbits >= 31)
    ಹಿಂತಿರುಗಿ FALSE;
  nslotbits = ಗಣ->slotbits ? (uint8_t)(ಗಣ->slotbits + 1) : 5;
  nslots = CURL_U32_ಸ್ಥಾನ_CNT(nslotbits);
  ಆದರೆ(nslots > (SIZE_MAX / ಸ್ಥಾನ_ಗಾತ್ರ)) /* 32-bit arch may trigger here */
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                ಗಣ->slotbits, nslotbits, nslots);
#ಆದರೆ_ಅಂತ್ಯ
  d = curlx_calloc(1, nslots * ಸ್ಥಾನ_ಗಾತ್ರ);
  ಆದರೆ(!d)
    ಹಿಂತಿರುಗಿ FALSE;

  prev_ದತ್ತಾಂಶ = ಗಣ->ದತ್ತಾಂಶ;
  prev_ids = ಗಣ->ids;
  pslotbits = ಗಣ->slotbits;
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wanalyzer-allocation-size"
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->ದತ್ತಾಂಶ = d;
  ಗಣ->ids = (uint32_t *)(ಗಣ->ದತ್ತಾಂಶ + nslots);
  ಗಣ->psl = (uint32_t *)(ಗಣ->ids + nslots);
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

  ಗಣ->slotbits = nslotbits;
  ಗಣ->slotmask = (uint32_t)((1U << ಗಣ->slotbits) - 1);
  ಗಣ->ಎಣಿಕೆ_2 = 0;
  /* re-add previous entries */
  ಗಾಗಿ(i = 0; i < CURL_U32_ಸ್ಥಾನ_CNT(pslotbits); ++i) {
    ಆದರೆ(prev_ದತ್ತಾಂಶ[i])
      u32_ptrset_ಕೂಡಿಸು(ಗಣ, prev_ids[i], prev_ದತ್ತಾಂಶ[i]);
  }
  ಆದರೆ(prev_ದತ್ತಾಂಶ)
    curlx_free(prev_ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool u32_ptrset_get_ಸೂಚ್ಯಂಕ(ರಚನೆ u32_ptrset *ಗಣ,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_ಸ್ಥಾನ_IDX(ಗಣ, id);
  uint32_t psl = 0;
  ಆದರೆ(!ಗಣ->ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆಗ(ಗಣ->ದತ್ತಾಂಶ[i] && (psl <= ಗಣ->psl[i])) {
    ಆದರೆ(ಗಣ->ids[i] == id) {
      *pindex = i;
#ಆದರೆ CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, ಗಣ->ದತ್ತಾಂಶ[i], i);
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ TRUE;
    }
    i = CURL_U32_PTRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    ++psl;
  }
#ಆದರೆ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#ಆದರೆ_ಅಂತ್ಯ
  *pindex = 0;
  ಹಿಂತಿರುಗಿ FALSE;
}

uint32_t Curl_u32_ptrset_ಎಣಿಕೆ(ರಚನೆ u32_ptrset *ಗಣ)
{
  ಹಿಂತಿರುಗಿ ಗಣ->ಎಣಿಕೆ_2;
}

ಶೂನ್ಯ *Curl_u32_ptrset_get(ರಚನೆ u32_ptrset *ಗಣ, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(ಗಣ->init == CURL_U32_PTRSET_MAGIC);
  ಆದರೆ(u32_ptrset_get_ಸೂಚ್ಯಂಕ(ಗಣ, id, &i))
    ಹಿಂತಿರುಗಿ ಗಣ->ದತ್ತಾಂಶ[i];
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURLcode Curl_u32_ptrset_ಗಣ(ರಚನೆ u32_ptrset *ಗಣ,
                             uint32_t id, ಶೂನ್ಯ *ptr)
{
  uint32_t i;

  DEBUGASSERT(ಗಣ->init == CURL_U32_PTRSET_MAGIC);
  ಆದರೆ(!ptr) {
    Curl_u32_ptrset_unset(ಗಣ, id);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(u32_ptrset_get_ಸೂಚ್ಯಂಕ(ಗಣ, id, &i)) {
    /* `id` is in set, replace value */
    ಆದರೆ(ಗಣ->dtor && (ಗಣ->ದತ್ತಾಂಶ[i] != ptr))
      ಗಣ->dtor(ಗಣ->ids[i], ಗಣ->ದತ್ತಾಂಶ[i]);
#ಆದರೆ CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ->ದತ್ತಾಂಶ[i] = ptr;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ಆದರೆ((ಗಣ->ಎಣಿಕೆ_2 >= CURL_U32_PTRSET_ಸ್ಥಾನ_CNT(ಗಣ)) &&
     !u32_ptrset_grow(ಗಣ)) {
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  u32_ptrset_ಕೂಡಿಸು(ಗಣ, id, ptr);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ u32_ptrset_unset(ರಚನೆ u32_ptrset *ಗಣ, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(ಗಣ->init == CURL_U32_PTRSET_MAGIC);
  ಆದರೆ(u32_ptrset_get_ಸೂಚ್ಯಂಕ(ಗಣ, id, &i)) {
    /* `id` is in set */
#ಆದರೆ CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ಗಣ->dtor)
      ಗಣ->dtor(ಗಣ->ids[i], ಗಣ->ದತ್ತಾಂಶ[i]);
    ಗಣ->ದತ್ತಾಂಶ[i] = ಖಾಲಿ;
    ಗಣ->ids[i] = ಗಣ->psl[i] = 0;
    --ಗಣ->ಎಣಿಕೆ_2;
    j = CURL_U32_PTRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    /* shift all entries with positive psl "down" */
    ಆಗ(ಗಣ->ದತ್ತಾಂಶ[j] && ಗಣ->psl[j]) {
      ಗಣ->ದತ್ತಾಂಶ[i] = ಗಣ->ದತ್ತಾಂಶ[j];
      ಗಣ->ids[i] = ಗಣ->ids[j];
      ಗಣ->psl[i] = ಗಣ->psl[j] - 1;
      ಗಣ->ದತ್ತಾಂಶ[j] = ಖಾಲಿ;
      ಗಣ->ids[j] = ಗಣ->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    }
  }
}

ಶೂನ್ಯ Curl_u32_ptrset_unset(ರಚನೆ u32_ptrset *ಗಣ, uint32_t id)
{
  u32_ptrset_unset(ಗಣ, id);
}

ಶೂನ್ಯ Curl_u32_ptrset_ಭೇಟಿ(ರಚನೆ u32_ptrset *ಗಣ,
                            Curl_u32_ptrset_ಭೇಟಿ_cb *cb,
                            ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  uint32_t i;
  ಗಾಗಿ(i = 0; i < CURL_U32_PTRSET_ಸ್ಥಾನ_CNT(ಗಣ); ++i) {
    ಆದರೆ(ಗಣ->ದತ್ತಾಂಶ[i] && !cb(ಗಣ->ids[i], ಗಣ->ದತ್ತಾಂಶ[i], user_ದತ್ತಾಂಶ))
      ನಿಲ್ಲಿಸು;
  }
}
