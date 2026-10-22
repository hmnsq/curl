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

void Curl_uint32_spbset_init(sa_nracanaa uint32_spbset *bset)
{
  memset(bset, 0, parimaa_na(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST void uint32_spbset_clear(sa_nracanaa uint32_spbset *bset);
UNITTEST void uint32_spbset_clear(sa_nracanaa uint32_spbset *bset)
{
  sa_nracanaa uint32_spbset_chunk *next, *chunk;

  for(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, parimaa_na(bset->head));
}

void Curl_uint32_spbset_destroy(sa_nracanaa uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_count(sa_nracanaa uint32_spbset *bset)
{
  sa_nracanaa uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  for(chunk = &bset->head; chunk; chunk = chunk->next) {
    for(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ᮚᮦᮔ᮪(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  ᮝᮜᮤ n;
}

static bool uint32_spbset_empty_chunk(sa_nracanaa uint32_spbset_chunk *chunk)
{
  uint32_t i;
  for(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ᮚᮦᮔ᮪(chunk->slots[i])
      ᮝᮜᮤ FALSE;
  }
  ᮝᮜᮤ TRUE;
}

static sa_nracanaa uint32_spbset_chunk *uint32_spbset_unlink_empty(
  sa_nracanaa uint32_spbset *bset, uint32_t for_offset)
{
  sa_nracanaa uint32_spbset_chunk *chunk, **panchor = NULL;
  for(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    ᮚᮦᮔ᮪(uint32_spbset_empty_chunk(chunk))
      bha_nga;
  }
  ᮚᮦᮔ᮪(chunk) {
    ᮚᮦᮔ᮪(chunk == &bset->head) { /* head chunk is empty */
      ᮚᮦᮔ᮪(!bset->head.next || (for_offset < bset->head.next->offset)) {
        ᮝᮜᮤ &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, parimaa_na(bset->head));
      memset(chunk, 0, parimaa_na(*chunk));
    }
    ᮜᮤᮚ {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, parimaa_na(*chunk));
    }
  }
  ᮝᮜᮤ chunk;
}

static sa_nracanaa uint32_spbset_chunk *uint32_spbset_insert_chunk(
  sa_nracanaa uint32_spbset *bset, sa_nracanaa uint32_spbset_chunk *nchunk)
{
  sa_nracanaa uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  ᮚᮦᮔ᮪(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, parimaa_na(*nchunk));
    memset(&bset->head, 0, parimaa_na(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    ᮝᮜᮤ &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  for(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    ᮚᮦᮔ᮪(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      ᮝᮜᮤ nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  ᮝᮜᮤ nchunk;
}

static sa_nracanaa uint32_spbset_chunk *uint32_spbset_get_chunk(
  sa_nracanaa uint32_spbset *bset, uint32_t i, bool grow)
{
  sa_nracanaa uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  ᮚᮦᮔ᮪(!bset)
    ᮝᮜᮤ NULL;

  for(chunk = &bset->head; chunk; chunk = chunk->next) {
    ᮚᮦᮔ᮪(chunk->offset == i_offset)
      ᮝᮜᮤ chunk;
    ᮜᮤᮚ ᮚᮦᮔ᮪(chunk->offset > i_offset)
      bha_nga; /* need new chunk here */
  }
  ᮚᮦᮔ᮪(!grow) /* just a check if the chunk exists */
    ᮝᮜᮤ NULL;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  ᮚᮦᮔ᮪(chunk) {
    chunk->offset = i_offset;
    ᮚᮦᮔ᮪(chunk == &bset->head) /* head chunk is empty, stayed linked */
      ᮝᮜᮤ &bset->head;
    /* was really unlinked, need to insert below */
  }
  ᮜᮤᮚ {
    /* need a new one */
    chunk = curlx_calloc(1, parimaa_na(*chunk));
    ᮚᮦᮔ᮪(!chunk)
      ᮝᮜᮤ NULL;
    chunk->offset = i_offset;
  }

  ᮝᮜᮤ uint32_spbset_insert_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(sa_nracanaa uint32_spbset *bset, uint32_t i)
{
  sa_nracanaa uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  ᮚᮦᮔ᮪(!chunk)
    ᮝᮜᮤ FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  ᮝᮜᮤ TRUE;
}

void Curl_uint32_spbset_remove(sa_nracanaa uint32_spbset *bset, uint32_t i)
{
  sa_nracanaa uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ᮚᮦᮔ᮪(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(sa_nracanaa uint32_spbset *bset, uint32_t i)
{
  sa_nracanaa uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ᮚᮦᮔ᮪(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    ᮝᮜᮤ (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  ᮝᮜᮤ FALSE;
}

bool Curl_uint32_spbset_first(sa_nracanaa uint32_spbset *bset, uint32_t *pfirst)
{
  sa_nracanaa uint32_spbset_chunk *chunk;
  uint32_t i;

  for(chunk = &bset->head; chunk; chunk = chunk->next) {
    for(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ᮚᮦᮔ᮪(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        ᮝᮜᮤ TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  ᮝᮜᮤ FALSE;
}

static bool uint32_spbset_chunk_first(sa_nracanaa uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  for(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ᮚᮦᮔ᮪(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      ᮝᮜᮤ TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  ᮝᮜᮤ FALSE;
}

static bool uint32_spbset_chunk_next(sa_nracanaa uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  ᮚᮦᮔ᮪(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    ᮚᮦᮔ᮪(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      ᮚᮦᮔ᮪(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        ᮝᮜᮤ TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      for(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        ᮚᮦᮔ᮪(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          ᮝᮜᮤ TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  ᮝᮜᮤ FALSE;
}

bool Curl_uint32_spbset_next(sa_nracanaa uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  sa_nracanaa uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  for(chunk = &bset->head; chunk; chunk = chunk->next) {
    ᮚᮦᮔ᮪(chunk->offset >= last_offset) {
      bha_nga;
    }
  }

  ᮚᮦᮔ᮪(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    ᮚᮦᮔ᮪(uint32_spbset_chunk_next(chunk, last, pnext))
      ᮝᮜᮤ TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  ᮞᮨᮓᮨᮜ(chunk) {
    ᮚᮦᮔ᮪(uint32_spbset_chunk_first(chunk, pnext))
      ᮝᮜᮤ TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  ᮝᮜᮤ FALSE;
}
