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

#अन्तर्भाव "u8_strset.h"
#अन्तर्भाव "curlx/strdup.h"

/* random patterns for API verification */
#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय CURL_U8_STRSET_MAGIC 0x7117e783
#यदि_अन्तः

#परिभाषय CURL_U8_STRSET_DEBUG      0

#परिभाषय CURL_SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#परिभाषय CURL_U8_STRSET_SLOT_IDX(s, i)    (uint8_t)((i) & (s)->slotmask)
#परिभाषय CURL_U8_SLOT_CNT(i)              (uint16_t)((i) ? (1U << (i)) : 0)
#परिभाषय CURL_U8_SLOT_आवरणम्(i)             (uint8_t)((1U << (i)) - 1)
#परिभाषय CURL_U8_STRSET_SLOT_CNT(s)       CURL_U8_SLOT_CNT((s)->slotbits)

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
शून्य Curl_u8_strset_init(संरचना u8_strset *समुच्चयः)
{
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Warray-bounds"
#यदि_अन्तः
  स्मृतिं_पूरय(समुच्चयः, 0, परिमाण(*समुच्चयः));
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
  समुच्चयः->दत्त = समुच्चयः->sdata;
  समुच्चयः->ids = समुच्चयः->sids;
  समुच्चयः->psl = समुच्चयः->spsl;
  समुच्चयः->slotbits = CURL_U8_STRSET_START_द्वयङ्काः;
  समुच्चयः->slotmask = CURL_U8_SLOT_आवरणम्(समुच्चयः->slotbits);
  समुच्चयः->गणनम् = 0;
#यदि_परिभाषितम् DEBUGBUILD
  समुच्चयः->init = CURL_U8_STRSET_MAGIC;
#यदि_अन्तः
}

शून्य Curl_u8_strset_clear(संरचना u8_strset *समुच्चयः)
{
  uint16_t i;
  DEBUGASSERT(समुच्चयः->init == CURL_U8_STRSET_MAGIC);
  प्रति(i = 0; i < CURL_U8_STRSET_SLOT_CNT(समुच्चयः); ++i)
    curlx_safefree(समुच्चयः->दत्त[i]);

  यदि(समुच्चयः->दत्त != समुच्चयः->sdata)
    curlx_safefree(समुच्चयः->दत्त);
  Curl_u8_strset_init(समुच्चयः);
}

स्थिर शून्य u8_strset_addn(संरचना u8_strset *समुच्चयः, uint8_t id, वर्ण *val)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(समुच्चयः, id);
  uint8_t psl = 0;
  यावत्(समुच्चयः->दत्त[i]) {
    यदि(psl > समुच्चयः->psl[i]) { /* SWAP */
      वर्ण *tmpdata;
      tmpdata = समुच्चयः->दत्त[i];
      समुच्चयः->दत्त[i] = val;
      val = tmpdata;
      CURL_SWAP(समुच्चयः->psl[i], psl);
      CURL_SWAP(समुच्चयः->ids[i], id);
    }
    i = CURL_U8_STRSET_SLOT_IDX(समुच्चयः, i + 1);
    ++psl;
  }
  समुच्चयः->ids[i] = id;
  समुच्चयः->दत्त[i] = val;
  समुच्चयः->psl[i] = psl;
  ++समुच्चयः->गणनम्;
}

स्थिर bool u8_strset_grow(संरचना u8_strset *समुच्चयः)
{
  uint8_t *पूर्व_ids, nslotbits;
  uint16_t पूर्व_slots;
  वर्ण **पूर्व_दत्त;
  size_t nslots;
  शून्य *d;

  यदि(समुच्चयः->slotbits >= 8)
    प्रत्यागम FALSE;
  nslotbits = (uint8_t)(समुच्चयः->slotbits + 1);
#यदि CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_grow from %d to %d\n",
                समुच्चयः->slotbits, nslotbits);
