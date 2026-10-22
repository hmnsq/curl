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
#समावेश "curl_setup.h"

#समावेश "u8_strset.h"
#समावेश "curlx/strdup.h"

/* random patterns for API verification */
#अगर_घोषित DEBUGBUILD
#घोषणा CURL_U8_STRSET_MAGIC 0x7117e783
#पूर्ण_अगर

#घोषणा CURL_U8_STRSET_DEBUG      0

#घोषणा CURL_SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#घोषणा CURL_U8_STRSET_SLOT_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#घोषणा CURL_U8_SLOT_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#घोषणा CURL_U8_SLOT_MASK(i)             (uint8_t)((1U << (i)) - 1)
#घोषणा CURL_U8_STRSET_SLOT_CNT(s)       CURL_U8_SLOT_CNT((s)->slotbits)

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
व्योम Curl_u8_strset_init(काष्ठा u8_strset *set)
{
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Warray-bounds"
#पूर्ण_अगर
  स_रखो(set, 0, माप(*set));
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
  set->तथ्य = set->sdata;
  set->ids = set->sids;
  set->psl = set->spsl;
  set->slotbits = CURL_U8_STRSET_START_BITS;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->गिनती = 0;
#अगर_घोषित DEBUGBUILD
  set->init = CURL_U8_STRSET_MAGIC;
#पूर्ण_अगर
}

व्योम Curl_u8_strset_clear(काष्ठा u8_strset *set)
{
  uint16_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  क्रम(i = 0; i < CURL_U8_STRSET_SLOT_CNT(set); ++i)
    curlx_safefree(set->तथ्य[i]);

  अगर(set->तथ्य != set->sdata)
    curlx_safefree(set->तथ्य);
  Curl_u8_strset_init(set);
}

अटल व्योम u8_strset_addn(काष्ठा u8_strset *set, uint8_t id, अक्षर *val)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  जबतक(set->तथ्य[i]) {
    अगर(psl > set->psl[i]) { /* SWAP */
      अक्षर *tmpdata;
      tmpdata = set->तथ्य[i];
      set->तथ्य[i] = val;
      val = tmpdata;
      CURL_SWAP(set->psl[i], psl);
      CURL_SWAP(set->ids[i], id);
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
  set->ids[i] = id;
  set->तथ्य[i] = val;
  set->psl[i] = psl;
  ++set->गिनती;
}

अटल bool u8_strset_grow(काष्ठा u8_strset *set)
{
  uint8_t *पूर्व_ids, nslotbits;
  uint16_t पूर्व_slots;
  अक्षर **पूर्व_तथ्य;
  माप_प्रकार nslots;
  व्योम *d;

  अगर(set->slotbits >= 8)
    वापस FALSE;
  nslotbits = (uint8_t)(set->slotbits + 1);
#अगर CURL_U8_STRSET_DEBUG
  curl_mfprintf(मानक_त्रुटि, "u8_strset_grow from %d to %d\n",
                set->slotbits, nslotbits);
#पूर्ण_अगर
  nslots = CURL_U8_SLOT_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * माप(अक्षर *)) + (2 * nslots));
  अगर(!d)
    वापस FALSE;

  पूर्व_तथ्य = set->तथ्य;
  पूर्व_ids = set->ids;
  पूर्व_slots = set->slotbits;
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wanalyzer-allocation-size"
#पूर्ण_अगर
  set->तथ्य = (अक्षर **)d;
#अगर defined(__GNUC__) && __GNUC__ >= 13
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
  set->ids = (uint8_t *)d + (nslots * माप(अक्षर *));
  set->psl = set->ids + nslots;
  set->slotbits = nslotbits;
  set->slotmask = CURL_U8_SLOT_MASK(set->slotbits);
  set->गिनती = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    क्रम(i = 0; i < CURL_U8_SLOT_CNT(पूर्व_slots); ++i) {
      अगर(पूर्व_तथ्य[i])
        u8_strset_addn(set, पूर्व_ids[i], पूर्व_तथ्य[i]);
    }
  }
  अगर(पूर्व_तथ्य != set->sdata)
    curlx_free(पूर्व_तथ्य);
  वापस TRUE;
}

