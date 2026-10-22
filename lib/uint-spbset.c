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

শূন্য Curl_uint32_spbset_init(কাঠামো uint32_spbset *bset)
{
  memset(bset, 0, আকার(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST শূন্য uint32_spbset_clear(কাঠামো uint32_spbset *bset);
UNITTEST শূন্য uint32_spbset_clear(কাঠামো uint32_spbset *bset)
{
  কাঠামো uint32_spbset_chunk *পরবর্তী, *chunk;

  জন্য(chunk = bset->শীর্ষ.পরবর্তী; chunk; chunk = পরবর্তী) {
    পরবর্তী = chunk->পরবর্তী;
    curlx_free(chunk);
  }
  memset(&bset->শীর্ষ, 0, আকার(bset->শীর্ষ));
}

শূন্য Curl_uint32_spbset_destroy(কাঠামো uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_গুনতি(কাঠামো uint32_spbset *bset)
{
  কাঠামো uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  জন্য(chunk = &bset->শীর্ষ; chunk; chunk = chunk->পরবর্তী) {
    জন্য(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      যদি(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  ফেরত n;
}

স্থির bool uint32_spbset_empty_chunk(কাঠামো uint32_spbset_chunk *chunk)
{
  uint32_t i;
  জন্য(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    যদি(chunk->slots[i])
      ফেরত FALSE;
  }
  ফেরত TRUE;
}

স্থির কাঠামো uint32_spbset_chunk *uint32_spbset_unlink_empty(
  কাঠামো uint32_spbset *bset, uint32_t for_offset)
{
  কাঠামো uint32_spbset_chunk *chunk, **panchor = ফাঁকা;
  জন্য(chunk = &bset->শীর্ষ; chunk;
      panchor = &chunk->পরবর্তী, chunk = chunk->পরবর্তী) {
    যদি(uint32_spbset_empty_chunk(chunk))
      বিরতি;
  }
  যদি(chunk) {
    যদি(chunk == &bset->শীর্ষ) { /* head chunk is empty */
      যদি(!bset->শীর্ষ.পরবর্তী || (for_offset < bset->শীর্ষ.পরবর্তী->offset)) {
        ফেরত &bset->শীর্ষ;
      }
      /* swap head and next, unlink */
      chunk = bset->শীর্ষ.পরবর্তী;
      memcpy(&bset->শীর্ষ, chunk, আকার(bset->শীর্ষ));
      memset(chunk, 0, আকার(*chunk));
    }
    অন্যথা {
      *panchor = chunk->পরবর্তী; /* unlink */
      memset(chunk, 0, আকার(*chunk));
    }
  }
  ফেরত chunk;
}

স্থির কাঠামো uint32_spbset_chunk *uint32_spbset_ঢোকাও_chunk(
  কাঠামো uint32_spbset *bset, কাঠামো uint32_spbset_chunk *nchunk)
{
  কাঠামো uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  যদি(nchunk->offset < bset->শীর্ষ.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    memcpy(nchunk, &bset->শীর্ষ, আকার(*nchunk));
    memset(&bset->শীর্ষ, 0, আকার(bset->শীর্ষ));
    bset->শীর্ষ.পরবর্তী = nchunk;
    bset->শীর্ষ.offset = offset;
    ফেরত &bset->শীর্ষ;
  }
  DEBUGASSERT(nchunk->offset > bset->শীর্ষ.offset);
  panchor = &bset->শীর্ষ.পরবর্তী;
  জন্য(chunk = *panchor; chunk;
      panchor = &chunk->পরবর্তী, chunk = chunk->পরবর্তী) {
    যদি(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->পরবর্তী = chunk;
      *panchor = nchunk;
      ফেরত nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  ফেরত nchunk;
}

স্থির কাঠামো uint32_spbset_chunk *uint32_spbset_get_chunk(
  কাঠামো uint32_spbset *bset, uint32_t i, bool grow)
{
  কাঠামো uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  যদি(!bset)
    ফেরত ফাঁকা;

  জন্য(chunk = &bset->শীর্ষ; chunk; chunk = chunk->পরবর্তী) {
    যদি(chunk->offset == i_offset)
      ফেরত chunk;
    অন্যথা যদি(chunk->offset > i_offset)
      বিরতি; /* need new chunk here */
  }
  যদি(!grow) /* just a check if the chunk exists */
    ফেরত ফাঁকা;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  যদি(chunk) {
    chunk->offset = i_offset;
    যদি(chunk == &bset->শীর্ষ) /* head chunk is empty, stayed linked */
      ফেরত &bset->শীর্ষ;
    /* was really unlinked, need to insert below */
  }
  অন্যথা {
    /* need a new one */
    chunk = curlx_calloc(1, আকার(*chunk));
    যদি(!chunk)
      ফেরত ফাঁকা;
    chunk->offset = i_offset;
  }

  ফেরত uint32_spbset_ঢোকাও_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(কাঠামো uint32_spbset *bset, uint32_t i)
{
  কাঠামো uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  যদি(!chunk)
    ফেরত FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  ফেরত TRUE;
}

শূন্য Curl_uint32_spbset_remove(কাঠামো uint32_spbset *bset, uint32_t i)
{
  কাঠামো uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  যদি(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(কাঠামো uint32_spbset *bset, uint32_t i)
{
  কাঠামো uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  যদি(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    ফেরত (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  ফেরত FALSE;
}

bool Curl_uint32_spbset_first(কাঠামো uint32_spbset *bset, uint32_t *pfirst)
{
  কাঠামো uint32_spbset_chunk *chunk;
  uint32_t i;

  জন্য(chunk = &bset->শীর্ষ; chunk; chunk = chunk->পরবর্তী) {
    জন্য(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      যদি(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        ফেরত TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  ফেরত FALSE;
}

স্থির bool uint32_spbset_chunk_first(কাঠামো uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  জন্য(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    যদি(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      ফেরত TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  ফেরত FALSE;
}

স্থির bool uint32_spbset_chunk_পরবর্তী(কাঠামো uint32_spbset_chunk *chunk,
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
        ফেরত TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      জন্য(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        যদি(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          ফেরত TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  ফেরত FALSE;
}

bool Curl_uint32_spbset_পরবর্তী(কাঠামো uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  কাঠামো uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  জন্য(chunk = &bset->শীর্ষ; chunk; chunk = chunk->পরবর্তী) {
    যদি(chunk->offset >= last_offset) {
      বিরতি;
    }
  }

  যদি(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    যদি(uint32_spbset_chunk_পরবর্তী(chunk, last, pnext))
      ফেরত TRUE;
    /* not in this chunk */
    chunk = chunk->পরবর্তী;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  যখন(chunk) {
    যদি(uint32_spbset_chunk_first(chunk, pnext))
      ফেরত TRUE;
    chunk = chunk->পরবর্তী;
  }
  *pnext = UINT32_MAX;
  ফেরত FALSE;
}
