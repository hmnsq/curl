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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "u8_strset.h"
#అన్తర్భావ "curlx/strdup.h"

/* random patterns for API verification */
#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు CURL_U8_STRSET_MAGIC 0x7117e783
#అయితే_ముగింపు

#నిర్వచించు CURL_U8_STRSET_DEBUG      0

#నిర్వచించు CURL_మార్పిడి(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#నిర్వచించు CURL_U8_STRSET_స్థానం_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#నిర్వచించు CURL_U8_స్థానం_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#నిర్వచించు CURL_U8_స్థానం_ముసుగు(i)             (uint8_t)((1U << (i)) - 1)
#నిర్వచించు CURL_U8_STRSET_స్థానం_CNT(s)       CURL_U8_స్థానం_CNT((s)->slotbits)

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
శూన్య Curl_u8_strset_init(నిర్మాణం u8_strset *సమితి)
{
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Warray-bounds"
#అయితే_ముగింపు
  memset(సమితి, 0, పరిమాణం(*సమితి));
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
  సమితి->దత్తాంశం = సమితి->sdata;
  సమితి->ids = సమితి->sids;
  సమితి->psl = సమితి->spsl;
  సమితి->slotbits = CURL_U8_STRSET_ప్రారంభం_బిట్లు;
  సమితి->slotmask = CURL_U8_స్థానం_ముసుగు(సమితి->slotbits);
  సమితి->లెక్క = 0;
#నిర్వచితమైతే DEBUGBUILD
  సమితి->init = CURL_U8_STRSET_MAGIC;
#అయితే_ముగింపు
}

శూన్య Curl_u8_strset_clear(నిర్మాణం u8_strset *సమితి)
{
  uint16_t i;
  DEBUGASSERT(సమితి->init == CURL_U8_STRSET_MAGIC);
  కొరకు(i = 0; i < CURL_U8_STRSET_స్థానం_CNT(సమితి); ++i)
    curlx_safefree(సమితి->దత్తాంశం[i]);

  ఒకవేళ(సమితి->దత్తాంశం != సమితి->sdata)
    curlx_safefree(సమితి->దత్తాంశం);
  Curl_u8_strset_init(సమితి);
}

స్థిర శూన్య u8_strset_addn(నిర్మాణం u8_strset *సమితి, uint8_t id, అక్షరం *val)
{
  uint8_t i = CURL_U8_STRSET_స్థానం_IDX(సమితి, id);
  uint8_t psl = 0;
  అయితే(సమితి->దత్తాంశం[i]) {
    ఒకవేళ(psl > సమితి->psl[i]) { /*మార్పిడి*/
      అక్షరం *tmpdata;
      tmpdata = సమితి->దత్తాంశం[i];
      సమితి->దత్తాంశం[i] = val;
      val = tmpdata;
      CURL_మార్పిడి(సమితి->psl[i], psl);
      CURL_మార్పిడి(సమితి->ids[i], id);
    }
    i = CURL_U8_STRSET_స్థానం_IDX(సమితి, i + 1);
    ++psl;
  }
  సమితి->ids[i] = id;
  సమితి->దత్తాంశం[i] = val;
  సమితి->psl[i] = psl;
  ++సమితి->లెక్క;
}

స్థిర bool u8_strset_grow(నిర్మాణం u8_strset *సమితి)
{
  uint8_t *prev_ids, nslotbits;
  uint16_t prev_slots;
  అక్షరం **prev_దత్తాంశం;
  size_t nslots;
  శూన్య *d;

  ఒకవేళ(సమితి->slotbits >= 8)
    తిరిగి FALSE;
  nslotbits = (uint8_t)(సమితి->slotbits + 1);
#ఒకవేళ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                సమితి->slotbits, nslotbits);
#అయితే_ముగింపు
  nslots = CURL_U8_స్థానం_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * పరిమాణం(అక్షరం *)) + (2 * nslots));
  ఒకవేళ(!d)
    తిరిగి FALSE;

  prev_దత్తాంశం = సమితి->దత్తాంశం;
  prev_ids = సమితి->ids;
  prev_slots = సమితి->slotbits;
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wanalyzer-allocation-size"
#అయితే_ముగింపు
  సమితి->దత్తాంశం = (అక్షరం **)d;
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
  సమితి->ids = (uint8_t *)d + (nslots * పరిమాణం(అక్షరం *));
  సమితి->psl = సమితి->ids + nslots;
  సమితి->slotbits = nslotbits;
  సమితి->slotmask = CURL_U8_స్థానం_ముసుగు(సమితి->slotbits);
  సమితి->లెక్క = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    కొరకు(i = 0; i < CURL_U8_స్థానం_CNT(prev_slots); ++i) {
      ఒకవేళ(prev_దత్తాంశం[i])
        u8_strset_addn(సమితి, prev_ids[i], prev_దత్తాంశం[i]);
    }
  }
  ఒకవేళ(prev_దత్తాంశం != సమితి->sdata)
    curlx_free(prev_దత్తాంశం);
  తిరిగి TRUE;
}

