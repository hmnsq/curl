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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "uint-bset.h"
#ಅನ್ತರ್ಭಾವ "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

ಶೂನ್ಯ Curl_uint32_spbset_init(ಸಂರಚನಾ uint32_spbset *bset)
{
  memset(bset, 0, ಪರಿಮಾಣ(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST ಶೂನ್ಯ uint32_spbset_clear(ಸಂರಚನಾ uint32_spbset *bset);
UNITTEST ಶೂನ್ಯ uint32_spbset_clear(ಸಂರಚನಾ uint32_spbset *bset)
{
  ಸಂರಚನಾ uint32_spbset_chunk *next, *chunk;

  ಪ್ರತಿ(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, ಪರಿಮಾಣ(bset->head));
}

ಶೂನ್ಯ Curl_uint32_spbset_destroy(ಸಂರಚನಾ uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_ಎಣಿಕೆ(ಸಂರಚನಾ uint32_spbset *bset)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  ಪ್ರತಿ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ಪ್ರತಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ಯದಿ(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ bool uint32_spbset_empty_chunk(ಸಂರಚನಾ uint32_spbset_chunk *chunk)
{
  uint32_t i;
  ಪ್ರತಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ಯದಿ(chunk->slots[i])
      ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಸಂರಚನಾ uint32_spbset_chunk *uint32_spbset_unlink_empty(
  ಸಂರಚನಾ uint32_spbset *bset, uint32_t for_offset)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk, **panchor = ರಿಕ್ತ;
  ಪ್ರತಿ(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    ಯದಿ(uint32_spbset_empty_chunk(chunk))
      ಭಙ್ಗ;
  }
  ಯದಿ(chunk) {
    ಯದಿ(chunk == &bset->head) { /* head chunk is empty */
      ಯದಿ(!bset->head.next || (for_offset < bset->head.next->offset)) {
        ಪ್ರತ್ಯಾಗಮ &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, ಪರಿಮಾಣ(bset->head));
      memset(chunk, 0, ಪರಿಮಾಣ(*chunk));
    }
    ಅನ್ಯಥಾ {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, ಪರಿಮಾಣ(*chunk));
    }
  }
  ಪ್ರತ್ಯಾಗಮ chunk;
}

ಸ್ಥಿರ ಸಂರಚನಾ uint32_spbset_chunk *uint32_spbset_ಸೇರಿಸು_chunk(
  ಸಂರಚನಾ uint32_spbset *bset, ಸಂರಚನಾ uint32_spbset_chunk *nchunk)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  ಯದಿ(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, ಪರಿಮಾಣ(*nchunk));
    memset(&bset->head, 0, ಪರಿಮಾಣ(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    ಪ್ರತ್ಯಾಗಮ &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  ಪ್ರತಿ(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    ಯದಿ(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      ಪ್ರತ್ಯಾಗಮ nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  ಪ್ರತ್ಯಾಗಮ nchunk;
}

ಸ್ಥಿರ ಸಂರಚನಾ uint32_spbset_chunk *uint32_spbset_get_chunk(
  ಸಂರಚನಾ uint32_spbset *bset, uint32_t i, bool grow)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  ಯದಿ(!bset)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಪ್ರತಿ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ಯದಿ(chunk->offset == i_offset)
      ಪ್ರತ್ಯಾಗಮ chunk;
    ಅನ್ಯಥಾ ಯದಿ(chunk->offset > i_offset)
      ಭಙ್ಗ; /* need new chunk here */
  }
  ಯದಿ(!grow) /* just a check if the chunk exists */
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  ಯದಿ(chunk) {
    chunk->offset = i_offset;
    ಯದಿ(chunk == &bset->head) /* head chunk is empty, stayed linked */
      ಪ್ರತ್ಯಾಗಮ &bset->head;
    /* was really unlinked, need to insert below */
  }
  ಅನ್ಯಥಾ {
    /* need a new one */
    chunk = curlx_calloc(1, ಪರಿಮಾಣ(*chunk));
    ಯದಿ(!chunk)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    chunk->offset = i_offset;
  }

  ಪ್ರತ್ಯಾಗಮ uint32_spbset_ಸೇರಿಸು_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(ಸಂರಚನಾ uint32_spbset *bset, uint32_t i)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  ಯದಿ(!chunk)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಶೂನ್ಯ Curl_uint32_spbset_remove(ಸಂರಚನಾ uint32_spbset *bset, uint32_t i)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ಯದಿ(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(ಸಂರಚನಾ uint32_spbset *bset, uint32_t i)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ಯದಿ(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    ಪ್ರತ್ಯಾಗಮ (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_uint32_spbset_first(ಸಂರಚನಾ uint32_spbset *bset, uint32_t *pfirst)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk;
  uint32_t i;

  ಪ್ರತಿ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ಪ್ರತಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ಯದಿ(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        ಪ್ರತ್ಯಾಗಮ TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ bool uint32_spbset_chunk_first(ಸಂರಚನಾ uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  ಪ್ರತಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ಯದಿ(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ bool uint32_spbset_chunk_next(ಸಂರಚನಾ uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  ಯದಿ(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    ಯದಿ(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      ಯದಿ(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        ಪ್ರತ್ಯಾಗಮ TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      ಪ್ರತಿ(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        ಯದಿ(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          ಪ್ರತ್ಯಾಗಮ TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_uint32_spbset_next(ಸಂರಚನಾ uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  ಸಂರಚನಾ uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  ಪ್ರತಿ(chunk = &bset->head; chunk; chunk = chunk->next) {
    ಯದಿ(chunk->offset >= last_offset) {
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    ಯದಿ(uint32_spbset_chunk_next(chunk, last, pnext))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  ಯಾವತ್(chunk) {
    ಯದಿ(uint32_spbset_chunk_first(chunk, pnext))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}
