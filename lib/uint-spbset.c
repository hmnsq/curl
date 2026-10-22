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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "uint-bset.h"
#अन्तर्भाव "uint-spbset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_SPBSET_MAGIC  0x70737362
#endif

शून्य Curl_uint32_spbset_init(संरचना uint32_spbset *bset)
{
  स_रखो(bset, 0, आकार(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#endif
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST शून्य uint32_spbset_clear(संरचना uint32_spbset *bset);
UNITTEST शून्य uint32_spbset_clear(संरचना uint32_spbset *bset)
{
  संरचना uint32_spbset_chunk *next, *chunk;

  साठी(chunk = bset->head.next; chunk; chunk = next) {
    next = chunk->next;
    curlx_free(chunk);
  }
  स_रखो(&bset->head, 0, आकार(bset->head));
}

शून्य Curl_uint32_spbset_destroy(संरचना uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_मोजणी(संरचना uint32_spbset *bset)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  साठी(chunk = &bset->head; chunk; chunk = chunk->next) {
    साठी(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      जर(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  परत n;
}

स्थिर bool uint32_spbset_empty_chunk(संरचना uint32_spbset_chunk *chunk)
{
  uint32_t i;
  साठी(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    जर(chunk->slots[i])
      परत FALSE;
  }
  परत TRUE;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_unlink_empty(
  संरचना uint32_spbset *bset, uint32_t for_offset)
{
  संरचना uint32_spbset_chunk *chunk, **panchor = रिक्त;
  साठी(chunk = &bset->head; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    जर(uint32_spbset_empty_chunk(chunk))
      थांबा;
  }
  जर(chunk) {
    जर(chunk == &bset->head) { /* head chunk is empty */
      जर(!bset->head.next || (for_offset < bset->head.next->offset)) {
        परत &bset->head;
      }
      /* swap head and next, unlink */
      chunk = bset->head.next;
      स_नकल(&bset->head, chunk, आकार(bset->head));
      स_रखो(chunk, 0, आकार(*chunk));
    }
    अन्यथा {
      *panchor = chunk->next; /* unlink */
      स_रखो(chunk, 0, आकार(*chunk));
    }
  }
  परत chunk;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_घाला_chunk(
  संरचना uint32_spbset *bset, संरचना uint32_spbset_chunk *nchunk)
{
  संरचना uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  जर(nchunk->offset < bset->head.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    स_नकल(nchunk, &bset->head, आकार(*nchunk));
    स_रखो(&bset->head, 0, आकार(bset->head));
    bset->head.next = nchunk;
    bset->head.offset = offset;
    परत &bset->head;
  }
  DEBUGASSERT(nchunk->offset > bset->head.offset);
  panchor = &bset->head.next;
  साठी(chunk = *panchor; chunk;
      panchor = &chunk->next, chunk = chunk->next) {
    जर(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->next = chunk;
      *panchor = nchunk;
      परत nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  परत nchunk;
}

स्थिर संरचना uint32_spbset_chunk *uint32_spbset_get_chunk(
  संरचना uint32_spbset *bset, uint32_t i, bool grow)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  जर(!bset)
    परत रिक्त;

  साठी(chunk = &bset->head; chunk; chunk = chunk->next) {
    जर(chunk->offset == i_offset)
      परत chunk;
    अन्यथा जर(chunk->offset > i_offset)
      थांबा; /* need new chunk here */
  }
  जर(!grow) /* just a check if the chunk exists */
    परत रिक्त;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  जर(chunk) {
    chunk->offset = i_offset;
    जर(chunk == &bset->head) /* head chunk is empty, stayed linked */
      परत &bset->head;
    /* was really unlinked, need to insert below */
  }
  अन्यथा {
    /* need a new one */
    chunk = curlx_calloc(1, आकार(*chunk));
    जर(!chunk)
      परत रिक्त;
    chunk->offset = i_offset;
  }

  परत uint32_spbset_घाला_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  जर(!chunk)
    परत FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  परत TRUE;
}

शून्य Curl_uint32_spbset_remove(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  जर(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(संरचना uint32_spbset *bset, uint32_t i)
{
  संरचना uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  जर(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    परत (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  परत FALSE;
}

bool Curl_uint32_spbset_first(संरचना uint32_spbset *bset, uint32_t *pfirst)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t i;

  साठी(chunk = &bset->head; chunk; chunk = chunk->next) {
    साठी(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      जर(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        परत TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  परत FALSE;
}

स्थिर bool uint32_spbset_chunk_first(संरचना uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  साठी(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    जर(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      परत TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  परत FALSE;
}

स्थिर bool uint32_spbset_chunk_next(संरचना uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  जर(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    जर(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      जर(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        परत TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      साठी(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        जर(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          परत TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  परत FALSE;
}

bool Curl_uint32_spbset_next(संरचना uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  संरचना uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  साठी(chunk = &bset->head; chunk; chunk = chunk->next) {
    जर(chunk->offset >= last_offset) {
      थांबा;
    }
  }

  जर(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    जर(uint32_spbset_chunk_next(chunk, last, pnext))
      परत TRUE;
    /* not in this chunk */
    chunk = chunk->next;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  पर्यंत(chunk) {
    जर(uint32_spbset_chunk_first(chunk, pnext))
      परत TRUE;
    chunk = chunk->next;
  }
  *pnext = UINT32_MAX;
  परत FALSE;
}
