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

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

शून्य Curl_uint32_spbset_init(संरचना uint32_spbset *bset)
{
  memset(bset, 0, परिमाण(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST शून्य uint32_spbset_clear(संरचना uint32_spbset *bset);
UNITTEST शून्य uint32_spbset_clear(संरचना uint32_spbset *bset)
{
  संरचना uint32_spbset_chunk *next, *chunk;

  प्रति(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, परिमाण(bset->head));
}

शून्य Curl_uint32_spbset_destroy(संरचना uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_मोजणी(संरचना uint32_spbset *bset)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  प्रति(chunk = &bset->head; chunk; chunk = chunk->next) {
    प्रति(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      यदि(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  प्रत्यागम n;
}

स्थिर bool uint32_spbset_empty_chunk(संरचना uint32_spbset_chunk *chunk)
{
  uint32_t i;
  प्रति(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    यदि(chunk->slots[i])
      प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_unlink_empty(
  संरचना uint32_spbset *bset, uint32_t for_offset)
{
  संरचना uint32_spbset_chunk *chunk, **panchor = रिक्त;
  प्रति(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    यदि(uint32_spbset_empty_chunk(chunk))
      भङ्ग;
  }
  यदि(chunk) {
    यदि(chunk == &bset->head) { /* head chunk is empty */
      यदि(!bset->head.next || (for_offset < bset->head.next->offset)) {
        प्रत्यागम &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, परिमाण(bset->head));
      memset(chunk, 0, परिमाण(*chunk));
    }
    अन्यथा {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, परिमाण(*chunk));
    }
  }
  प्रत्यागम chunk;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_घाला_chunk(
  संरचना uint32_spbset *bset, संरचना uint32_spbset_chunk *nchunk)
{
  संरचना uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  यदि(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, परिमाण(*nchunk));
    memset(&bset->head, 0, परिमाण(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    प्रत्यागम &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  प्रति(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    यदि(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
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
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  यदि(!bset)
    प्रत्यागम रिक्त;

  प्रति(chunk = &bset->head; chunk; chunk = chunk->next) {
    यदि(chunk->offset == i_offset)
      प्रत्यागम chunk;
    अन्यथा यदि(chunk->offset > i_offset)
      भङ्ग; /* need new chunk here */
  }
  यदि(!grow) /* just a check if the chunk exists */
    प्रत्यागम रिक्त;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  यदि(chunk) {
    chunk->offset = i_offset;
    यदि(chunk == &bset->head) /* head chunk is empty, stayed linked */
      प्रत्यागम &bset->head;
    /* was really unlinked, need to insert below */
  }
  अन्यथा {
    /* need a new one */
    chunk = curlx_calloc(1, परिमाण(*chunk));
    यदि(!chunk)
      प्रत्यागम रिक्त;
    chunk->offset = i_offset;
  }

  प्रत्यागम uint32_spbset_घाला_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  यदि(!chunk)
    प्रत्यागम FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  प्रत्यागम TRUE;
}

शून्य Curl_uint32_spbset_remove(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  यदि(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  यदि(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    प्रत्यागम (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  प्रत्यागम FALSE;
}

bool Curl_uint32_spbset_first(संरचना uint32_spbset *bset, uint32_t *pfirst)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i;

  प्रति(chunk = &bset->head; chunk; chunk = chunk->next) {
    प्रति(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      यदि(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
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
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      प्रत्यागम TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  प्रत्यागम FALSE;
}

स्थिर bool uint32_spbset_chunk_next(संरचना uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  यदि(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    यदि(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      यदि(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        प्रत्यागम TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      प्रति(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        यदि(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          प्रत्यागम TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  प्रत्यागम FALSE;
}

bool Curl_uint32_spbset_next(संरचना uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  प्रति(chunk = &bset->head; chunk; chunk = chunk->next) {
    यदि(chunk->offset >= last_offset) {
      भङ्ग;
    }
  }

  यदि(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    यदि(uint32_spbset_chunk_next(chunk, last, pnext))
      प्रत्यागम TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  यावत्(chunk) {
    यदि(uint32_spbset_chunk_first(chunk, pnext))
      प्रत्यागम TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  प्रत्यागम FALSE;
}
