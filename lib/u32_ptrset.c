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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "u32_ptrset.h"

/* random patterns for API verification */
#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय CURL_U32_PTRSET_MAGIC 0x75703272
#यदि_अन्तः

#परिभाषय CURL_U32_PTRSET_DEBUG      0

#यदि_अपरिभाषितम् CURL_SWAP
#परिभाषय CURL_SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#यदि_अन्तः

#परिभाषय CURL_U32_SLOT_CNT(द्वयङ्काः)        ((द्वयङ्काः) ? (1U << (द्वयङ्काः)) : 0)
#परिभाषय CURL_U32_PTRSET_SLOT_CNT(s)    CURL_U32_SLOT_CNT((s)->slotbits)
#परिभाषय CURL_U32_PTRSET_SLOT_IDX(s, i) (uint32_t)((i) & (s)->slotmask)

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
शून्य Curl_u32_ptrset_init(संरचना u32_ptrset *समुच्चयः,
                          Curl_u32_ptrset_dtor *dtor)
{
  स्मृतिं_पूरय(समुच्चयः, 0, परिमाण(*समुच्चयः));
  समुच्चयः->dtor = dtor;
#यदि_परिभाषितम् DEBUGBUILD
  समुच्चयः->init = CURL_U32_PTRSET_MAGIC;
#यदि_अन्तः
}

शून्य Curl_u32_ptrset_clear(संरचना u32_ptrset *समुच्चयः)
{
  uint32_t i;
  DEBUGASSERT(समुच्चयः->init == CURL_U32_PTRSET_MAGIC);
  यदि(समुच्चयः->dtor)
    प्रति(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(समुच्चयः); ++i)
      यदि(समुच्चयः->दत्त[i])
        समुच्चयः->dtor(समुच्चयः->ids[i], समुच्चयः->दत्त[i]);

  curlx_safefree(समुच्चयः->दत्त);
  Curl_u32_ptrset_init(समुच्चयः, समुच्चयः->dtor);
}

स्थिर शून्य u32_ptrset_सङ्कलय(संरचना u32_ptrset *समुच्चयः, uint32_t id, शून्य *ptr)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(समुच्चयः, id);
  uint32_t psl = 0;
  यावत्(समुच्चयः->दत्त[i]) {
    यदि(psl > समुच्चयः->psl[i]) { /* SWAP */
      शून्य *tmpdata;
#यदि CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#यदि_अन्तः
      tmpdata = समुच्चयः->दत्त[i];
      समुच्चयः->दत्त[i] = ptr;
      ptr = tmpdata;
      CURL_SWAP(समुच्चयः->psl[i], psl);
      CURL_SWAP(समुच्चयः->ids[i], id);
    }
    i = CURL_U32_PTRSET_SLOT_IDX(समुच्चयः, i + 1);
    ++psl;
  }
  समुच्चयः->ids[i] = id;
  समुच्चयः->दत्त[i] = ptr;
  समुच्चयः->psl[i] = psl;
  ++समुच्चयः->गणनम्;
#यदि CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_add %u=%p to idx=%u\n", id, ptr, i);
#यदि_अन्तः
}

स्थिर bool u32_ptrset_grow(संरचना u32_ptrset *समुच्चयः)
{
  uint32_t i, *पूर्व_ids;
  uint8_t nslotbits, pslotbits;
  शून्य **पूर्व_दत्त;
  size_t nslots;
  ध्रुव size_t slot_आकार = (परिमाण(शून्य *) + (2 * परिमाण(uint32_t)));
  शून्य *d;

  यदि(समुच्चयः->slotbits >= 31)
    प्रत्यागम FALSE;
  nslotbits = समुच्चयः->slotbits ? (uint8_t)(समुच्चयः->slotbits + 1) : 5;
  nslots = CURL_U32_SLOT_CNT(nslotbits);
  यदि(nslots > (SIZE_MAX / slot_आकार)) /* 32-bit arch may trigger here */
    प्रत्यागम FALSE;
#यदि CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_grow from bits=%d to %d, slots=%zu\n",
                समुच्चयः->slotbits, nslotbits, nslots);
#यदि_अन्तः
  d = curlx_calloc(1, nslots * slot_आकार);
  यदि(!d)
    प्रत्यागम FALSE;

  पूर्व_दत्त = समुच्चयः->दत्त;
  पूर्व_ids = समुच्चयः->ids;
  pslotbits = समुच्चयः->slotbits;
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wanalyzer-allocation-size"
#यदि_अन्तः
  समुच्चयः->दत्त = d;
  समुच्चयः->ids = (uint32_t *)(समुच्चयः->दत्त + nslots);
  समुच्चयः->psl = (uint32_t *)(समुच्चयः->ids + nslots);
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

  समुच्चयः->slotbits = nslotbits;
  समुच्चयः->slotmask = (uint32_t)((1U << समुच्चयः->slotbits) - 1);
  समुच्चयः->गणनम् = 0;
  /* re-add previous entries */
  प्रति(i = 0; i < CURL_U32_SLOT_CNT(pslotbits); ++i) {
    यदि(पूर्व_दत्त[i])
      u32_ptrset_सङ्कलय(समुच्चयः, पूर्व_ids[i], पूर्व_दत्त[i]);
  }
  यदि(पूर्व_दत्त)
    curlx_free(पूर्व_दत्त);
  प्रत्यागम TRUE;
}

