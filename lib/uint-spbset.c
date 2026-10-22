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
#समावेश "curl_setup.h"

#समावेश "uint-bset.h"
#समावेश "uint-spbset.h"

#अगर_घोषित DEBUGBUILD
#घोषणा CURL_UINT32_SPBSET_MAGIC  0x70737362
#पूर्ण_अगर

व्योम Curl_uint32_spbset_init(काष्ठा uint32_spbset *bset)
{
  स_रखो(bset, 0, माप(*bset));
#अगर_घोषित DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#पूर्ण_अगर
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST व्योम uint32_spbset_clear(काष्ठा uint32_spbset *bset);
UNITTEST व्योम uint32_spbset_clear(काष्ठा uint32_spbset *bset)
{
  काष्ठा uint32_spbset_chunk *अग्र, *chunk;

  क्रम(chunk = bset->शीर्ष.अग्र; chunk; chunk = अग्र) {
    अग्र = chunk->अग्र;
    curlx_free(chunk);
  }
  स_रखो(&bset->शीर्ष, 0, माप(bset->शीर्ष));
}

व्योम Curl_uint32_spbset_destroy(काष्ठा uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_गिनती(काष्ठा uint32_spbset *bset)
{
  काष्ठा uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  क्रम(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    क्रम(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      अगर(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  वापस n;
}

अटल bool uint32_spbset_empty_chunk(काष्ठा uint32_spbset_chunk *chunk)
{
  uint32_t i;
  क्रम(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    अगर(chunk->slots[i])
      वापस FALSE;
  }
  वापस TRUE;
}

अटल काष्ठा uint32_spbset_chunk *uint32_spbset_unlink_empty(
  काष्ठा uint32_spbset *bset, uint32_t for_offset)
{
  काष्ठा uint32_spbset_chunk *chunk, **panchor = शून्य;
  क्रम(chunk = &bset->शीर्ष; chunk;
      panchor = &chunk->अग्र, chunk = chunk->अग्र) {
    अगर(uint32_spbset_empty_chunk(chunk))
      अवरोध;
  }
  अगर(chunk) {
    अगर(chunk == &bset->शीर्ष) { /* head chunk is empty */
      अगर(!bset->शीर्ष.अग्र || (for_offset < bset->शीर्ष.अग्र->offset)) {
        वापस &bset->शीर्ष;
      }
      /* swap head and next, unlink */
      chunk = bset->शीर्ष.अग्र;
      स_नकल(&bset->शीर्ष, chunk, माप(bset->शीर्ष));
      स_रखो(chunk, 0, माप(*chunk));
    }
    अन्यथा {
      *panchor = chunk->अग्र; /* unlink */
      स_रखो(chunk, 0, माप(*chunk));
    }
  }
  वापस chunk;
}

अटल काष्ठा uint32_spbset_chunk *uint32_spbset_जोड़ो_chunk(
  काष्ठा uint32_spbset *bset, काष्ठा uint32_spbset_chunk *nchunk)
{
  काष्ठा uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  अगर(nchunk->offset < bset->शीर्ष.offset) {
    /* swap chunk and head */
    uint32_t offset = nchunk->offset;
    स_नकल(nchunk, &bset->शीर्ष, माप(*nchunk));
    स_रखो(&bset->शीर्ष, 0, माप(bset->शीर्ष));
    bset->शीर्ष.अग्र = nchunk;
    bset->शीर्ष.offset = offset;
    वापस &bset->शीर्ष;
  }
  DEBUGASSERT(nchunk->offset > bset->शीर्ष.offset);
  panchor = &bset->शीर्ष.अग्र;
  क्रम(chunk = *panchor; chunk;
      panchor = &chunk->अग्र, chunk = chunk->अग्र) {
    अगर(chunk->offset > nchunk->offset) { /* insert before this chunk */
      nchunk->अग्र = chunk;
      *panchor = nchunk;
      वापस nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  वापस nchunk;
}

अटल काष्ठा uint32_spbset_chunk *uint32_spbset_get_chunk(
  काष्ठा uint32_spbset *bset, uint32_t i, bool grow)
{
  काष्ठा uint32_spbset_chunk *chunk;
  uint32_t i_offset = (i & ~CURL_UINT32_SPBSET_CH_MASK);

  अगर(!bset)
    वापस शून्य;

  क्रम(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    अगर(chunk->offset == i_offset)
      वापस chunk;
    अन्यथा अगर(chunk->offset > i_offset)
      अवरोध; /* need new chunk here */
  }
  अगर(!grow) /* just a check if the chunk exists */
    वापस शून्य;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_empty(bset, i_offset);
  अगर(chunk) {
    chunk->offset = i_offset;
    अगर(chunk == &bset->शीर्ष) /* head chunk is empty, stayed linked */
      वापस &bset->शीर्ष;
    /* was really unlinked, need to insert below */
  }
  अन्यथा {
    /* need a new one */
    chunk = curlx_calloc(1, माप(*chunk));
    अगर(!chunk)
      वापस शून्य;
    chunk->offset = i_offset;
  }

  वापस uint32_spbset_जोड़ो_chunk(bset, chunk);
}

bool Curl_uint32_spbset_add(काष्ठा uint32_spbset *bset, uint32_t i)
{
  काष्ठा uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  अगर(!chunk)
    वापस FALSE;

  DEBUGASSERT(i >= chunk->offset);
  i -= chunk->offset;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  वापस TRUE;
}

व्योम Curl_uint32_spbset_remove(काष्ठा uint32_spbset *bset, uint32_t i)
{
  काष्ठा uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  अगर(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_contains(काष्ठा uint32_spbset *bset, uint32_t i)
{
  काष्ठा uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  अगर(chunk) {
    DEBUGASSERT(i >= chunk->offset);
    i -= chunk->offset;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    वापस (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  वापस FALSE;
}

bool Curl_uint32_spbset_first(काष्ठा uint32_spbset *bset, uint32_t *pfirst)
{
  काष्ठा uint32_spbset_chunk *chunk;
  uint32_t i;

  क्रम(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    क्रम(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      अगर(chunk->slots[i]) {
        *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        वापस TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  वापस FALSE;
}

अटल bool uint32_spbset_chunk_first(काष्ठा uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  क्रम(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    अगर(chunk->slots[i]) {
      *pfirst = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      वापस TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  वापस FALSE;
}

अटल bool uint32_spbset_chunk_अग्र(काष्ठा uint32_spbset_chunk *chunk,
                                     uint32_t last,
                                     uint32_t *pnext)
{
  अगर(chunk->offset <= last) {
    uint64_t x;
    uint32_t i = ((last - chunk->offset) / 64);
    अगर(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (last % 64));
      अगर(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = last + CURL_CTZ64(x);
        वापस TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      क्रम(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        अगर(chunk->slots[i]) {
          *pnext = chunk->offset + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          वापस TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  वापस FALSE;
}

bool Curl_uint32_spbset_अग्र(काष्ठा uint32_spbset *bset, uint32_t last,
                             uint32_t *pnext)
{
  काष्ठा uint32_spbset_chunk *chunk;
  uint32_t last_offset;

  ++last; /* look for the next higher number */
  last_offset = (last & ~CURL_UINT32_SPBSET_CH_MASK);

  क्रम(chunk = &bset->शीर्ष; chunk; chunk = chunk->अग्र) {
    अगर(chunk->offset >= last_offset) {
      अवरोध;
    }
  }

  अगर(chunk && (chunk->offset == last_offset)) {
    /* is there a number higher than last in this chunk? */
    अगर(uint32_spbset_chunk_अग्र(chunk, last, pnext))
      वापस TRUE;
    /* not in this chunk */
    chunk = chunk->अग्र;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  जबतक(chunk) {
    अगर(uint32_spbset_chunk_first(chunk, pnext))
      वापस TRUE;
    chunk = chunk->अग्र;
  }
  *pnext = UINT32_MAX;
  वापस FALSE;
}
