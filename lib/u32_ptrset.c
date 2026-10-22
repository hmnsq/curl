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

#అన్తర్భావ "u32_ptrset.h"

/* random patterns for API verification */
#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు CURL_U32_PTRSET_MAGIC 0x75703272
#అయితే_ముగింపు

#నిర్వచించు CURL_U32_PTRSET_DEBUG      0

#నిర్వచితం_కాకపోతే CURL_మార్పిడి
#నిర్వచించు CURL_మార్పిడి(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#అయితే_ముగింపు

#నిర్వచించు CURL_U32_స్థానం_CNT(బిట్లు)        ((బిట్లు) ? (1U << (బిట్లు)) : 0)
#నిర్వచించు CURL_U32_PTRSET_స్థానం_CNT(s)    CURL_U32_స్థానం_CNT((s)->slotbits)
#నిర్వచించు CURL_U32_PTRSET_స్థానం_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
శూన్య Curl_u32_ptrset_init(నిర్మాణం u32_ptrset *సమితి,
                          Curl_u32_ptrset_dtor *dtor)
{
  memset(సమితి, 0, పరిమాణం(*సమితి));
  సమితి->dtor = dtor;
#నిర్వచితమైతే DEBUGBUILD
  సమితి->init = CURL_U32_PTRSET_MAGIC;
#అయితే_ముగింపు
}

శూన్య Curl_u32_ptrset_clear(నిర్మాణం u32_ptrset *సమితి)
{
  uint32_t i;
  DEBUGASSERT(సమితి->init == CURL_U32_PTRSET_MAGIC);
  ఒకవేళ(సమితి->dtor)
    కొరకు(i = 0; i < CURL_U32_PTRSET_స్థానం_CNT(సమితి); ++i)
      ఒకవేళ(సమితి->దత్తాంశం[i])
        సమితి->dtor(సమితి->ids[i], సమితి->దత్తాంశం[i]);

  curlx_safefree(సమితి->దత్తాంశం);
  Curl_u32_ptrset_init(సమితి, సమితి->dtor);
}

స్థిర శూన్య u32_ptrset_కూడు(నిర్మాణం u32_ptrset *సమితి, uint32_t id, శూన్య *ptr)
{
  uint32_t i = CURL_U32_PTRSET_స్థానం_IDX(సమితి, id);
  uint32_t psl = 0;
  అయితే(సమితి->దత్తాంశం[i]) {
    ఒకవేళ(psl > సమితి->psl[i]) { /*మార్పిడి*/
      శూన్య *tmpdata;
#ఒకవేళ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#అయితే_ముగింపు
      tmpdata = సమితి->దత్తాంశం[i];
      సమితి->దత్తాంశం[i] = ptr;
      ptr = tmpdata;
      CURL_మార్పిడి(సమితి->psl[i], psl);
      CURL_మార్పిడి(సమితి->ids[i], id);
    }
    i = CURL_U32_PTRSET_స్థానం_IDX(సమితి, i + 1);
    ++psl;
  }
  సమితి->ids[i] = id;
  సమితి->దత్తాంశం[i] = ptr;
  సమితి->psl[i] = psl;
  ++సమితి->లెక్క;
#ఒకవేళ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#అయితే_ముగింపు
}

స్థిర bool u32_ptrset_grow(నిర్మాణం u32_ptrset *సమితి)
{
  uint32_t i, *prev_ids;
  uint8_t nslotbits, pslotbits;
  శూన్య **prev_దత్తాంశం;
  size_t nslots;
  స్థిరరాశి size_t స్థానం_కొలత = (పరిమాణం(శూన్య *) + (2 * పరిమాణం(uint32_t)));
  శూన్య *d;

  ఒకవేళ(సమితి->slotbits >= 31)
    తిరిగి FALSE;
  nslotbits = సమితి->slotbits ? (uint8_t)(సమితి->slotbits + 1) : 5;
  nslots = CURL_U32_స్థానం_CNT(nslotbits);
  ఒకవేళ(nslots > (SIZE_MAX / స్థానం_కొలత)) /* 32-bit arch may trigger here */
    తిరిగి FALSE;
#ఒకవేళ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                సమితి->slotbits, nslotbits, nslots);
#అయితే_ముగింపు
  d = curlx_calloc(1, nslots * స్థానం_కొలత);
  ఒకవేళ(!d)
    తిరిగి FALSE;

  prev_దత్తాంశం = సమితి->దత్తాంశం;
  prev_ids = సమితి->ids;
  pslotbits = సమితి->slotbits;
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wanalyzer-allocation-size"
#అయితే_ముగింపు
  సమితి->దత్తాంశం = d;
  సమితి->ids = (uint32_t *)(సమితి->దత్తాంశం + nslots);
  సమితి->psl = (uint32_t *)(సమితి->ids + nslots);
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 13
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

  సమితి->slotbits = nslotbits;
  సమితి->slotmask = (uint32_t)((1U << సమితి->slotbits) - 1);
  సమితి->లెక్క = 0;
  /* re-add previous entries */
  కొరకు(i = 0; i < CURL_U32_స్థానం_CNT(pslotbits); ++i) {
    ఒకవేళ(prev_దత్తాంశం[i])
      u32_ptrset_కూడు(సమితి, prev_ids[i], prev_దత్తాంశం[i]);
  }
  ఒకవేళ(prev_దత్తాంశం)
    curlx_free(prev_దత్తాంశం);
  తిరిగి TRUE;
}

