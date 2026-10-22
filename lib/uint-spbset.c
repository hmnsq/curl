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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "uint-bset.h"
#അന്തര്ഭാവ "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

ശൂന്യം Curl_uint32_spbset_init(ഘടന uint32_spbset *bset)
{
  memset(bset, 0, വലുപ്പം(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST ശൂന്യം uint32_spbset_clear(ഘടന uint32_spbset *bset);
UNITTEST ശൂന്യം uint32_spbset_clear(ഘടന uint32_spbset *bset)
{
  ഘടന uint32_spbset_chunk *next, *chunk;

  വേണ്ടി(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, വലുപ്പം(bset->head));
}

ശൂന്യം Curl_uint32_spbset_destroy(ഘടന uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_എണ്ണം(ഘടന uint32_spbset *bset)
{
  ഘടന uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  വേണ്ടി(chunk = &bset->head; chunk; chunk = chunk->next) {
    വേണ്ടി(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      എങ്കിൽ(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  തിരികെ n;
}

സ്ഥിതം bool uint32_spbset_empty_chunk(ഘടന uint32_spbset_chunk *chunk)
{
  uint32_t i;
  വേണ്ടി(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    എങ്കിൽ(chunk->slots[i])
      തിരികെ FALSE;
  }
  തിരികെ TRUE;
}

സ്ഥിതം ഘടന uint32_spbset_chunk *uint32_spbset_unlink_empty(
  ഘടന uint32_spbset *bset, uint32_t for_offset)
{
  ഘടന uint32_spbset_chunk *chunk, **panchor = ശൂന്യ;
  വേണ്ടി(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    എങ്കിൽ(uint32_spbset_empty_chunk(chunk))
      നിർത്തുക;
  }
  എങ്കിൽ(chunk) {
    എങ്കിൽ(chunk == &bset->head) { /* head chunk is empty */
      എങ്കിൽ(!bset->head.next || (for_offset < bset->head.next->offset)) {
        തിരികെ &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, വലുപ്പം(bset->head));
      memset(chunk, 0, വലുപ്പം(*chunk));
    }
    അല്ലെങ്കിൽ {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, വലുപ്പം(*chunk));
    }
  }
  തിരികെ chunk;
}

സ്ഥിതം ഘടന uint32_spbset_chunk *uint32_spbset_തിരുകുക_chunk(
  ഘടന uint32_spbset *bset, ഘടന uint32_spbset_chunk *nchunk)
{
  ഘടന uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  എങ്കിൽ(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, വലുപ്പം(*nchunk));
    memset(&bset->head, 0, വലുപ്പം(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    തിരികെ &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  വേണ്ടി(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    എങ്കിൽ(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      തിരികെ nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  തിരികെ nchunk;
}

സ്ഥിതം ഘടന uint32_spbset_chunk *uint32_spbset_get_chunk(
  ഘടന uint32_spbset *bset, uint32_t i, bool grow)
{
  ഘടന uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  എങ്കിൽ(!bset)
    തിരികെ ശൂന്യ;

  വേണ്ടി(chunk = &bset->head; chunk; chunk = chunk->next) {
    എങ്കിൽ(chunk->offset == i_offset)
      തിരികെ chunk;
    അല്ലെങ്കിൽ എങ്കിൽ(chunk->offset > i_offset)
      നിർത്തുക; /* need new chunk here */
  }
  എങ്കിൽ(!grow) /* just a check if the chunk exists */
    തിരികെ ശൂന്യ;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  എങ്കിൽ(chunk) {
    chunk->offset = i_offset;
    എങ്കിൽ(chunk == &bset->head) /* head chunk is empty, stayed linked */
      തിരികെ &bset->head;
    /* was really unlinked, need to insert below */
  }
  അല്ലെങ്കിൽ {
    /* need a new one */
    chunk = curlx_calloc(1, വലുപ്പം(*chunk));
    എങ്കിൽ(!chunk)
      തിരികെ ശൂന്യ;
    chunk->offset = i_offset;
  }

  തിരികെ uint32_spbset_തിരുകുക_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(ഘടന uint32_spbset *bset, uint32_t i)
{
  ഘടന uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  എങ്കിൽ(!chunk)
    തിരികെ FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  തിരികെ TRUE;
}

ശൂന്യം Curl_uint32_spbset_remove(ഘടന uint32_spbset *bset, uint32_t i)
{
  ഘടന uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  എങ്കിൽ(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(ഘടന uint32_spbset *bset, uint32_t i)
{
  ഘടന uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  എങ്കിൽ(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    തിരികെ (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  തിരികെ FALSE;
}

bool Curl_uint32_spbset_first(ഘടന uint32_spbset *bset, uint32_t *pfirst)
{
  ഘടന uint32_spbset_chunk *chunk;
  uint32_t i;

  വേണ്ടി(chunk = &bset->head; chunk; chunk = chunk->next) {
    വേണ്ടി(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      എങ്കിൽ(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        തിരികെ TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  തിരികെ FALSE;
}

സ്ഥിതം bool uint32_spbset_chunk_first(ഘടന uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  വേണ്ടി(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    എങ്കിൽ(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      തിരികെ TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  തിരികെ FALSE;
}

സ്ഥിതം bool uint32_spbset_chunk_next(ഘടന uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  എങ്കിൽ(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    എങ്കിൽ(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      എങ്കിൽ(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        തിരികെ TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      വേണ്ടി(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        എങ്കിൽ(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          തിരികെ TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  തിരികെ FALSE;
}

bool Curl_uint32_spbset_next(ഘടന uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  ഘടന uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  വേണ്ടി(chunk = &bset->head; chunk; chunk = chunk->next) {
    എങ്കിൽ(chunk->offset >= last_offset) {
      നിർത്തുക;
    }
  }

  എങ്കിൽ(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    എങ്കിൽ(uint32_spbset_chunk_next(chunk, last, pnext))
      തിരികെ TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  ആകുമ്പോൾ(chunk) {
    എങ്കിൽ(uint32_spbset_chunk_first(chunk, pnext))
      തിരികെ TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  തിരികെ FALSE;
}
