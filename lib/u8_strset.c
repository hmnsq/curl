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

#ಅನ್ತರ್ಭಾವ "u8_strset.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

/* random patterns for API verification */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U8_STRSET_MAGIC 0x7117e783
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U8_STRSET_DEBUG      0

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಅದಲುಬದಲು(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U8_STRSET_ಸ್ಥಾನ_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U8_ಸ್ಥಾನ_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U8_ಸ್ಥಾನ_ಮುಖವಾಡ(i)             (uint8_t)((1U << (i)) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_U8_STRSET_ಸ್ಥಾನ_CNT(s)       CURL_U8_ಸ್ಥಾನ_CNT((s)->slotbits)

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
ಶೂನ್ಯ Curl_u8_strset_init(ರಚನೆ u8_strset *ಗಣ)
{
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Warray-bounds"
#ಆದರೆ_ಅಂತ್ಯ
  memset(ಗಣ, 0, ಗಾತ್ರ(*ಗಣ));
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->ದತ್ತಾಂಶ = ಗಣ->sdata;
  ಗಣ->ids = ಗಣ->sids;
  ಗಣ->psl = ಗಣ->spsl;
  ಗಣ->slotbits = CURL_U8_STRSET_ಆರಂಭ_ಬಿಟ್ಟುಗಳು;
  ಗಣ->slotmask = CURL_U8_ಸ್ಥಾನ_ಮುಖವಾಡ(ಗಣ->slotbits);
  ಗಣ->ಎಣಿಕೆ_2 = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಗಣ->init = CURL_U8_STRSET_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Curl_u8_strset_clear(ರಚನೆ u8_strset *ಗಣ)
{
  uint16_t i;
  DEBUGASSERT(ಗಣ->init == CURL_U8_STRSET_MAGIC);
  ಗಾಗಿ(i = 0; i < CURL_U8_STRSET_ಸ್ಥಾನ_CNT(ಗಣ); ++i)
    curlx_safefree(ಗಣ->ದತ್ತಾಂಶ[i]);

  ಆದರೆ(ಗಣ->ದತ್ತಾಂಶ != ಗಣ->sdata)
    curlx_safefree(ಗಣ->ದತ್ತಾಂಶ);
  Curl_u8_strset_init(ಗಣ);
}

ಸ್ಥಿರ ಶೂನ್ಯ u8_strset_addn(ರಚನೆ u8_strset *ಗಣ, uint8_t id, ಅಕ್ಷರ *val)
{
  uint8_t i = CURL_U8_STRSET_ಸ್ಥಾನ_IDX(ಗಣ, id);
  uint8_t psl = 0;
  ಆಗ(ಗಣ->ದತ್ತಾಂಶ[i]) {
    ಆದರೆ(psl > ಗಣ->psl[i]) { /*ಅದಲುಬದಲು*/
      ಅಕ್ಷರ *tmpdata;
      tmpdata = ಗಣ->ದತ್ತಾಂಶ[i];
      ಗಣ->ದತ್ತಾಂಶ[i] = val;
      val = tmpdata;
      CURL_ಅದಲುಬದಲು(ಗಣ->psl[i], psl);
      CURL_ಅದಲುಬದಲು(ಗಣ->ids[i], id);
    }
    i = CURL_U8_STRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    ++psl;
  }
  ಗಣ->ids[i] = id;
  ಗಣ->ದತ್ತಾಂಶ[i] = val;
  ಗಣ->psl[i] = psl;
  ++ಗಣ->ಎಣಿಕೆ_2;
}

ಸ್ಥಿರ bool u8_strset_grow(ರಚನೆ u8_strset *ಗಣ)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  ಅಕ್ಷರ **prev_ದತ್ತಾಂಶ;
  size_t nslots;
  ಶೂನ್ಯ *d;

  ಆದರೆ(ಗಣ->slotbits >= 8)
    ಹಿಂತಿರುಗಿ FALSE;
  nslotbits = (uint8_t)(ಗಣ->slotbits + 1);
#ಆದರೆ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                ಗಣ->slotbits, nslotbits);
#ಆದರೆ_ಅಂತ್ಯ
  nslots = CURL_U8_ಸ್ಥಾನ_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * ಗಾತ್ರ(ಅಕ್ಷರ *)) + (2 * nslots));
  ಆದರೆ(!d)
    ಹಿಂತಿರುಗಿ FALSE;

  prev_ದತ್ತಾಂಶ = ಗಣ->ದತ್ತಾಂಶ;
  prev_ids = ಗಣ->ids;
  prev_slots = ಗಣ->slotbits;
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wanalyzer-allocation-size"
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->ದತ್ತಾಂಶ = (ಅಕ್ಷರ **)d;
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 13
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->ids = (uint8_t *)d + (nslots * ಗಾತ್ರ(ಅಕ್ಷರ *));
  ಗಣ->psl = ಗಣ->ids + nslots;
  ಗಣ->slotbits = nslotbits;
  ಗಣ->slotmask = CURL_U8_ಸ್ಥಾನ_ಮುಖವಾಡ(ಗಣ->slotbits);
  ಗಣ->ಎಣಿಕೆ_2 = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    ಗಾಗಿ(i = 0; i < CURL_U8_ಸ್ಥಾನ_CNT(prev_slots); ++i) {
      ಆದರೆ(prev_ದತ್ತಾಂಶ[i])
        u8_strset_addn(ಗಣ, prev_ids[i], prev_ದತ್ತಾಂಶ[i]);
    }
  }
  ಆದರೆ(prev_ದತ್ತಾಂಶ != ಗಣ->sdata)
    curlx_free(prev_ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool u8_strset_get_ಸೂಚ್ಯಂಕ(ರಚನೆ u8_strset *ಗಣ,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_ಸ್ಥಾನ_IDX(ಗಣ, id);
  uint8_t psl = 0;
  ಆಗ(ಗಣ->ದತ್ತಾಂಶ[i] && (psl <= ಗಣ->psl[i])) {
    ಆದರೆ(ಗಣ->ids[i] == id) {
      *pindex = i;
#ಆದರೆ CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, ಗಣ->ದತ್ತಾಂಶ[i]);
#ಆದರೆ_ಅಂತ್ಯ
      ಹಿಂತಿರುಗಿ TRUE;
    }
    i = CURL_U8_STRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    ++psl;
  }
#ಆದರೆ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#ಆದರೆ_ಅಂತ್ಯ
  *pindex = 0;
  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_ಎಣಿಕೆ(ರಚನೆ u8_strset *ಗಣ)
{
  ಹಿಂತಿರುಗಿ ಗಣ->ಎಣಿಕೆ_2;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_u8_strset_get(ರಚನೆ u8_strset *ಗಣ, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(ಗಣ->init == CURL_U8_STRSET_MAGIC);
  ಆದರೆ(u8_strset_get_ಸೂಚ್ಯಂಕ(ಗಣ, id, &i))
    ಹಿಂತಿರುಗಿ ಗಣ->ದತ್ತಾಂಶ[i];
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURLcode Curl_u8_strset_setn(ರಚನೆ u8_strset *ಗಣ,
                             uint8_t id, ಅಕ್ಷರ *str)
{
  uint8_t i;

  DEBUGASSERT(ಗಣ->init == CURL_U8_STRSET_MAGIC);
#ಆದರೆ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!str) {
    Curl_u8_strset_unset(ಗಣ, id);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(u8_strset_get_ಸೂಚ್ಯಂಕ(ಗಣ, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(ಗಣ->ದತ್ತಾಂಶ[i]);
    ಗಣ->ದತ್ತಾಂಶ[i] = str;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ಆದರೆ((ಗಣ->ಎಣಿಕೆ_2 >= CURL_U8_STRSET_ಸ್ಥಾನ_CNT(ಗಣ)) && !u8_strset_grow(ಗಣ)) {
    curlx_free(str);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  u8_strset_addn(ಗಣ, id, str);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_u8_strset_setx(ರಚನೆ u8_strset *ಗಣ,
                             uint8_t id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t slen)
{
  ಅಕ್ಷರ *val;

  DEBUGASSERT(ಗಣ->init == CURL_U8_STRSET_MAGIC);
  ಆದರೆ(!str) {
    Curl_u8_strset_unset(ಗಣ, id);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  ಆದರೆ(!val)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಹಿಂತಿರುಗಿ Curl_u8_strset_setn(ಗಣ, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_ಗಣ(ರಚನೆ u8_strset *ಗಣ,
                                uint8_t id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str);
UNITTEST CURLcode u8_strset_ಗಣ(ರಚನೆ u8_strset *ಗಣ,
                                uint8_t id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
  ಹಿಂತಿರುಗಿ Curl_u8_strset_setx(ಗಣ, id, str, str ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str) : 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ u8_strset_unset(ರಚನೆ u8_strset *ಗಣ, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(ಗಣ->init == CURL_U8_STRSET_MAGIC);
  ಆದರೆ(u8_strset_get_ಸೂಚ್ಯಂಕ(ಗಣ, id, &i)) {
    /* `id` is in set */
    ಆದರೆ(zero)
      curlx_strzero(ಗಣ->ದತ್ತಾಂಶ[i]);
    curlx_safefree(ಗಣ->ದತ್ತಾಂಶ[i]);
    ಗಣ->ids[i] = ಗಣ->psl[i] = 0;
    --ಗಣ->ಎಣಿಕೆ_2;
    j = CURL_U8_STRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    /* shift all entries with positive psl "down" */
    ಆಗ(ಗಣ->ದತ್ತಾಂಶ[j] && ಗಣ->psl[j]) {
      ಗಣ->ದತ್ತಾಂಶ[i] = ಗಣ->ದತ್ತಾಂಶ[j];
      ಗಣ->ids[i] = ಗಣ->ids[j];
      ಗಣ->psl[i] = (uint8_t)(ಗಣ->psl[j] - 1);
      ಗಣ->ದತ್ತಾಂಶ[j] = ಖಾಲಿ;
      ಗಣ->ids[j] = ಗಣ->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_ಸ್ಥಾನ_IDX(ಗಣ, i + 1);
    }
  }
}

ಶೂನ್ಯ Curl_u8_strset_unset(ರಚನೆ u8_strset *ಗಣ, uint8_t id)
{
  u8_strset_unset(ಗಣ, id, FALSE);
}

ಶೂನ್ಯ Curl_u8_strset_unset0(ರಚನೆ u8_strset *ಗಣ, uint8_t id)
{
  u8_strset_unset(ಗಣ, id, TRUE);
}

CURLcode Curl_u8_strset_copy(ರಚನೆ u8_strset *dest, ರಚನೆ u8_strset *src)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  ಗಾಗಿ(i = 0; !ಫಲಿತಾಂಶ && (i < CURL_U8_STRSET_ಸ್ಥಾನ_CNT(src)); ++i) {
    ಆದರೆ(src->ದತ್ತಾಂಶ[i])
      ಫಲಿತಾಂಶ = u8_strset_ಗಣ(dest, src->ids[i], src->ದತ್ತಾಂಶ[i]);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
