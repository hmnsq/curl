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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "uint-bset.h"
#அந்தர்பாவ "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

வெறுமை Curl_uint32_spbset_init(கட்டமைப்பு uint32_spbset *bset)
{
  memset(bset, 0, அளவு(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST வெறுமை uint32_spbset_clear(கட்டமைப்பு uint32_spbset *bset);
UNITTEST வெறுமை uint32_spbset_clear(கட்டமைப்பு uint32_spbset *bset)
{
  கட்டமைப்பு uint32_spbset_chunk *next, *chunk;

  க்கு(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  memset(&bset->head, 0, அளவு(bset->head));
}

வெறுமை Curl_uint32_spbset_destroy(கட்டமைப்பு uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_மொத்தம்(கட்டமைப்பு uint32_spbset *bset)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  க்கு(chunk = &bset->head; chunk; chunk = chunk->next) {
    க்கு(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      எனில்(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  திரும்பு n;
}

நிலையான bool uint32_spbset_empty_chunk(கட்டமைப்பு uint32_spbset_chunk *chunk)
{
  uint32_t i;
  க்கு(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    எனில்(chunk->slots[i])
      திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

நிலையான கட்டமைப்பு uint32_spbset_chunk *uint32_spbset_unlink_empty(
  கட்டமைப்பு uint32_spbset *bset, uint32_t for_offset)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk, **panchor = வெற்று;
  க்கு(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    எனில்(uint32_spbset_empty_chunk(chunk))
      நிறுத்து;
  }
  எனில்(chunk) {
    எனில்(chunk == &bset->head) { /* head chunk is empty */
      எனில்(!bset->head.next || (for_offset < bset->head.next->offset)) {
        திரும்பு &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      memcpy(&bset->head, chunk, அளவு(bset->head));
      memset(chunk, 0, அளவு(*chunk));
    }
    இல்லையெனில் {
      *panchor = chunk->next; /* unlink */
      memset(chunk, 0, அளவு(*chunk));
    }
  }
  திரும்பு chunk;
}

நிலையான கட்டமைப்பு uint32_spbset_chunk *uint32_spbset_செருகு_chunk(
  கட்டமைப்பு uint32_spbset *bset, கட்டமைப்பு uint32_spbset_chunk *nchunk)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  எனில்(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->head, அளவு(*nchunk));
    memset(&bset->head, 0, அளவு(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    திரும்பு &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  க்கு(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    எனில்(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      திரும்பு nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  திரும்பு nchunk;
}

நிலையான கட்டமைப்பு uint32_spbset_chunk *uint32_spbset_get_chunk(
  கட்டமைப்பு uint32_spbset *bset, uint32_t i, bool grow)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  எனில்(!bset)
    திரும்பு வெற்று;

  க்கு(chunk = &bset->head; chunk; chunk = chunk->next) {
    எனில்(chunk->offset == i_offset)
      திரும்பு chunk;
    இல்லையெனில் எனில்(chunk->offset > i_offset)
      நிறுத்து; /* need new chunk here */
  }
  எனில்(!grow) /* just a check if the chunk exists */
    திரும்பு வெற்று;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  எனில்(chunk) {
    chunk->offset = i_offset;
    எனில்(chunk == &bset->head) /* head chunk is empty, stayed linked */
      திரும்பு &bset->head;
    /* was really unlinked, need to insert below */
  }
  இல்லையெனில் {
    /* need a new one */
    chunk = curlx_calloc(1, அளவு(*chunk));
    எனில்(!chunk)
      திரும்பு வெற்று;
    chunk->offset = i_offset;
  }

  திரும்பு uint32_spbset_செருகு_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(கட்டமைப்பு uint32_spbset *bset, uint32_t i)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  எனில்(!chunk)
    திரும்பு FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  திரும்பு TRUE;
}

வெறுமை Curl_uint32_spbset_remove(கட்டமைப்பு uint32_spbset *bset, uint32_t i)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  எனில்(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(கட்டமைப்பு uint32_spbset *bset, uint32_t i)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  எனில்(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    திரும்பு (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  திரும்பு FALSE;
}

bool Curl_uint32_spbset_first(கட்டமைப்பு uint32_spbset *bset, uint32_t *pfirst)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk;
  uint32_t i;

  க்கு(chunk = &bset->head; chunk; chunk = chunk->next) {
    க்கு(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      எனில்(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        திரும்பு TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  திரும்பு FALSE;
}

நிலையான bool uint32_spbset_chunk_first(கட்டமைப்பு uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  க்கு(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    எனில்(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      திரும்பு TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  திரும்பு FALSE;
}

நிலையான bool uint32_spbset_chunk_next(கட்டமைப்பு uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  எனில்(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    எனில்(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      எனில்(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        திரும்பு TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      க்கு(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        எனில்(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          திரும்பு TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  திரும்பு FALSE;
}

bool Curl_uint32_spbset_next(கட்டமைப்பு uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  கட்டமைப்பு uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  க்கு(chunk = &bset->head; chunk; chunk = chunk->next) {
    எனில்(chunk->offset >= last_offset) {
      நிறுத்து;
    }
  }

  எனில்(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    எனில்(uint32_spbset_chunk_next(chunk, last, pnext))
      திரும்பு TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  போது(chunk) {
    எனில்(uint32_spbset_chunk_first(chunk, pnext))
      திரும்பு TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  திரும்பு FALSE;
}