#यदि_अन्तः
  nslots = CURL_U8_SLOT_CNT(nslotbits);
  d = curlx_calloc(1, (nslots * परिमाण(वर्ण *)) + (2 * nslots));
  यदि(!d)
    प्रत्यागम FALSE;

  पूर्व_दत्त = समुच्चयः->दत्त;
  पूर्व_ids = समुच्चयः->ids;
  पूर्व_slots = समुच्चयः->slotbits;
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wanalyzer-allocation-size"
#यदि_अन्तः
  समुच्चयः->दत्त = (वर्ण **)d;
#यदि defined(__GNUC__) && __GNUC__ >= 13
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
  समुच्चयः->ids = (uint8_t *)d + (nslots * परिमाण(वर्ण *));
  समुच्चयः->psl = समुच्चयः->ids + nslots;
  समुच्चयः->slotbits = nslotbits;
  समुच्चयः->slotmask = CURL_U8_SLOT_आवरणम्(समुच्चयः->slotbits);
  समुच्चयः->गणनम् = 0;
  /* re-add previous entries */
  {
    uint16_t i;
    प्रति(i = 0; i < CURL_U8_SLOT_CNT(पूर्व_slots); ++i) {
      यदि(पूर्व_दत्त[i])
        u8_strset_addn(समुच्चयः, पूर्व_ids[i], पूर्व_दत्त[i]);
    }
  }
  यदि(पूर्व_दत्त != समुच्चयः->sdata)
    curlx_free(पूर्व_दत्त);
  प्रत्यागम TRUE;
}

स्थिर bool u8_strset_get_सूचकांक(संरचना u8_strset *समुच्चयः,
                                uint8_t id, uint8_t *pindex)
{
  uint8_t i = CURL_U8_STRSET_SLOT_IDX(समुच्चयः, id);
  uint8_t psl = 0;
  यावत्(समुच्चयः->दत्त[i] && (psl <= समुच्चयः->psl[i])) {
    यदि(समुच्चयः->ids[i] == id) {
      *pindex = i;
#यदि CURL_U8_STRSET_DEBUG
      curl_mfprintf(stderr, "u8_strset_index %d=%s\n", id, समुच्चयः->दत्त[i]);
#यदि_अन्तः
      प्रत्यागम TRUE;
    }
    i = CURL_U8_STRSET_SLOT_IDX(समुच्चयः, i + 1);
    ++psl;
  }
#यदि CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_index %d not found\n", id);
#यदि_अन्तः
  *pindex = 0;
  प्रत्यागम FALSE;
}

#यदि_परिभाषितम् UNITTESTS
/* @unittest 3211 */
uint16_t Curl_u8_strset_गणनम्(संरचना u8_strset *समुच्चयः)
{
  प्रत्यागम समुच्चयः->गणनम्;
}
#यदि_अन्तः

ध्रुव वर्ण *Curl_u8_strset_get(संरचना u8_strset *समुच्चयः, uint8_t id)
{
  uint8_t i;
  DEBUGASSERT(समुच्चयः->init == CURL_U8_STRSET_MAGIC);
  यदि(u8_strset_get_सूचकांक(समुच्चयः, id, &i))
    प्रत्यागम समुच्चयः->दत्त[i];
  प्रत्यागम रिक्त;
}

