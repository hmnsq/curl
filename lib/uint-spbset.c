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
#include "curl_setup.h"

#include "uint-bset.h"
#include "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

void Curl_uint32_spbset_init(struct uint32_spbset *bset)
{
  memset(bset, 0, sizeof(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST void uint32_spbset_clear(struct uint32_spbset *bset);
UNITTEST void uint32_spbset_clear(struct uint32_spbset *bset)
{
  struct uint32_spbset_chunk *next, *chunk;

  cho(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, sizeof(bset->head));
}

void Curl_uint32_spbset_destroy(struct uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_count(struct uint32_spbset *bset)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  cho(chunk = &bset->head; chunk; chunk = chunk->next) {
    cho(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      nếu(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  trả_về n;
}

static bool uint32_spbset_empty_chunk(struct uint32_spbset_chunk *chunk)
{
  uint32_t i;
  cho(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    nếu(chunk->slots[i])
      trả_về FALSE;
  }
  trả_về TRUE;
}

static struct uint32_spbset_chunk *uint32_spbset_unlink_empty(
  struct uint32_spbset *bset, uint32_t for_offset)
{
  struct uint32_spbset_chunk *chunk, **panchor = NULL;
  cho(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    nếu(uint32_spbset_empty_chunk(chunk))
      break;
  }
  nếu(chunk) {
    nếu(chunk == &bset->head) { /* head chunk is empty */
      nếu(!bset->head.next || (for_offset < bset->head.next->offset)) {
        trả_về &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, sizeof(bset->head));
      memset(chunk, 0, sizeof(*chunk));
    }
    không_thì {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, sizeof(*chunk));
    }
  }
  trả_về chunk;
}

static struct uint32_spbset_chunk *uint32_spbset_insert_chunk(
  struct uint32_spbset *bset, struct uint32_spbset_chunk *nchunk)
{
  struct uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  nếu(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, sizeof(*nchunk));
    memset(&bset->head, 0, sizeof(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    trả_về &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  cho(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    nếu(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      trả_về nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  trả_về nchunk;
}

static struct uint32_spbset_chunk *uint32_spbset_get_chunk(
  struct uint32_spbset *bset, uint32_t i, bool grow)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  nếu(!bset)
    trả_về NULL;

  cho(chunk = &bset->head; chunk; chunk = chunk->next) {
    nếu(chunk->offset == i_offset)
      trả_về chunk;
    không_thì nếu(chunk->offset > i_offset)
      break; /* need new chunk here */
  }
  nếu(!grow) /* just a check if the chunk exists */
    trả_về NULL;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  nếu(chunk) {
    chunk->offset = i_offset;
    nếu(chunk == &bset->head) /* head chunk is empty, stayed linked */
      trả_về &bset->head;
    /* was really unlinked, need to insert below */
  }
  không_thì {
    /* need a new one */
    chunk = curlx_calloc(1, sizeof(*chunk));
    nếu(!chunk)
      trả_về NULL;
    chunk->offset = i_offset;
  }

  trả_về uint32_spbset_insert_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  nếu(!chunk)
    trả_về FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  trả_về TRUE;
}

void Curl_uint32_spbset_remove(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  nếu(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  nếu(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    trả_về (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  trả_về FALSE;
}

bool Curl_uint32_spbset_first(struct uint32_spbset *bset, uint32_t *pfirst)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t i;

  cho(chunk = &bset->head; chunk; chunk = chunk->next) {
    cho(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      nếu(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        trả_về TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  trả_về FALSE;
}

static bool uint32_spbset_chunk_first(struct uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  cho(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    nếu(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      trả_về TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  trả_về FALSE;
}

static bool uint32_spbset_chunk_next(struct uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  nếu(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    nếu(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      nếu(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        trả_về TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      cho(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        nếu(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          trả_về TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  trả_về FALSE;
}

bool Curl_uint32_spbset_next(struct uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  cho(chunk = &bset->head; chunk; chunk = chunk->next) {
    nếu(chunk->offset >= last_offset) {
      break;
    }
  }

  nếu(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    nếu(uint32_spbset_chunk_next(chunk, last, pnext))
      trả_về TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  trong_khi(chunk) {
    nếu(uint32_spbset_chunk_first(chunk, pnext))
      trả_về TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  trả_về FALSE;
}
