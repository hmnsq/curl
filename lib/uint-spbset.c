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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "uint-bset.h"
#ଅନ୍ତର୍ଭାଵ "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

ଶୂନ୍ୟ Curl_uint32_spbset_init(ସଂରଚନା uint32_spbset *bset)
{
  memset(bset, 0, ଆକାର(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST ଶୂନ୍ୟ uint32_spbset_clear(ସଂରଚନା uint32_spbset *bset);
UNITTEST ଶୂନ୍ୟ uint32_spbset_clear(ସଂରଚନା uint32_spbset *bset)
{
  ସଂରଚନା uint32_spbset_chunk *next, *chunk;

  ପାଇଁ(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, ଆକାର(bset->head));
}

ଶୂନ୍ୟ Curl_uint32_spbset_destroy(ସଂରଚନା uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_ଗଣନା(ସଂରଚନା uint32_spbset *bset)
{
  ସଂରଚନା uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  ପାଇଁ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ପାଇଁ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ଯଦି(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  ଫେରନ୍ତୁ n;
}

ସ୍ଥିର bool uint32_spbset_empty_chunk(ସଂରଚନା uint32_spbset_chunk *chunk)
{
  uint32_t i;
  ପାଇଁ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ଯଦି(chunk->slots[i])
      ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ସଂରଚନା uint32_spbset_chunk *uint32_spbset_unlink_empty(
  ସଂରଚନା uint32_spbset *bset, uint32_t for_offset)
{
  ସଂରଚନା uint32_spbset_chunk *chunk, **panchor = ଖାଲି;
  ପାଇଁ(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    ଯଦି(uint32_spbset_empty_chunk(chunk))
      ଭାଙ୍ଗ;
  }
  ଯଦି(chunk) {
    ଯଦି(chunk == &bset->head) { /* head chunk is empty */
      ଯଦି(!bset->head.next || (for_offset < bset->head.next->offset)) {
        ଫେରନ୍ତୁ &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, ଆକାର(bset->head));
      memset(chunk, 0, ଆକାର(*chunk));
    }
    ଅନ୍ୟଥା {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, ଆକାର(*chunk));
    }
  }
  ଫେରନ୍ତୁ chunk;
}

ସ୍ଥିର ସଂରଚନା uint32_spbset_chunk *uint32_spbset_ଭର୍ତ୍ତି_chunk(
  ସଂରଚନା uint32_spbset *bset, ସଂରଚନା uint32_spbset_chunk *nchunk)
{
  ସଂରଚନା uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  ଯଦି(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, ଆକାର(*nchunk));
    memset(&bset->head, 0, ଆକାର(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    ଫେରନ୍ତୁ &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  ପାଇଁ(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    ଯଦି(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      ଫେରନ୍ତୁ nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  ଫେରନ୍ତୁ nchunk;
}

ସ୍ଥିର ସଂରଚନା uint32_spbset_chunk *uint32_spbset_get_chunk(
  ସଂରଚନା uint32_spbset *bset, uint32_t i, bool grow)
{
  ସଂରଚନା uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  ଯଦି(!bset)
    ଫେରନ୍ତୁ ଖାଲି;

  ପାଇଁ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ଯଦି(chunk->offset == i_offset)
      ଫେରନ୍ତୁ chunk;
    ଅନ୍ୟଥା ଯଦି(chunk->offset > i_offset)
      ଭାଙ୍ଗ; /* need new chunk here */
  }
  ଯଦି(!grow) /* just a check if the chunk exists */
    ଫେରନ୍ତୁ ଖାଲି;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  ଯଦି(chunk) {
    chunk->offset = i_offset;
    ଯଦି(chunk == &bset->head) /* head chunk is empty, stayed linked */
      ଫେରନ୍ତୁ &bset->head;
    /* was really unlinked, need to insert below */
  }
  ଅନ୍ୟଥା {
    /* need a new one */
    chunk = curlx_calloc(1, ଆକାର(*chunk));
    ଯଦି(!chunk)
      ଫେରନ୍ତୁ ଖାଲି;
    chunk->offset = i_offset;
  }

  ଫେରନ୍ତୁ uint32_spbset_ଭର୍ତ୍ତି_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(ସଂରଚନା uint32_spbset *bset, uint32_t i)
{
  ସଂରଚନା uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  ଯଦି(!chunk)
    ଫେରନ୍ତୁ FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  ଫେରନ୍ତୁ TRUE;
}

ଶୂନ୍ୟ Curl_uint32_spbset_remove(ସଂରଚନା uint32_spbset *bset, uint32_t i)
{
  ସଂରଚନା uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ଯଦି(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(ସଂରଚନା uint32_spbset *bset, uint32_t i)
{
  ସଂରଚନା uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ଯଦି(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    ଫେରନ୍ତୁ (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_uint32_spbset_first(ସଂରଚନା uint32_spbset *bset, uint32_t *pfirst)
{
  ସଂରଚନା uint32_spbset_chunk *chunk;
  uint32_t i;

  ପାଇଁ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ପାଇଁ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ଯଦି(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        ଫେରନ୍ତୁ TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool uint32_spbset_chunk_first(ସଂରଚନା uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  ପାଇଁ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ଯଦି(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      ଫେରନ୍ତୁ TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool uint32_spbset_chunk_next(ସଂରଚନା uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  ଯଦି(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    ଯଦି(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      ଯଦି(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        ଫେରନ୍ତୁ TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      ପାଇଁ(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        ଯଦି(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          ଫେରନ୍ତୁ TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_uint32_spbset_next(ସଂରଚନା uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  ସଂରଚନା uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  ପାଇଁ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ଯଦି(chunk->offset >= last_offset) {
      ଭାଙ୍ଗ;
    }
  }

  ଯଦି(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    ଯଦି(uint32_spbset_chunk_next(chunk, last, pnext))
      ଫେରନ୍ତୁ TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  ଯେତେବେଳେ(chunk) {
    ଯଦି(uint32_spbset_chunk_first(chunk, pnext))
      ଫେରନ୍ତୁ TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  ଫେରନ୍ତୁ FALSE;
}
