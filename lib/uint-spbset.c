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

  为(chunk = bset->head.next; chunk; chunk = next) {
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

  为(chunk = &bset->head; chunk; chunk = chunk->next) {
    为(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      如果(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  返回 n;
}

static bool uint32_spbset_empty_chunk(struct uint32_spbset_chunk *chunk)
{
  uint32_t i;
  为(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    如果(chunk->slots[i])
      返回 FALSE;
  }
  返回 TRUE;
}

static struct uint32_spbset_chunk *uint32_spbset_unlink_empty(
  struct uint32_spbset *bset, uint32_t for_offset)
{
  struct uint32_spbset_chunk *chunk, **panchor = NULL;
  为(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    如果(uint32_spbset_empty_chunk(chunk))
      break;
  }
  如果(chunk) {
    如果(chunk == &bset->head) { /* head chunk is empty */
      如果(!bset->head.next || (for_offset < bset->head.next->offset)) {
        返回 &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, sizeof(bset->head));
      memset(chunk, 0, sizeof(*chunk));
    }
    否则 {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, sizeof(*chunk));
    }
  }
  返回 chunk;
}

static struct uint32_spbset_chunk *uint32_spbset_insert_chunk(
  struct uint32_spbset *bset, struct uint32_spbset_chunk *nchunk)
{
  struct uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  如果(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, sizeof(*nchunk));
    memset(&bset->head, 0, sizeof(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    返回 &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  为(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    如果(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      返回 nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  返回 nchunk;
}

static struct uint32_spbset_chunk *uint32_spbset_get_chunk(
  struct uint32_spbset *bset, uint32_t i, bool grow)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  如果(!bset)
    返回 NULL;

  为(chunk = &bset->head; chunk; chunk = chunk->next) {
    如果(chunk->offset == i_offset)
      返回 chunk;
    否则 如果(chunk->offset > i_offset)
      break; /* need new chunk here */
  }
  如果(!grow) /* just a check if the chunk exists */
    返回 NULL;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  如果(chunk) {
    chunk->offset = i_offset;
    如果(chunk == &bset->head) /* head chunk is empty, stayed linked */
      返回 &bset->head;
    /* was really unlinked, need to insert below */
  }
  否则 {
    /* need a new one */
    chunk = curlx_calloc(1, sizeof(*chunk));
    如果(!chunk)
      返回 NULL;
    chunk->offset = i_offset;
  }

  返回 uint32_spbset_insert_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  如果(!chunk)
    返回 FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  返回 TRUE;
}

void Curl_uint32_spbset_remove(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  如果(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  如果(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    返回 (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  返回 FALSE;
}

bool Curl_uint32_spbset_first(struct uint32_spbset *bset, uint32_t *pfirst)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t i;

  为(chunk = &bset->head; chunk; chunk = chunk->next) {
    为(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      如果(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        返回 TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  返回 FALSE;
}

static bool uint32_spbset_chunk_first(struct uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  为(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    如果(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      返回 TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  返回 FALSE;
}

static bool uint32_spbset_chunk_next(struct uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  如果(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    如果(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      如果(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        返回 TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      为(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        如果(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          返回 TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  返回 FALSE;
}

bool Curl_uint32_spbset_next(struct uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  为(chunk = &bset->head; chunk; chunk = chunk->next) {
    如果(chunk->offset >= last_offset) {
      break;
    }
  }

  如果(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    如果(uint32_spbset_chunk_next(chunk, last, pnext))
      返回 TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  当(chunk) {
    如果(uint32_spbset_chunk_first(chunk, pnext))
      返回 TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  返回 FALSE;
}
