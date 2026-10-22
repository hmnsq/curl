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

  ために(chunk = bset->head.next; chunk; chunk = next) {
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

  ために(chunk = &bset->head; chunk; chunk = chunk->next) {
    ために(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      もし(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  戻る n;
}

static bool uint32_spbset_empty_chunk(struct uint32_spbset_chunk *chunk)
{
  uint32_t i;
  ために(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    もし(chunk->slots[i])
      戻る FALSE;
  }
  戻る TRUE;
}

static struct uint32_spbset_chunk *uint32_spbset_unlink_empty(
  struct uint32_spbset *bset, uint32_t for_offset)
{
  struct uint32_spbset_chunk *chunk, **panchor = NULL;
  ために(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    もし(uint32_spbset_empty_chunk(chunk))
      break;
  }
  もし(chunk) {
    もし(chunk == &bset->head) { /* head chunk is empty */
      もし(!bset->head.next || (for_offset < bset->head.next->offset)) {
        戻る &bset->head;
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
  戻る chunk;
}

static struct uint32_spbset_chunk *uint32_spbset_insert_chunk(
  struct uint32_spbset *bset, struct uint32_spbset_chunk *nchunk)
{
  struct uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  もし(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, sizeof(*nchunk));
    memset(&bset->head, 0, sizeof(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    戻る &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  ために(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    もし(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      戻る nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  戻る nchunk;
}

static struct uint32_spbset_chunk *uint32_spbset_get_chunk(
  struct uint32_spbset *bset, uint32_t i, bool grow)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  もし(!bset)
    戻る NULL;

  ために(chunk = &bset->head; chunk; chunk = chunk->next) {
    もし(chunk->offset == i_offset)
      戻る chunk;
    否则 もし(chunk->offset > i_offset)
      break; /* need new chunk here */
  }
  もし(!grow) /* just a check if the chunk exists */
    戻る NULL;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  もし(chunk) {
    chunk->offset = i_offset;
    もし(chunk == &bset->head) /* head chunk is empty, stayed linked */
      戻る &bset->head;
    /* was really unlinked, need to insert below */
  }
  否则 {
    /* need a new one */
    chunk = curlx_calloc(1, sizeof(*chunk));
    もし(!chunk)
      戻る NULL;
    chunk->offset = i_offset;
  }

  戻る uint32_spbset_insert_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  もし(!chunk)
    戻る FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  戻る TRUE;
}

void Curl_uint32_spbset_remove(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  もし(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(struct uint32_spbset *bset, uint32_t i)
{
  struct uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  もし(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    戻る (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  戻る FALSE;
}

bool Curl_uint32_spbset_first(struct uint32_spbset *bset, uint32_t *pfirst)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t i;

  ために(chunk = &bset->head; chunk; chunk = chunk->next) {
    ために(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      もし(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        戻る TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  戻る FALSE;
}

static bool uint32_spbset_chunk_first(struct uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  ために(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    もし(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      戻る TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  戻る FALSE;
}

static bool uint32_spbset_chunk_next(struct uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  もし(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    もし(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      もし(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        戻る TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      ために(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        もし(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          戻る TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  戻る FALSE;
}

bool Curl_uint32_spbset_next(struct uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  struct uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  ために(chunk = &bset->head; chunk; chunk = chunk->next) {
    もし(chunk->offset >= last_offset) {
      break;
    }
  }

  もし(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    もし(uint32_spbset_chunk_next(chunk, last, pnext))
      戻る TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  ながら(chunk) {
    もし(uint32_spbset_chunk_first(chunk, pnext))
      戻る TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  戻る FALSE;
}
