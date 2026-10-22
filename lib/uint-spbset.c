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
#شامل "curl_setup.h"

#شامل "uint-bset.h"
#شامل "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

تهی Curl_uint32_spbset_init(ساختار uint32_spbset *bset)
{
  memset(bset, 0, اندازه(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST تهی uint32_spbset_clear(ساختار uint32_spbset *bset);
UNITTEST تهی uint32_spbset_clear(ساختار uint32_spbset *bset)
{
  ساختار uint32_spbset_chunk *next, *chunk;

  برای(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, اندازه(bset->head));
}

تهی Curl_uint32_spbset_destroy(ساختار uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_شمار(ساختار uint32_spbset *bset)
{
  ساختار uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  برای(chunk = &bset->head; chunk; chunk = chunk->next) {
    برای(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      اگر(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  بازگشت n;
}

ایستا bool uint32_spbset_empty_chunk(ساختار uint32_spbset_chunk *chunk)
{
  uint32_t i;
  برای(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    اگر(chunk->slots[i])
      بازگشت FALSE;
  }
  بازگشت TRUE;
}

ایستا ساختار uint32_spbset_chunk *uint32_spbset_unlink_empty(
  ساختار uint32_spbset *bset, uint32_t for_offset)
{
  ساختار uint32_spbset_chunk *chunk, **panchor = پوچ;
  برای(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    اگر(uint32_spbset_empty_chunk(chunk))
      بشکن;
  }
  اگر(chunk) {
    اگر(chunk == &bset->head) { /* head chunk is empty */
      اگر(!bset->head.next || (for_offset < bset->head.next->offset)) {
        بازگشت &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, اندازه(bset->head));
      memset(chunk, 0, اندازه(*chunk));
    }
    وگرنه {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, اندازه(*chunk));
    }
  }
  بازگشت chunk;
}

ایستا ساختار uint32_spbset_chunk *uint32_spbset_درج_chunk(
  ساختار uint32_spbset *bset, ساختار uint32_spbset_chunk *nchunk)
{
  ساختار uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  اگر(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, اندازه(*nchunk));
    memset(&bset->head, 0, اندازه(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    بازگشت &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  برای(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    اگر(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      بازگشت nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  بازگشت nchunk;
}

ایستا ساختار uint32_spbset_chunk *uint32_spbset_get_chunk(
  ساختار uint32_spbset *bset, uint32_t i, bool grow)
{
  ساختار uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  اگر(!bset)
    بازگشت پوچ;

  برای(chunk = &bset->head; chunk; chunk = chunk->next) {
    اگر(chunk->offset == i_offset)
      بازگشت chunk;
    وگرنه اگر(chunk->offset > i_offset)
      بشکن; /* need new chunk here */
  }
  اگر(!grow) /* just a check if the chunk exists */
    بازگشت پوچ;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  اگر(chunk) {
    chunk->offset = i_offset;
    اگر(chunk == &bset->head) /* head chunk is empty, stayed linked */
      بازگشت &bset->head;
    /* was really unlinked, need to insert below */
  }
  وگرنه {
    /* need a new one */
    chunk = curlx_calloc(1, اندازه(*chunk));
    اگر(!chunk)
      بازگشت پوچ;
    chunk->offset = i_offset;
  }

  بازگشت uint32_spbset_درج_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(ساختار uint32_spbset *bset, uint32_t i)
{
  ساختار uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  اگر(!chunk)
    بازگشت FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  بازگشت TRUE;
}

تهی Curl_uint32_spbset_remove(ساختار uint32_spbset *bset, uint32_t i)
{
  ساختار uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  اگر(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(ساختار uint32_spbset *bset, uint32_t i)
{
  ساختار uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  اگر(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    بازگشت (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  بازگشت FALSE;
}

bool Curl_uint32_spbset_first(ساختار uint32_spbset *bset, uint32_t *pfirst)
{
  ساختار uint32_spbset_chunk *chunk;
  uint32_t i;

  برای(chunk = &bset->head; chunk; chunk = chunk->next) {
    برای(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      اگر(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        بازگشت TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  بازگشت FALSE;
}

ایستا bool uint32_spbset_chunk_first(ساختار uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  برای(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    اگر(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      بازگشت TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  بازگشت FALSE;
}

ایستا bool uint32_spbset_chunk_next(ساختار uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  اگر(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    اگر(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      اگر(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        بازگشت TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      برای(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        اگر(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          بازگشت TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  بازگشت FALSE;
}

bool Curl_uint32_spbset_next(ساختار uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  ساختار uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  برای(chunk = &bset->head; chunk; chunk = chunk->next) {
    اگر(chunk->offset >= last_offset) {
      بشکن;
    }
  }

  اگر(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    اگر(uint32_spbset_chunk_next(chunk, last, pnext))
      بازگشت TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  وقتی(chunk) {
    اگر(uint32_spbset_chunk_first(chunk, pnext))
      بازگشت TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  بازگشت FALSE;
}
