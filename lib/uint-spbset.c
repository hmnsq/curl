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

#अन्तर्भाव "uint-bset.h"
#अन्तर्भाव "uint-spbset.h"

#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय CURL_UINT32_SPBSET_MAGIC  0x70737362
#यदि_अन्तः

शून्य Curl_uint32_spbset_init(संरचना uint32_spbset *bset)
{
  स्मृतिं_पूरय(bset, 0, परिमाण(*bset));
#यदि_परिभाषितम् DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#यदि_अन्तः
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST शून्य uint32_spbset_clear(संरचना uint32_spbset *bset);
UNITTEST शून्य uint32_spbset_clear(संरचना uint32_spbset *bset)
{
  संरचना uint32_spbset_chunk *अग्र, *chunk;

  प्रति(chunk = bset->शीर्ष.अग्र; chunk; chunk = अग्र) {
    अग्र = chunk->अग्र;
    curlx_free(chunk);
  }
  स्मृतिं_पूरय(&bset->शीर्ष, 0, परिमाण(bset->शीर्ष));
}

शून्य Curl_uint32_spbset_destroy(संरचना uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_गणनम्(संरचना uint32_spbset *bset)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  प्रति(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    प्रति(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      यदि(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  प्रत्यागम n;
}

स्थिर bool uint32_spbset_रिक्तानि_chunk(संरचना uint32_spbset_chunk *chunk)
{
  uint32_t i;
  प्रति(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    यदि(chunk->slots[i])
      प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_unlink_रिक्तानि(
  संरचना uint32_spbset *bset, uint32_t for_विस्थापनम्)
{
  संरचना uint32_spbset_chunk *chunk, **panchor = रिक्त;
  प्रति(chunk = &bset->शीर्ष; chunk;
      panchor = &chunk->अग्र, chunk = chunk->अग्र) {
    यदि(uint32_spbset_रिक्तानि_chunk(chunk))
      भङ्ग;
  }
  यदि(chunk) {
    यदि(chunk == &bset->शीर्ष) { /* head chunk is empty */
      यदि(!bset->शीर्ष.अग्र || (for_विस्थापनम् < bset->शीर्ष.अग्र->विस्थापनम्)) {
        प्रत्यागम &bset->शीर्ष;
      }
      /* swap head and next, unlink */
      chunk = bset->शीर्ष.अग्र;
      स्मृतिं_प्रतिलिख(&bset->शीर्ष, chunk, परिमाण(bset->शीर्ष));
      स्मृतिं_पूरय(chunk, 0, परिमाण(*chunk));
    }
    अन्यथा {
      *panchor = chunk->अग्र; /* unlink */
      स्मृतिं_पूरय(chunk, 0, परिमाण(*chunk));
    }
  }
  प्रत्यागम chunk;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_निवेशय_chunk(
  संरचना uint32_spbset *bset, संरचना uint32_spbset_chunk *nchunk)
{
  संरचना uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  यदि(nchunk->विस्थापनम् < bset->शीर्ष.विस्थापनम्) {
    /* swap chunk and head */
    uint32_t विस्थापनम् = nchunk->विस्थापनम्;
    स्मृतिं_प्रतिलिख(nchunk, &bset->शीर्ष, परिमाण(*nchunk));
    स्मृतिं_पूरय(&bset->शीर्ष, 0, परिमाण(bset->शीर्ष));
    bset->शीर्ष.अग्र = nchunk;
    bset->शीर्ष.विस्थापनम् = विस्थापनम्;
    प्रत्यागम &bset->शीर्ष;
  }
  DEBUGASSERT(nchunk->विस्थापनम् > bset->शीर्ष.विस्थापनम्);
  panchor = &bset->शीर्ष.अग्र;
  प्रति(chunk = *panchor; chunk;
      panchor = &chunk->अग्र, chunk = chunk->अग्र) {
    यदि(chunk->विस्थापनम् > nchunk->विस्थापनम्) { /* insert before this chunk */
      nchunk->अग्र = chunk;
      *panchor = nchunk;
      प्रत्यागम nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  प्रत्यागम nchunk;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_get_chunk(
  संरचना uint32_spbset *bset, uint32_t i, bool grow)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i_विस्थापनम् = (i & ~CURL_UINT32_SPBSET_CH_आवरणम्);

  यदि(!bset)
    प्रत्यागम रिक्त;

  प्रति(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    यदि(chunk->विस्थापनम् == i_विस्थापनम्)
      प्रत्यागम chunk;
    अन्यथा यदि(chunk->विस्थापनम् > i_विस्थापनम्)
      भङ्ग; /* need new chunk here */
  }
  यदि(!grow) /* just a check if the chunk exists */
    प्रत्यागम रिक्त;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_रिक्तानि(bset, i_विस्थापनम्);
  यदि(chunk) {
    chunk->विस्थापनम् = i_विस्थापनम्;
    यदि(chunk == &bset->शीर्ष) /* head chunk is empty, stayed linked */
      प्रत्यागम &bset->शीर्ष;
    /* was really unlinked, need to insert below */
  }
  अन्यथा {
    /* need a new one */
    chunk = curlx_calloc(1, परिमाण(*chunk));
    यदि(!chunk)
      प्रत्यागम रिक्त;
    chunk->विस्थापनम् = i_विस्थापनम्;
  }

  प्रत्यागम uint32_spbset_निवेशय_chunk(bset, chunk);
}

bool Curl_uint32_spbset_सङ्कलय(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  यदि(!chunk)
    प्रत्यागम FALSE;

  DEBUGASSERT(i >= chunk->विस्थापनम्);
  i -= chunk->विस्थापनम्;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  प्रत्यागम TRUE;
}

शून्य Curl_uint32_spbset_remove(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  यदि(chunk) {
    DEBUGASSERT(i >= chunk->विस्थापनम्);
    i -= chunk->विस्थापनम्;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_अन्तर्भवति(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  यदि(chunk) {
    DEBUGASSERT(i >= chunk->विस्थापनम्);
    i -= chunk->विस्थापनम्;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    प्रत्यागम (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  प्रत्यागम FALSE;
}

bool Curl_uint32_spbset_first(संरचना uint32_spbset *bset, uint32_t *pfirst)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i;

  प्रति(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    प्रति(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      यदि(chunk->slots[i]) {
        *pfirst = chunk->विस्थापनम् + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        प्रत्यागम TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  प्रत्यागम FALSE;
}

स्थिर bool uint32_spbset_chunk_first(संरचना uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  प्रति(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    यदि(chunk->slots[i]) {
      *pfirst = chunk->विस्थापनम् + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      प्रत्यागम TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  प्रत्यागम FALSE;
}

स्थिर bool uint32_spbset_chunk_अग्र(संरचना uint32_spbset_chunk *chunk,
                                     uint32_t चरमः,
                                     uint32_t *pnext)
{
  यदि(chunk->विस्थापनम् <= चरमः) {
    uint64_t x;
    uint32_t i = ((चरमः - chunk->विस्थापनम्) / 64);
    यदि(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (चरमः % 64));
      यदि(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = चरमः + CURL_CTZ64(x);
        प्रत्यागम TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      प्रति(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        यदि(chunk->slots[i]) {
          *pnext = chunk->विस्थापनम् + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          प्रत्यागम TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  प्रत्यागम FALSE;
}

bool Curl_uint32_spbset_अग्र(संरचना uint32_spbset *bset, uint32_t चरमः,
                             uint32_t *pnext)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t चरमः_विस्थापनम्;

  ++चरमः; /* look for the next higher number */
  चरमः_विस्थापनम् = (चरमः & ~CURL_UINT32_SPBSET_CH_आवरणम्);

  प्रति(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    यदि(chunk->विस्थापनम् >= चरमः_विस्थापनम्) {
      भङ्ग;
    }
  }

  यदि(chunk && (chunk->विस्थापनम् == चरमः_विस्थापनम्)) {
    /* is there a number higher than last in this chunk? */
    यदि(uint32_spbset_chunk_अग्र(chunk, चरमः, pnext))
      प्रत्यागम TRUE;
    /* not in this chunk */
    chunk = chunk->अग्र;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  यावत्(chunk) {
    यदि(uint32_spbset_chunk_first(chunk, pnext))
      प्रत्यागम TRUE;
    chunk = chunk->अग्र;
  }
  *pnext = UINT32_MAX;
  प्रत्यागम FALSE;
}
