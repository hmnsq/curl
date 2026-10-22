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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "uint-bset.h"
#অন্তর্ভাব "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

শূন্য Curl_uint32_spbset_init(সংরচনা uint32_spbset *bset)
{
  memset(bset, 0, পরিমাণ(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST শূন্য uint32_spbset_clear(সংরচনা uint32_spbset *bset);
UNITTEST শূন্য uint32_spbset_clear(সংরচনা uint32_spbset *bset)
{
  সংরচনা uint32_spbset_chunk *next, *chunk;

  প্রতি(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, পরিমাণ(bset->head));
}

শূন্য Curl_uint32_spbset_destroy(সংরচনা uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_গুনতি(সংরচনা uint32_spbset *bset)
{
  সংরচনা uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  প্রতি(chunk = &bset->head; chunk; chunk = chunk->next) {
    প্রতি(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      যদি(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  প্রত্যাগম n;
}

স্থির bool uint32_spbset_empty_chunk(সংরচনা uint32_spbset_chunk *chunk)
{
  uint32_t i;
  প্রতি(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    যদি(chunk->slots[i])
      প্রত্যাগম FALSE;
  }
  প্রত্যাগম TRUE;
}

স্থির সংরচনা uint32_spbset_chunk *uint32_spbset_unlink_empty(
  সংরচনা uint32_spbset *bset, uint32_t for_offset)
{
  সংরচনা uint32_spbset_chunk *chunk, **panchor = রিক্ত;
  প্রতি(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    যদি(uint32_spbset_empty_chunk(chunk))
      ভঙ্গ;
  }
  যদি(chunk) {
    যদি(chunk == &bset->head) { /* head chunk is empty */
      যদি(!bset->head.next || (for_offset < bset->head.next->offset)) {
        প্রত্যাগম &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, পরিমাণ(bset->head));
      memset(chunk, 0, পরিমাণ(*chunk));
    }
    অন্যথা {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, পরিমাণ(*chunk));
    }
  }
  প্রত্যাগম chunk;
}

স্থির সংরচনা uint32_spbset_chunk *uint32_spbset_ঢোকাও_chunk(
  সংরচনা uint32_spbset *bset, সংরচনা uint32_spbset_chunk *nchunk)
{
  সংরচনা uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  যদি(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, পরিমাণ(*nchunk));
    memset(&bset->head, 0, পরিমাণ(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    প্রত্যাগম &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  প্রতি(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    যদি(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      প্রত্যাগম nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  প্রত্যাগম nchunk;
}

স্থির সংরচনা uint32_spbset_chunk *uint32_spbset_get_chunk(
  সংরচনা uint32_spbset *bset, uint32_t i, bool grow)
{
  সংরচনা uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  যদি(!bset)
    প্রত্যাগম রিক্ত;

  প্রতি(chunk = &bset->head; chunk; chunk = chunk->next) {
    যদি(chunk->offset == i_offset)
      প্রত্যাগম chunk;
    অন্যথা যদি(chunk->offset > i_offset)
      ভঙ্গ; /* need new chunk here */
  }
  যদি(!grow) /* just a check if the chunk exists */
    প্রত্যাগম রিক্ত;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  যদি(chunk) {
    chunk->offset = i_offset;
    যদি(chunk == &bset->head) /* head chunk is empty, stayed linked */
      প্রত্যাগম &bset->head;
    /* was really unlinked, need to insert below */
  }
  অন্যথা {
    /* need a new one */
    chunk = curlx_calloc(1, পরিমাণ(*chunk));
    যদি(!chunk)
      প্রত্যাগম রিক্ত;
    chunk->offset = i_offset;
  }

  প্রত্যাগম uint32_spbset_ঢোকাও_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(সংরচনা uint32_spbset *bset, uint32_t i)
{
  সংরচনা uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  যদি(!chunk)
    প্রত্যাগম FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  প্রত্যাগম TRUE;
}

শূন্য Curl_uint32_spbset_remove(সংরচনা uint32_spbset *bset, uint32_t i)
{
  সংরচনা uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  যদি(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(সংরচনা uint32_spbset *bset, uint32_t i)
{
  সংরচনা uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  যদি(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    প্রত্যাগম (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  প্রত্যাগম FALSE;
}

bool Curl_uint32_spbset_first(সংরচনা uint32_spbset *bset, uint32_t *pfirst)
{
  সংরচনা uint32_spbset_chunk *chunk;
  uint32_t i;

  প্রতি(chunk = &bset->head; chunk; chunk = chunk->next) {
    প্রতি(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      যদি(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        প্রত্যাগম TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  প্রত্যাগম FALSE;
}

স্থির bool uint32_spbset_chunk_first(সংরচনা uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  প্রতি(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    যদি(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      প্রত্যাগম TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  প্রত্যাগম FALSE;
}

স্থির bool uint32_spbset_chunk_next(সংরচনা uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  যদি(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    যদি(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      যদি(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        প্রত্যাগম TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      প্রতি(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        যদি(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          প্রত্যাগম TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  প্রত্যাগম FALSE;
}

bool Curl_uint32_spbset_next(সংরচনা uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  সংরচনা uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  প্রতি(chunk = &bset->head; chunk; chunk = chunk->next) {
    যদি(chunk->offset >= last_offset) {
      ভঙ্গ;
    }
  }

  যদি(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    যদি(uint32_spbset_chunk_next(chunk, last, pnext))
      প্রত্যাগম TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  যাবত্(chunk) {
    যদি(uint32_spbset_chunk_first(chunk, pnext))
      প্রত্যাগম TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  প্রত্যাগম FALSE;
}
