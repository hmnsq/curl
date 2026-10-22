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
#ضمن "curl_setup.h"

#ضمن "uint-bset.h"
#ضمن "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

فراغ Curl_uint32_spbset_init(بنية uint32_spbset *bset)
{
  memset(bset, 0, حجم(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset);
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *next, *chunk;

  עבור(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, حجم(bset->head));
}

فراغ Curl_uint32_spbset_destroy(بنية uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_count(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  עבור(chunk = &bset->head; chunk; chunk = chunk->next) {
    עבור(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      אם(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  החזר n;
}

ساكن bool uint32_spbset_empty_chunk(بنية uint32_spbset_chunk *chunk)
{
  uint32_t i;
  עבור(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    אם(chunk->slots[i])
      החזר FALSE;
  }
  החזר TRUE;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_unlink_empty(
  بنية uint32_spbset *bset, uint32_t for_offset)
{
  بنية uint32_spbset_chunk *chunk, **panchor = عدم;
  עבור(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    אם(uint32_spbset_empty_chunk(chunk))
      اكسر;
  }
  אם(chunk) {
    אם(chunk == &bset->head) { /* head chunk is empty */
      אם(!bset->head.next || (for_offset < bset->head.next->offset)) {
        החזר &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, حجم(bset->head));
      memset(chunk, 0, حجم(*chunk));
    }
    אחרת {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, حجم(*chunk));
    }
  }
  החזר chunk;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_insert_chunk(
  بنية uint32_spbset *bset, بنية uint32_spbset_chunk *nchunk)
{
  بنية uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  אם(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, حجم(*nchunk));
    memset(&bset->head, 0, حجم(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    החזר &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  עבור(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    אם(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      החזר nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  החזר nchunk;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_get_chunk(
  بنية uint32_spbset *bset, uint32_t i, bool grow)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  אם(!bset)
    החזר عدم;

  עבור(chunk = &bset->head; chunk; chunk = chunk->next) {
    אם(chunk->offset == i_offset)
      החזר chunk;
    אחרת אם(chunk->offset > i_offset)
      اكسر; /* need new chunk here */
  }
  אם(!grow) /* just a check if the chunk exists */
    החזר عدم;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  אם(chunk) {
    chunk->offset = i_offset;
    אם(chunk == &bset->head) /* head chunk is empty, stayed linked */
      החזר &bset->head;
    /* was really unlinked, need to insert below */
  }
  אחרת {
    /* need a new one */
    chunk = curlx_calloc(1, حجم(*chunk));
    אם(!chunk)
      החזר عدم;
    chunk->offset = i_offset;
  }

  החזר uint32_spbset_insert_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  אם(!chunk)
    החזר FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  החזר TRUE;
}

فراغ Curl_uint32_spbset_remove(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  אם(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  אם(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    החזר (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  החזר FALSE;
}

bool Curl_uint32_spbset_first(بنية uint32_spbset *bset, uint32_t *pfirst)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i;

  עבור(chunk = &bset->head; chunk; chunk = chunk->next) {
    עבור(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      אם(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        החזר TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  החזר FALSE;
}

ساكن bool uint32_spbset_chunk_first(بنية uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  עבור(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    אם(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      החזר TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  החזר FALSE;
}

ساكن bool uint32_spbset_chunk_next(بنية uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  אם(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    אם(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      אם(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        החזר TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      עבור(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        אם(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          החזר TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  החזר FALSE;
}

bool Curl_uint32_spbset_next(بنية uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  עבור(chunk = &bset->head; chunk; chunk = chunk->next) {
    אם(chunk->offset >= last_offset) {
      اكسر;
    }
  }

  אם(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    אם(uint32_spbset_chunk_next(chunk, last, pnext))
      החזר TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  כלעוד(chunk) {
    אם(uint32_spbset_chunk_first(chunk, pnext))
      החזר TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  החזר FALSE;
}
