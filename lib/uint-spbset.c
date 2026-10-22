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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "uint-bset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-spbset.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_UINT32_SPBSET_MAGIC  0x70737362
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_uint32_spbset_init(ਬਣਤਰ uint32_spbset *bset)
{
  memset(bset, 0, ਆਕਾਰ(*bset));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#ਜੇ_ਅੰਤ
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST ਖਾਲੀ uint32_spbset_clear(ਬਣਤਰ uint32_spbset *bset);
UNITTEST ਖਾਲੀ uint32_spbset_clear(ਬਣਤਰ uint32_spbset *bset)
{
  ਬਣਤਰ uint32_spbset_chunk *ਅਗਲਾ, *chunk;

  ਲਈ(chunk = bset->ਸਿਰ.ਅਗਲਾ; chunk; chunk = ਅਗਲਾ) {
    ਅਗਲਾ = chunk->ਅਗਲਾ;
    curlx_free(chunk);
  }
  memset(&bset->ਸਿਰ, 0, ਆਕਾਰ(bset->ਸਿਰ));
}

ਖਾਲੀ Curl_uint32_spbset_destroy(ਬਣਤਰ uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_ਗਿਣਤੀ(ਬਣਤਰ uint32_spbset *bset)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  ਲਈ(chunk = &bset->ਸਿਰ; chunk; chunk = chunk->ਅਗਲਾ) {
    ਲਈ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ਜੇ(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  ਵਾਪਸ n;
}

ਸਥਿਰ bool uint32_spbset_ਖਾਲੀ_ਥਾਵਾਂ_chunk(ਬਣਤਰ uint32_spbset_chunk *chunk)
{
  uint32_t i;
  ਲਈ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ਜੇ(chunk->slots[i])
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਬਣਤਰ uint32_spbset_chunk *uint32_spbset_unlink_ਖਾਲੀ_ਥਾਵਾਂ(
  ਬਣਤਰ uint32_spbset *bset, uint32_t for_ਵਿੱਥ)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk, **panchor = ਨਿਰਾ;
  ਲਈ(chunk = &bset->ਸਿਰ; chunk;
      panchor = &chunk->ਅਗਲਾ, chunk = chunk->ਅਗਲਾ) {
    ਜੇ(uint32_spbset_ਖਾਲੀ_ਥਾਵਾਂ_chunk(chunk))
      ਰੋਕੋ;
  }
  ਜੇ(chunk) {
    ਜੇ(chunk == &bset->ਸਿਰ) { /* head chunk is empty */
      ਜੇ(!bset->ਸਿਰ.ਅਗਲਾ || (for_ਵਿੱਥ < bset->ਸਿਰ.ਅਗਲਾ->ਵਿੱਥ)) {
        ਵਾਪਸ &bset->ਸਿਰ;
      }
      /* swap head and next, unlink */
      chunk = bset->ਸਿਰ.ਅਗਲਾ;
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&bset->ਸਿਰ, chunk, ਆਕਾਰ(bset->ਸਿਰ));
      memset(chunk, 0, ਆਕਾਰ(*chunk));
    }
    ਨਹੀਂਤਾਂ {
      *panchor = chunk->ਅਗਲਾ; /* unlink */
      memset(chunk, 0, ਆਕਾਰ(*chunk));
    }
  }
  ਵਾਪਸ chunk;
}

ਸਥਿਰ ਬਣਤਰ uint32_spbset_chunk *uint32_spbset_ਪਾਓ_chunk(
  ਬਣਤਰ uint32_spbset *bset, ਬਣਤਰ uint32_spbset_chunk *nchunk)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  ਜੇ(nchunk->ਵਿੱਥ < bset->ਸਿਰ.ਵਿੱਥ) {
    /* swap chunk and head */
    uint32_t ਵਿੱਥ = nchunk->ਵਿੱਥ;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(nchunk, &bset->ਸਿਰ, ਆਕਾਰ(*nchunk));
    memset(&bset->ਸਿਰ, 0, ਆਕਾਰ(bset->ਸਿਰ));
    bset->ਸਿਰ.ਅਗਲਾ = nchunk;
    bset->ਸਿਰ.ਵਿੱਥ = ਵਿੱਥ;
    ਵਾਪਸ &bset->ਸਿਰ;
  }
  DEBUGASSERT(nchunk->ਵਿੱਥ > bset->ਸਿਰ.ਵਿੱਥ);
  panchor = &bset->ਸਿਰ.ਅਗਲਾ;
  ਲਈ(chunk = *panchor; chunk;
      panchor = &chunk->ਅਗਲਾ, chunk = chunk->ਅਗਲਾ) {
    ਜੇ(chunk->ਵਿੱਥ > nchunk->ਵਿੱਥ) { /* insert before this chunk */
      nchunk->ਅਗਲਾ = chunk;
      *panchor = nchunk;
      ਵਾਪਸ nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  ਵਾਪਸ nchunk;
}

ਸਥਿਰ ਬਣਤਰ uint32_spbset_chunk *uint32_spbset_get_chunk(
  ਬਣਤਰ uint32_spbset *bset, uint32_t i, bool grow)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk;
  uint32_t i_ਵਿੱਥ = (i & ~CURL_UINT32_SPBSET_CH_ਨਕਾਬ);

  ਜੇ(!bset)
    ਵਾਪਸ ਨਿਰਾ;

  ਲਈ(chunk = &bset->ਸਿਰ; chunk; chunk = chunk->ਅਗਲਾ) {
    ਜੇ(chunk->ਵਿੱਥ == i_ਵਿੱਥ)
      ਵਾਪਸ chunk;
    ਨਹੀਂਤਾਂ ਜੇ(chunk->ਵਿੱਥ > i_ਵਿੱਥ)
      ਰੋਕੋ; /* need new chunk here */
  }
  ਜੇ(!grow) /* just a check if the chunk exists */
    ਵਾਪਸ ਨਿਰਾ;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_ਖਾਲੀ_ਥਾਵਾਂ(bset, i_ਵਿੱਥ);
  ਜੇ(chunk) {
    chunk->ਵਿੱਥ = i_ਵਿੱਥ;
    ਜੇ(chunk == &bset->ਸਿਰ) /* head chunk is empty, stayed linked */
      ਵਾਪਸ &bset->ਸਿਰ;
    /* was really unlinked, need to insert below */
  }
  ਨਹੀਂਤਾਂ {
    /* need a new one */
    chunk = curlx_calloc(1, ਆਕਾਰ(*chunk));
    ਜੇ(!chunk)
      ਵਾਪਸ ਨਿਰਾ;
    chunk->ਵਿੱਥ = i_ਵਿੱਥ;
  }

  ਵਾਪਸ uint32_spbset_ਪਾਓ_chunk(bset, chunk);
}

bool Curl_uint32_spbset_ਜੋੜੋ(ਬਣਤਰ uint32_spbset *bset, uint32_t i)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  ਜੇ(!chunk)
    ਵਾਪਸ FALSE;

  DEBUGASSERT(i >= chunk->ਵਿੱਥ);
  i -= chunk->ਵਿੱਥ;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  ਵਾਪਸ TRUE;
}

ਖਾਲੀ Curl_uint32_spbset_remove(ਬਣਤਰ uint32_spbset *bset, uint32_t i)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ਜੇ(chunk) {
    DEBUGASSERT(i >= chunk->ਵਿੱਥ);
    i -= chunk->ਵਿੱਥ;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_ਸ਼ਾਮਲ_ਹੈ(ਬਣਤਰ uint32_spbset *bset, uint32_t i)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ਜੇ(chunk) {
    DEBUGASSERT(i >= chunk->ਵਿੱਥ);
    i -= chunk->ਵਿੱਥ;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    ਵਾਪਸ (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  ਵਾਪਸ FALSE;
}

bool Curl_uint32_spbset_ਪਹਿਲਾ(ਬਣਤਰ uint32_spbset *bset, uint32_t *pfirst)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk;
  uint32_t i;

  ਲਈ(chunk = &bset->ਸਿਰ; chunk; chunk = chunk->ਅਗਲਾ) {
    ਲਈ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ਜੇ(chunk->slots[i]) {
        *pfirst = chunk->ਵਿੱਥ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        ਵਾਪਸ TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool uint32_spbset_chunk_ਪਹਿਲਾ(ਬਣਤਰ uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  ਲਈ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ਜੇ(chunk->slots[i]) {
      *pfirst = chunk->ਵਿੱਥ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      ਵਾਪਸ TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool uint32_spbset_chunk_ਅਗਲਾ(ਬਣਤਰ uint32_spbset_chunk *chunk,
                                     uint32_t ਆਖਰੀ,
                                     uint32_t *pnext)
{
  ਜੇ(chunk->ਵਿੱਥ <= ਆਖਰੀ) {
    uint64_t x;
    uint32_t i = ((ਆਖਰੀ - chunk->ਵਿੱਥ) / 64);
    ਜੇ(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (ਆਖਰੀ % 64));
      ਜੇ(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = ਆਖਰੀ + CURL_CTZ64(x);
        ਵਾਪਸ TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      ਲਈ(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        ਜੇ(chunk->slots[i]) {
          *pnext = chunk->ਵਿੱਥ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          ਵਾਪਸ TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  ਵਾਪਸ FALSE;
}

bool Curl_uint32_spbset_ਅਗਲਾ(ਬਣਤਰ uint32_spbset *bset, uint32_t ਆਖਰੀ,
                             uint32_t *pnext)
{
  ਬਣਤਰ uint32_spbset_chunk *chunk;
  uint32_t ਆਖਰੀ_ਵਿੱਥ;

  ++ਆਖਰੀ; /* look for the next higher number */
  ਆਖਰੀ_ਵਿੱਥ = (ਆਖਰੀ & ~CURL_UINT32_SPBSET_CH_ਨਕਾਬ);

  ਲਈ(chunk = &bset->ਸਿਰ; chunk; chunk = chunk->ਅਗਲਾ) {
    ਜੇ(chunk->ਵਿੱਥ >= ਆਖਰੀ_ਵਿੱਥ) {
      ਰੋਕੋ;
    }
  }

  ਜੇ(chunk && (chunk->ਵਿੱਥ == ਆਖਰੀ_ਵਿੱਥ)) {
    /* is there a number higher than last in this chunk? */
    ਜੇ(uint32_spbset_chunk_ਅਗਲਾ(chunk, ਆਖਰੀ, pnext))
      ਵਾਪਸ TRUE;
    /* not in this chunk */
    chunk = chunk->ਅਗਲਾ;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  ਜਦੋਂ(chunk) {
    ਜੇ(uint32_spbset_chunk_ਪਹਿਲਾ(chunk, pnext))
      ਵਾਪਸ TRUE;
    chunk = chunk->ਅਗਲਾ;
  }
  *pnext = UINT32_MAX;
  ਵਾਪਸ FALSE;
}