स्थिर bool u32_ptrset_get_सूचकांक(संरचना u32_ptrset *समुच्चयः,
                                 uint32_t id, uint32_t *pindex)
{
  uint32_t i = CURL_U32_PTRSET_SLOT_IDX(समुच्चयः, id);
  uint32_t psl = 0;
  यदि(!समुच्चयः->दत्त)
    प्रत्यागम FALSE;
  यावत्(समुच्चयः->दत्त[i] && (psl <= समुच्चयः->psl[i])) {
    यदि(समुच्चयः->ids[i] == id) {
      *pindex = i;
#यदि CURL_U32_PTRSET_DEBUG
      curl_mfprintf(stderr, "u32_ptrset_index %u=%p at idx=%u\n",
                    id, समुच्चयः->दत्त[i], i);
#यदि_अन्तः
      प्रत्यागम TRUE;
    }
    i = CURL_U32_PTRSET_SLOT_IDX(समुच्चयः, i + 1);
    ++psl;
  }
#यदि CURL_U32_PTRSET_DEBUG
  curl_mfprintf(stderr, "u32_ptrset_index %u not found\n", id);
#यदि_अन्तः
  *pindex = 0;
  प्रत्यागम FALSE;
}

uint32_t Curl_u32_ptrset_गणनम्(संरचना u32_ptrset *समुच्चयः)
{
  प्रत्यागम समुच्चयः->गणनम्;
}

शून्य *Curl_u32_ptrset_get(संरचना u32_ptrset *समुच्चयः, uint32_t id)
{
  uint32_t i;
  DEBUGASSERT(समुच्चयः->init == CURL_U32_PTRSET_MAGIC);
  यदि(u32_ptrset_get_सूचकांक(समुच्चयः, id, &i))
    प्रत्यागम समुच्चयः->दत्त[i];
  प्रत्यागम रिक्त;
}

CURLcode Curl_u32_ptrset_समुच्चयः(संरचना u32_ptrset *समुच्चयः,
                             uint32_t id, शून्य *ptr)
{
  uint32_t i;

  DEBUGASSERT(समुच्चयः->init == CURL_U32_PTRSET_MAGIC);
  यदि(!ptr) {
    Curl_u32_ptrset_unset(समुच्चयः, id);
    प्रत्यागम CURLE_OK;
  }

  यदि(u32_ptrset_get_सूचकांक(समुच्चयः, id, &i)) {
    /* `id` is in set, replace value */
    यदि(समुच्चयः->dtor && (समुच्चयः->दत्त[i] != ptr))
      समुच्चयः->dtor(समुच्चयः->ids[i], समुच्चयः->दत्त[i]);
#यदि CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_set replace %u=%p at idx=%u\n",
                  id, ptr, i);
#यदि_अन्तः
    समुच्चयः->दत्त[i] = ptr;
    प्रत्यागम CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  यदि((समुच्चयः->गणनम् >= CURL_U32_PTRSET_SLOT_CNT(समुच्चयः)) &&
     !u32_ptrset_grow(समुच्चयः)) {
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  u32_ptrset_सङ्कलय(समुच्चयः, id, ptr);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य u32_ptrset_unset(संरचना u32_ptrset *समुच्चयः, uint32_t id)
{
  uint32_t i, j;

  DEBUGASSERT(समुच्चयः->init == CURL_U32_PTRSET_MAGIC);
  यदि(u32_ptrset_get_सूचकांक(समुच्चयः, id, &i)) {
    /* `id` is in set */
#यदि CURL_U32_PTRSET_DEBUG
    curl_mfprintf(stderr, "u32_ptrset_unset %u at idx=%u\n", id, i);
#यदि_अन्तः
    यदि(समुच्चयः->dtor)
      समुच्चयः->dtor(समुच्चयः->ids[i], समुच्चयः->दत्त[i]);
    समुच्चयः->दत्त[i] = रिक्त;
    समुच्चयः->ids[i] = समुच्चयः->psl[i] = 0;
    --समुच्चयः->गणनम्;
    j = CURL_U32_PTRSET_SLOT_IDX(समुच्चयः, i + 1);
    /* shift all entries with positive psl "down" */
    यावत्(समुच्चयः->दत्त[j] && समुच्चयः->psl[j]) {
      समुच्चयः->दत्त[i] = समुच्चयः->दत्त[j];
      समुच्चयः->ids[i] = समुच्चयः->ids[j];
      समुच्चयः->psl[i] = समुच्चयः->psl[j] - 1;
      समुच्चयः->दत्त[j] = रिक्त;
      समुच्चयः->ids[j] = समुच्चयः->psl[j] = 0;
      i = j;
      j = CURL_U32_PTRSET_SLOT_IDX(समुच्चयः, i + 1);
    }
  }
}

शून्य Curl_u32_ptrset_unset(संरचना u32_ptrset *समुच्चयः, uint32_t id)
{
  u32_ptrset_unset(समुच्चयः, id);
}

शून्य Curl_u32_ptrset_दर्शय(संरचना u32_ptrset *समुच्चयः,
                            Curl_u32_ptrset_दर्शय_cb *cb,
                            शून्य *user_दत्त)
{
  uint32_t i;
  प्रति(i = 0; i < CURL_U32_PTRSET_SLOT_CNT(समुच्चयः); ++i) {
    यदि(समुच्चयः->दत्त[i] && !cb(समुच्चयः->ids[i], समुच्चयः->दत्त[i], user_दत्त))
      भङ्ग;
  }
}