CURLcode Curl_u8_strset_setn(संरचना u8_strset *समुच्चयः,
                             uint8_t id, वर्ण *str)
{
  uint8_t i;

  DEBUGASSERT(समुच्चयः->init == CURL_U8_STRSET_MAGIC);
#यदि CURL_U8_STRSET_DEBUG
  curl_mfprintf(stderr, "u8_strset_setn %d=%s\n", id, str);
#यदि_अन्तः
  यदि(!str) {
    Curl_u8_strset_unset(समुच्चयः, id);
    प्रत्यागम CURLE_OK;
  }

  यदि(u8_strset_get_सूचकांक(समुच्चयः, id, &i)) {
    /* `id` is in set, replace value */
    curlx_free(समुच्चयः->दत्त[i]);
    समुच्चयः->दत्त[i] = str;
    प्रत्यागम CURLE_OK;
  }
  /* `id` not in set yet, grow if full */
  यदि((समुच्चयः->गणनम् >= CURL_U8_STRSET_SLOT_CNT(समुच्चयः)) && !u8_strset_grow(समुच्चयः)) {
    curlx_free(str);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  u8_strset_addn(समुच्चयः, id, str);
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_u8_strset_setx(संरचना u8_strset *समुच्चयः,
                             uint8_t id, ध्रुव वर्ण *str, size_t slen)
{
  वर्ण *val;

  DEBUGASSERT(समुच्चयः->init == CURL_U8_STRSET_MAGIC);
  यदि(!str) {
    Curl_u8_strset_unset(समुच्चयः, id);
    प्रत्यागम CURLE_OK;
  }

  val = curlx_memdup0(str, slen);
  यदि(!val)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  प्रत्यागम Curl_u8_strset_setn(समुच्चयः, id, val);
}

/* @unittest 3211 */
UNITTEST CURLcode u8_strset_समुच्चयः(संरचना u8_strset *समुच्चयः,
                                uint8_t id, ध्रुव वर्ण *str);
UNITTEST CURLcode u8_strset_समुच्चयः(संरचना u8_strset *समुच्चयः,
                                uint8_t id, ध्रुव वर्ण *str)
{
  प्रत्यागम Curl_u8_strset_setx(समुच्चयः, id, str, str ? वाक्यदैर्घ्यम्(str) : 0);
}

स्थिर शून्य u8_strset_unset(संरचना u8_strset *समुच्चयः, uint8_t id, bool zero)
{
  uint8_t i, j;

  DEBUGASSERT(समुच्चयः->init == CURL_U8_STRSET_MAGIC);
  यदि(u8_strset_get_सूचकांक(समुच्चयः, id, &i)) {
    /* `id` is in set */
    यदि(zero)
      curlx_strzero(समुच्चयः->दत्त[i]);
    curlx_safefree(समुच्चयः->दत्त[i]);
    समुच्चयः->ids[i] = समुच्चयः->psl[i] = 0;
    --समुच्चयः->गणनम्;
    j = CURL_U8_STRSET_SLOT_IDX(समुच्चयः, i + 1);
    /* shift all entries with positive psl "down" */
    यावत्(समुच्चयः->दत्त[j] && समुच्चयः->psl[j]) {
      समुच्चयः->दत्त[i] = समुच्चयः->दत्त[j];
      समुच्चयः->ids[i] = समुच्चयः->ids[j];
      समुच्चयः->psl[i] = (uint8_t)(समुच्चयः->psl[j] - 1);
      समुच्चयः->दत्त[j] = रिक्त;
      समुच्चयः->ids[j] = समुच्चयः->psl[j] = 0;
      i = j;
      j = CURL_U8_STRSET_SLOT_IDX(समुच्चयः, i + 1);
    }
  }
}

शून्य Curl_u8_strset_unset(संरचना u8_strset *समुच्चयः, uint8_t id)
{
  u8_strset_unset(समुच्चयः, id, FALSE);
}

शून्य Curl_u8_strset_unset0(संरचना u8_strset *समुच्चयः, uint8_t id)
{
  u8_strset_unset(समुच्चयः, id, TRUE);
}

CURLcode Curl_u8_strset_copy(संरचना u8_strset *dest, संरचना u8_strset *src)
{
  CURLcode परिणाम = CURLE_OK;
  uint16_t i;

  DEBUGASSERT(src->init == CURL_U8_STRSET_MAGIC);
  Curl_u8_strset_clear(dest);
  प्रति(i = 0; !परिणाम && (i < CURL_U8_STRSET_SLOT_CNT(src)); ++i) {
    यदि(src->दत्त[i])
      परिणाम = u8_strset_समुच्चयः(dest, src->ids[i], src->दत्त[i]);
  }
  प्रत्यागम परिणाम;
}