స్థిర bool u8_strset_get_సూచీ(నిర్మాణం u8_strset *సమితి,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_స్థానం_IDX(సమితి, id);
  uint8_t psl = 0;
  అయితే(సమితి->దత్తాంశం[i] && (psl <= సమితి->psl[i])) {
    ఒకవేళ(సమితి->ids[i] == id) {
      *pindex = i;
#ఒకవేళ CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, సమితి->దత్తాంశం[i]);
#అయితే_ముగింపు
      తిరిగి TRUE;
    }
    i = CURL_U8_STRSET_స్థానం_IDX(సమితి, i + 1);
    ++psl;
  }
#ఒకవేళ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#అయితే_ముగింపు
  *pindex = 0;
  తిరిగి FALSE;
}

#నిర్వచితమైతే UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_లెక్క(నిర్మాణం u8_strset *సమితి)
{
  తిరిగి సమితి->లెక్క;
}
#అయితే_ముగింపు

స్థిరరాశి అక్షరం *Curl_u8_strset_get(నిర్మాణం u8_strset *సమితి, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(సమితి->init == CURL_U8_STRSET_MAGIC);
  ఒకవేళ(u8_strset_get_సూచీ(సమితి, id, &i))
    తిరిగి సమితి->దత్తాంశం[i];
  తిరిగి ఖాళీ;
}

CURLcode Curl_u8_strset_setn(నిర్మాణం u8_strset *సమితి,
                             uint8_t id, అక్షరం *str)
{
  uint8_t i;

  DEBUGASSERT(సమితి->init == CURL_U8_STRSET_MAGIC);
#ఒకవేళ CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#అయితే_ముగింపు
  ఒకవేళ(!str) {
    Curl_u8_strset_unset(సమితి, id);
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(u8_strset_get_సూచీ(సమితి, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(సమితి->దత్తాంశం[i]);
    సమితి->దత్తాంశం[i] = str;
    తిరిగి CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ఒకవేళ((సమితి->లెక్క >= CURL_U8_STRSET_స్థానం_CNT(సమితి)) && !u8_strset_grow(సమితి)) {
    curlx_free(str);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  u8_strset_addn(సమితి, id, str);
  తిరిగి CURLE_OK;
}

CURLcode Curl_u8_strset_setx(నిర్మాణం u8_strset *సమితి,
                             uint8_t id, స్థిరరాశి అక్షరం *str, size_t slen)
{
  అక్షరం *val;

  DEBUGASSERT(సమితి->init == CURL_U8_STRSET_MAGIC);
  ఒకవేళ(!str) {
    Curl_u8_strset_unset(సమితి, id);
    తిరిగి CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  ఒకవేళ(!val)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  తిరిగి Curl_u8_strset_setn(సమితి, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_సమితి(నిర్మాణం u8_strset *సమితి,
                                uint8_t id, స్థిరరాశి అక్షరం *str);
UNITTEST CURLcode u8_strset_సమితి(నిర్మాణం u8_strset *సమితి,
                                uint8_t id, స్థిరరాశి అక్షరం *str)
{
  తిరిగి Curl_u8_strset_setx(సమితి, id, str, str ? అక్షరశ్రేణి_పొడవు(str) : 0);
}

స్థిర శూన్య u8_strset_unset(నిర్మాణం u8_strset *సమితి, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(సమితి->init == CURL_U8_STRSET_MAGIC);
  ఒకవేళ(u8_strset_get_సూచీ(సమితి, id, &i)) {
    /* `id` is in set */
    ఒకవేళ(zero)
      curlx_strzero(సమితి->దత్తాంశం[i]);
    curlx_safefree(సమితి->దత్తాంశం[i]);
    సమితి->ids[i] = సమితి->psl[i] = 0;
    --సమితి->లెక్క;
    j = CURL_U8_STRSET_స్థానం_IDX(సమితి, i + 1);
    /* shift all entries with positive psl "down" */
    అయితే(సమితి->దత్తాంశం[j] && సమితి->psl[j]) {
      సమితి->దత్తాంశం[i] = సమితి->దత్తాంశం[j];
      సమితి->ids[i] = సమితి->ids[j];
      సమితి->psl[i] = (uint8_t)(సమితి->psl[j] - 1);
      సమితి->దత్తాంశం[j] = ఖాళీ;
      సమితి->ids[j] = సమితి->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_స్థానం_IDX(సమితి, i + 1);
    }
  }
}

శూన్య Curl_u8_strset_unset(నిర్మాణం u8_strset *సమితి, uint8_t id)
{
  u8_strset_unset(సమితి, id, FALSE);
}

శూన్య Curl_u8_strset_unset0(నిర్మాణం u8_strset *సమితి, uint8_t id)
{
  u8_strset_unset(సమితి, id, TRUE);
}

CURLcode Curl_u8_strset_copy(నిర్మాణం u8_strset *dest, నిర్మాణం u8_strset *src)
{
  CURLcode ఫలితం = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  కొరకు(i = 0; !ఫలితం && (i < CURL_U8_STRSET_స్థానం_CNT(src)); ++i) {
    ఒకవేళ(src->దత్తాంశం[i])
      ఫలితం = u8_strset_సమితి(dest, src->ids[i], src->దత్తాంశం[i]);
  }
  తిరిగి ఫలితం;
}