अटल bool u8_strset_get_सूचकांक(काष्ठा u8_strset *set,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(set, id);
  uint8_t psl = 0;
  जबतक(set->तथ्य[i] && (psl <= set->psl[i])) {
    अगर(set->ids[i] == id) {
      *pindex = i;
#अगर CURL_U8_STRSET_DEBUG
      curl_mfprintf(मानक_त्रुटि, "u8_strset_index %d=%s\n", id, set->तथ्य[i]);
#पूर्ण_अगर
      वापस TRUE;
    }
    i = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    ++psl;
  }
#अगर CURL_U8_STRSET_DEBUG
  curl_mfprintf(मानक_त्रुटि, "u8_strset_index %d not found\n", id);
#पूर्ण_अगर
  *pindex = 0;
  वापस FALSE;
}

#अगर_घोषित UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_गिनती(काष्ठा u8_strset *set)
{
  वापस set->गिनती;
}
#पूर्ण_अगर

स्थिर अक्षर *Curl_u8_strset_get(काष्ठा u8_strset *set, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  अगर(u8_strset_get_सूचकांक(set, id, &i))
    वापस set->तथ्य[i];
  वापस शून्य;
}

CURLcode Curl_u8_strset_setn(काष्ठा u8_strset *set,
                             uint8_t id, अक्षर *str)
{
  uint8_t i;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
#अगर CURL_U8_STRSET_DEBUG
  curl_mfprintf(मानक_त्रुटि, "u8_strset_setn %d=%s\n", id, str);
#पूर्ण_अगर
  अगर(!str) {
    Curl_u8_strset_unset(set, id);
    वापस CURLE_OK;
  }

  अगर(u8_strset_get_सूचकांक(set, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(set->तथ्य[i]);
    set->तथ्य[i] = str;
    वापस CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  अगर((set->गिनती >= CURL_U8_STRSET_SLOT_CNT(set)) && !u8_strset_grow(set)) {
    curlx_free(str);
    वापस CURLE_OUT_OF_MEMORY;
  }

  u8_strset_addn(set, id, str);
  वापस CURLE_OK;
}

CURLcode Curl_u8_strset_setx(काष्ठा u8_strset *set,
                             uint8_t id, स्थिर अक्षर *str, माप_प्रकार slen)
{
  अक्षर *val;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  अगर(!str) {
    Curl_u8_strset_unset(set, id);
    वापस CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  अगर(!val)
    वापस CURLE_OUT_OF_MEMORY;
  वापस Curl_u8_strset_setn(set, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_set(काष्ठा u8_strset *set,
                                uint8_t id, स्थिर अक्षर *str);
UNITTEST CURLcode u8_strset_set(काष्ठा u8_strset *set,
                                uint8_t id, स्थिर अक्षर *str)
{
  वापस Curl_u8_strset_setx(set, id, str, str ? म_माप(str) : 0);
}

अटल व्योम u8_strset_unset(काष्ठा u8_strset *set, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(set->init == CURL_U8_STRSET_MAGIC);
  अगर(u8_strset_get_सूचकांक(set, id, &i)) {
    /* `id` is in set */
    अगर(zero)
      curlx_strzero(set->तथ्य[i]);
    curlx_safefree(set->तथ्य[i]);
    set->ids[i] = set->psl[i] = 0;
    --set->गिनती;
    j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    /* shift all entries with positive psl "down" */
    जबतक(set->तथ्य[j] && set->psl[j]) {
      set->तथ्य[i] = set->तथ्य[j];
      set->ids[i] = set->ids[j];
      set->psl[i] = (uint8_t)(set->psl[j] - 1);
      set->तथ्य[j] = शून्य;
      set->ids[j] = set->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_SLOT_IDX(set, i + 1);
    }
  }
}

व्योम Curl_u8_strset_unset(काष्ठा u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, FALSE);
}

व्योम Curl_u8_strset_unset0(काष्ठा u8_strset *set, uint8_t id)
{
  u8_strset_unset(set, id, TRUE);
}

CURLcode Curl_u8_strset_copy(काष्ठा u8_strset *dest, काष्ठा u8_strset *src)
{
  CURLcode परिणाम = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  क्रम(i = 0; !परिणाम && (i < CURL_U8_STRSET_SLOT_CNT(src)); ++i) {
    अगर(src->तथ्य[i])
      परिणाम = u8_strset_set(dest, src->ids[i], src->तथ्य[i]);
  }
  वापस परिणाम;
}
