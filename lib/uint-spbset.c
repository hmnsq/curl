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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_UINT32_SPBSET_MAGIC  0x70737362
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_uint32_spbset_init(ರಚನೆ uint32_spbset *bset)
{
  memset(bset, 0, ಗಾತ್ರ(*bset));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST ಶೂನ್ಯ uint32_spbset_clear(ರಚನೆ uint32_spbset *bset);
UNITTEST ಶೂನ್ಯ uint32_spbset_clear(ರಚನೆ uint32_spbset *bset)
{
  ರಚನೆ uint32_spbset_chunk *ಮುಂದಿನ, *chunk;

  ಗಾಗಿ(chunk = bset->ತಲೆ.ಮುಂದಿನ; chunk; chunk = ಮುಂದಿನ) {
    ಮುಂದಿನ = chunk->ಮುಂದಿನ;
    curlx_free(chunk);
  }
  memset(&bset->ತಲೆ, 0, ಗಾತ್ರ(bset->ತಲೆ));
}

ಶೂನ್ಯ Curl_uint32_spbset_destroy(ರಚನೆ uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_ಎಣಿಕೆ(ರಚನೆ uint32_spbset *bset)
{
  ರಚನೆ uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  ಗಾಗಿ(chunk = &bset->ತಲೆ; chunk; chunk = chunk->ಮುಂದಿನ) {
    ಗಾಗಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ಆದರೆ(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ bool uint32_spbset_ಖಾಲಿಗಳು_chunk(ರಚನೆ uint32_spbset_chunk *chunk)
{
  uint32_t i;
  ಗಾಗಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ಆದರೆ(chunk->slots[i])
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ರಚನೆ uint32_spbset_chunk *uint32_spbset_unlink_ಖಾಲಿಗಳು(
  ರಚನೆ uint32_spbset *bset, uint32_t for_ಸ್ಥಾನಾಂತರ)
{
  ರಚನೆ uint32_spbset_chunk *chunk, **panchor = ಖಾಲಿ;
  ಗಾಗಿ(chunk = &bset->ತಲೆ; chunk;
      panchor = &chunk->ಮುಂದಿನ, chunk = chunk->ಮುಂದಿನ) {
    ಆದರೆ(uint32_spbset_ಖಾಲಿಗಳು_chunk(chunk))
      ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(chunk) {
    ಆದರೆ(chunk == &bset->ತಲೆ) { /* head chunk is empty */
      ಆದರೆ(!bset->ತಲೆ.ಮುಂದಿನ || (for_ಸ್ಥಾನಾಂತರ < bset->ತಲೆ.ಮುಂದಿನ->ಸ್ಥಾನಾಂತರ)) {
        ಹಿಂತಿರುಗಿ &bset->ತಲೆ;
      }
      /* swap head and next, unlink */
      chunk = bset->ತಲೆ.ಮುಂದಿನ;
      ಸ್ಮರಣೆ_ನಕಲು(&bset->ತಲೆ, chunk, ಗಾತ್ರ(bset->ತಲೆ));
      memset(chunk, 0, ಗಾತ್ರ(*chunk));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      *panchor = chunk->ಮುಂದಿನ; /* unlink */
      memset(chunk, 0, ಗಾತ್ರ(*chunk));
    }
  }
  ಹಿಂತಿರುಗಿ chunk;
}

ಸ್ಥಿರ ರಚನೆ uint32_spbset_chunk *uint32_spbset_ಸೇರಿಸು_chunk(
  ರಚನೆ uint32_spbset *bset, ರಚನೆ uint32_spbset_chunk *nchunk)
{
  ರಚನೆ uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  ಆದರೆ(nchunk->ಸ್ಥಾನಾಂತರ < bset->ತಲೆ.ಸ್ಥಾನಾಂತರ) {
    /* swap chunk and head */
    uint32_t ಸ್ಥಾನಾಂತರ = nchunk->ಸ್ಥಾನಾಂತರ;
    ಸ್ಮರಣೆ_ನಕಲು(nchunk, &bset->ತಲೆ, ಗಾತ್ರ(*nchunk));
    memset(&bset->ತಲೆ, 0, ಗಾತ್ರ(bset->ತಲೆ));
    bset->ತಲೆ.ಮುಂದಿನ = nchunk;
    bset->ತಲೆ.ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ;
    ಹಿಂತಿರುಗಿ &bset->ತಲೆ;
  }
  DEBUGASSERT(nchunk->ಸ್ಥಾನಾಂತರ > bset->ತಲೆ.ಸ್ಥಾನಾಂತರ);
  panchor = &bset->ತಲೆ.ಮುಂದಿನ;
  ಗಾಗಿ(chunk = *panchor; chunk;
      panchor = &chunk->ಮುಂದಿನ, chunk = chunk->ಮುಂದಿನ) {
    ಆದರೆ(chunk->ಸ್ಥಾನಾಂತರ > nchunk->ಸ್ಥಾನಾಂತರ) { /* insert before this chunk */
      nchunk->ಮುಂದಿನ = chunk;
      *panchor = nchunk;
      ಹಿಂತಿರುಗಿ nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  ಹಿಂತಿರುಗಿ nchunk;
}

ಸ್ಥಿರ ರಚನೆ uint32_spbset_chunk *uint32_spbset_get_chunk(
  ರಚನೆ uint32_spbset *bset, uint32_t i, bool grow)
{
  ರಚನೆ uint32_spbset_chunk *chunk;
  uint32_t i_ಸ್ಥಾನಾಂತರ = (i & ~CURL_UINT32_SPBSET_CH_ಮುಖವಾಡ);

  ಆದರೆ(!bset)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಗಾಗಿ(chunk = &bset->ತಲೆ; chunk; chunk = chunk->ಮುಂದಿನ) {
    ಆದರೆ(chunk->ಸ್ಥಾನಾಂತರ == i_ಸ್ಥಾನಾಂತರ)
      ಹಿಂತಿರುಗಿ chunk;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(chunk->ಸ್ಥಾನಾಂತರ > i_ಸ್ಥಾನಾಂತರ)
      ನಿಲ್ಲಿಸು; /* need new chunk here */
  }
  ಆದರೆ(!grow) /* just a check if the chunk exists */
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_ಖಾಲಿಗಳು(bset, i_ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(chunk) {
    chunk->ಸ್ಥಾನಾಂತರ = i_ಸ್ಥಾನಾಂತರ;
    ಆದರೆ(chunk == &bset->ತಲೆ) /* head chunk is empty, stayed linked */
      ಹಿಂತಿರುಗಿ &bset->ತಲೆ;
    /* was really unlinked, need to insert below */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* need a new one */
    chunk = curlx_calloc(1, ಗಾತ್ರ(*chunk));
    ಆದರೆ(!chunk)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    chunk->ಸ್ಥಾನಾಂತರ = i_ಸ್ಥಾನಾಂತರ;
  }

  ಹಿಂತಿರುಗಿ uint32_spbset_ಸೇರಿಸು_chunk(bset, chunk);
}

bool Curl_uint32_spbset_ಕೂಡಿಸು(ರಚನೆ uint32_spbset *bset, uint32_t i)
{
  ರಚನೆ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  ಆದರೆ(!chunk)
    ಹಿಂತಿರುಗಿ FALSE;

  DEBUGASSERT(i >= chunk->ಸ್ಥಾನಾಂತರ);
  i -= chunk->ಸ್ಥಾನಾಂತರ;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  ಹಿಂತಿರುಗಿ TRUE;
}

ಶೂನ್ಯ Curl_uint32_spbset_remove(ರಚನೆ uint32_spbset *bset, uint32_t i)
{
  ರಚನೆ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ಆದರೆ(chunk) {
    DEBUGASSERT(i >= chunk->ಸ್ಥಾನಾಂತರ);
    i -= chunk->ಸ್ಥಾನಾಂತರ;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_ಒಳಗೊಂಡಿದೆ(ರಚನೆ uint32_spbset *bset, uint32_t i)
{
  ರಚನೆ uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ಆದರೆ(chunk) {
    DEBUGASSERT(i >= chunk->ಸ್ಥಾನಾಂತರ);
    i -= chunk->ಸ್ಥಾನಾಂತರ;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    ಹಿಂತಿರುಗಿ (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_uint32_spbset_ಮೊದಲ(ರಚನೆ uint32_spbset *bset, uint32_t *pfirst)
{
  ರಚನೆ uint32_spbset_chunk *chunk;
  uint32_t i;

  ಗಾಗಿ(chunk = &bset->ತಲೆ; chunk; chunk = chunk->ಮುಂದಿನ) {
    ಗಾಗಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ಆದರೆ(chunk->slots[i]) {
        *pfirst = chunk->ಸ್ಥಾನಾಂತರ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        ಹಿಂತಿರುಗಿ TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool uint32_spbset_chunk_ಮೊದಲ(ರಚನೆ uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  ಗಾಗಿ(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ಆದರೆ(chunk->slots[i]) {
      *pfirst = chunk->ಸ್ಥಾನಾಂತರ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool uint32_spbset_chunk_ಮುಂದಿನ(ರಚನೆ uint32_spbset_chunk *chunk,
                                     uint32_t ಕೊನೆಯ,
                                     uint32_t *pnext)
{
  ಆದರೆ(chunk->ಸ್ಥಾನಾಂತರ <= ಕೊನೆಯ) {
    uint64_t x;
    uint32_t i = ((ಕೊನೆಯ - chunk->ಸ್ಥಾನಾಂತರ) / 64);
    ಆದರೆ(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (ಕೊನೆಯ % 64));
      ಆದರೆ(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = ಕೊನೆಯ + CURL_CTZ64(x);
        ಹಿಂತಿರುಗಿ TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      ಗಾಗಿ(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        ಆದರೆ(chunk->slots[i]) {
          *pnext = chunk->ಸ್ಥಾನಾಂತರ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          ಹಿಂತಿರುಗಿ TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_uint32_spbset_ಮುಂದಿನ(ರಚನೆ uint32_spbset *bset, uint32_t ಕೊನೆಯ,
                             uint32_t *pnext)
{
  ರಚನೆ uint32_spbset_chunk *chunk;
  uint32_t ಕೊನೆಯ_ಸ್ಥಾನಾಂತರ;

  ++ಕೊನೆಯ; /* look for the next higher number */
  ಕೊನೆಯ_ಸ್ಥಾನಾಂತರ = (ಕೊನೆಯ & ~CURL_UINT32_SPBSET_CH_ಮುಖವಾಡ);

  ಗಾಗಿ(chunk = &bset->ತಲೆ; chunk; chunk = chunk->ಮುಂದಿನ) {
    ಆದರೆ(chunk->ಸ್ಥಾನಾಂತರ >= ಕೊನೆಯ_ಸ್ಥಾನಾಂತರ) {
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(chunk && (chunk->ಸ್ಥಾನಾಂತರ == ಕೊನೆಯ_ಸ್ಥಾನಾಂತರ)) {
    /* is there a number higher than last in this chunk? */
    ಆದರೆ(uint32_spbset_chunk_ಮುಂದಿನ(chunk, ಕೊನೆಯ, pnext))
      ಹಿಂತಿರುಗಿ TRUE;
    /* not in this chunk */
    chunk = chunk->ಮುಂದಿನ;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  ಆಗ(chunk) {
    ಆದರೆ(uint32_spbset_chunk_ಮೊದಲ(chunk, pnext))
      ಹಿಂತಿರುಗಿ TRUE;
    chunk = chunk->ಮುಂದಿನ;
  }
  *pnext = UINT32_MAX;
  ಹಿಂತಿರುಗಿ FALSE;
}
