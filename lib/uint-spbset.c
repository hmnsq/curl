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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "uint-bset.h"
#અન્તર્ભાવ "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

શૂન્ય Curl_uint32_spbset_init(સંરચના uint32_spbset *bset)
{
  memset(bset, 0, કદ(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST શૂન્ય uint32_spbset_clear(સંરચના uint32_spbset *bset);
UNITTEST શૂન્ય uint32_spbset_clear(સંરચના uint32_spbset *bset)
{
  સંરચના uint32_spbset_chunk *next, *chunk;

  માટે(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, કદ(bset->head));
}

શૂન્ય Curl_uint32_spbset_destroy(સંરચના uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_ગણતરી(સંરચના uint32_spbset *bset)
{
  સંરચના uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  માટે(chunk = &bset->head; chunk; chunk = chunk->next) {
    માટે(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      જો(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  પાછા n;
}

સ્થિર bool uint32_spbset_empty_chunk(સંરચના uint32_spbset_chunk *chunk)
{
  uint32_t i;
  માટે(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    જો(chunk->slots[i])
      પાછા FALSE;
  }
  પાછા TRUE;
}

સ્થિર સંરચના uint32_spbset_chunk *uint32_spbset_unlink_empty(
  સંરચના uint32_spbset *bset, uint32_t for_offset)
{
  સંરચના uint32_spbset_chunk *chunk, **panchor = ખાલી;
  માટે(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    જો(uint32_spbset_empty_chunk(chunk))
      તોડો;
  }
  જો(chunk) {
    જો(chunk == &bset->head) { /* head chunk is empty */
      જો(!bset->head.next || (for_offset < bset->head.next->offset)) {
        પાછા &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, કદ(bset->head));
      memset(chunk, 0, કદ(*chunk));
    }
    નહીંતર {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, કદ(*chunk));
    }
  }
  પાછા chunk;
}

સ્થિર સંરચના uint32_spbset_chunk *uint32_spbset_ઉમેરો_chunk(
  સંરચના uint32_spbset *bset, સંરચના uint32_spbset_chunk *nchunk)
{
  સંરચના uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  જો(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, કદ(*nchunk));
    memset(&bset->head, 0, કદ(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    પાછા &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  માટે(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    જો(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      પાછા nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  પાછા nchunk;
}

સ્થિર સંરચના uint32_spbset_chunk *uint32_spbset_get_chunk(
  સંરચના uint32_spbset *bset, uint32_t i, bool grow)
{
  સંરચના uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  જો(!bset)
    પાછા ખાલી;

  માટે(chunk = &bset->head; chunk; chunk = chunk->next) {
    જો(chunk->offset == i_offset)
      પાછા chunk;
    નહીંતર જો(chunk->offset > i_offset)
      તોડો; /* need new chunk here */
  }
  જો(!grow) /* just a check if the chunk exists */
    પાછા ખાલી;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  જો(chunk) {
    chunk->offset = i_offset;
    જો(chunk == &bset->head) /* head chunk is empty, stayed linked */
      પાછા &bset->head;
    /* was really unlinked, need to insert below */
  }
  નહીંતર {
    /* need a new one */
    chunk = curlx_calloc(1, કદ(*chunk));
    જો(!chunk)
      પાછા ખાલી;
    chunk->offset = i_offset;
  }

  પાછા uint32_spbset_ઉમેરો_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(સંરચના uint32_spbset *bset, uint32_t i)
{
  સંરચના uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  જો(!chunk)
    પાછા FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  પાછા TRUE;
}

શૂન્ય Curl_uint32_spbset_remove(સંરચના uint32_spbset *bset, uint32_t i)
{
  સંરચના uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  જો(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(સંરચના uint32_spbset *bset, uint32_t i)
{
  સંરચના uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  જો(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    પાછા (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  પાછા FALSE;
}

bool Curl_uint32_spbset_first(સંરચના uint32_spbset *bset, uint32_t *pfirst)
{
  સંરચના uint32_spbset_chunk *chunk;
  uint32_t i;

  માટે(chunk = &bset->head; chunk; chunk = chunk->next) {
    માટે(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      જો(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        પાછા TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  પાછા FALSE;
}

સ્થિર bool uint32_spbset_chunk_first(સંરચના uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  માટે(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    જો(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      પાછા TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  પાછા FALSE;
}

સ્થિર bool uint32_spbset_chunk_next(સંરચના uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  જો(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    જો(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      જો(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        પાછા TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      માટે(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        જો(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          પાછા TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  પાછા FALSE;
}

bool Curl_uint32_spbset_next(સંરચના uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  સંરચના uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  માટે(chunk = &bset->head; chunk; chunk = chunk->next) {
    જો(chunk->offset >= last_offset) {
      તોડો;
    }
  }

  જો(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    જો(uint32_spbset_chunk_next(chunk, last, pnext))
      પાછા TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  જ્યારે(chunk) {
    જો(uint32_spbset_chunk_first(chunk, pnext))
      પાછા TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  પાછા FALSE;
}