స్థిర bool u32_ptrset_get_సూచీ(నిర్మాణం u32_ptrset *సమితి,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_స్థానం_IDX(సమితి, id);
  uint32_t psl = 0;
  ఒకవేళ(!సమితి->దత్తాంశం)
    తిరిగి FALSE;
  అయితే(సమితి->దత్తాంశం[i] && (psl <= సమితి->psl[i])) {
    ఒకవేళ(సమితి->ids[i] == id) {
      *pindex = i;
#ఒకవేళ CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, సమితి->దత్తాంశం[i], i);
#అయితే_ముగింపు
      తిరిగి TRUE;
    }
    i = CURL_U32_PTRSET_స్థానం_IDX(సమితి, i + 1);
    ++psl;
  }
#ఒకవేళ CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#అయితే_ముగింపు
  *pindex = 0;
  తిరిగి FALSE;
}

uint32_t Curl_u32_ptrset_లెక్క(నిర్మాణం u32_ptrset *సమితి)
{
  తిరిగి సమితి->లెక్క;
}

శూన్య *Curl_u32_ptrset_get(నిర్మాణం u32_ptrset *సమితి, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(సమితి->init == CURL_U32_PTRSET_MAGIC);
  ఒకవేళ(u32_ptrset_get_సూచీ(సమితి, id, &i))
    తిరిగి సమితి->దత్తాంశం[i];
  తిరిగి ఖాళీ;
}

CURLcode Curl_u32_ptrset_సమితి(నిర్మాణం u32_ptrset *సమితి,
                             uint32_t id, శూన్య *ptr)
{
  uint32_t i;

  DEBUGASSERT(సమితి->init == CURL_U32_PTRSET_MAGIC);
  ఒకవేళ(!ptr) {
    Curl_u32_ptrset_unset(సమితి, id);
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(u32_ptrset_get_సూచీ(సమితి, id, &i)) {
    /* `id` is in set, replace value */
    ఒకవేళ(సమితి->dtor && (సమితి->దత్తాంశం[i] != ptr))
      సమితి->dtor(సమితి->ids[i], సమితి->దత్తాంశం[i]);
#ఒకవేళ CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#అయితే_ముగింపు
    సమితి->దత్తాంశం[i] = ptr;
    తిరిగి CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  ఒకవేళ((సమితి->లెక్క >= CURL_U32_PTRSET_స్థానం_CNT(సమితి)) &&
     !u32_ptrset_grow(సమితి)) {
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  u32_ptrset_కూడు(సమితి, id, ptr);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య u32_ptrset_unset(నిర్మాణం u32_ptrset *సమితి, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(సమితి->init == CURL_U32_PTRSET_MAGIC);
  ఒకవేళ(u32_ptrset_get_సూచీ(సమితి, id, &i)) {
    /* `id` is in set */
#ఒకవేళ CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#అయితే_ముగింపు
    ఒకవేళ(సమితి->dtor)
      సమితి->dtor(సమితి->ids[i], సమితి->దత్తాంశం[i]);
    సమితి->దత్తాంశం[i] = ఖాళీ;
    సమితి->ids[i] = సమితి->psl[i] = 0;
    --సమితి->లెక్క;
    j = CURL_U32_PTRSET_స్థానం_IDX(సమితి, i + 1);
    /* shift all entries with positive psl "down" */
    అయితే(సమితి->దత్తాంశం[j] && సమితి->psl[j]) {
      సమితి->దత్తాంశం[i] = సమితి->దత్తాంశం[j];
      సమితి->ids[i] = సమితి->ids[j];
      సమితి->psl[i] = సమితి->psl[j] - 1;
      సమితి->దత్తాంశం[j] = ఖాళీ;
      సమితి->ids[j] = సమితి->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_స్థానం_IDX(సమితి, i + 1);
    }
  }
}

శూన్య Curl_u32_ptrset_unset(నిర్మాణం u32_ptrset *సమితి, uint32_t id)
{
  u32_ptrset_unset(సమితి, id);
}

శూన్య Curl_u32_ptrset_సందర్శించు(నిర్మాణం u32_ptrset *సమితి,
                            Curl_u32_ptrset_సందర్శించు_cb *cb,
                            శూన్య *user_దత్తాంశం)
{
  uint32_t i;
  కొరకు(i = 0; i < CURL_U32_PTRSET_స్థానం_CNT(సమితి); ++i) {
    ఒకవేళ(సమితి->దత్తాంశం[i] && !cb(సమితి->ids[i], సమితి->దత్తాంశం[i], user_దత్తాంశం))
      ఆపు;
  }
}
