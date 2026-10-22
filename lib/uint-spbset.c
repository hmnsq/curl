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
#ضمن "curl_setup.h"

#ضمن "uint-bset.h"
#ضمن "uint-spbset.h"

#جے_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_UINT32_SPBSET_MAGIC  0x70737362
#جے_انت

فراغ Curl_uint32_spbset_init(بنية uint32_spbset *bset)
{
  memset(bset, 0, حجم(*bset));
#جے_تعریف_شدہ DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#جے_انت
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset);
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *اگلا, *chunk;

  لكل(chunk = bset->سر.اگلا; chunk; chunk = اگلا) {
    اگلا = chunk->اگلا;
    curlx_free(chunk);
  }
  memset(&bset->سر, 0, حجم(bset->سر));
}

فراغ Curl_uint32_spbset_destroy(بنية uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_گنتی(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  أرجع n;
}

ساكن bool uint32_spbset_خالی_تھاواں_chunk(بنية uint32_spbset_chunk *chunk)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i])
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_unlink_خالی_تھاواں(
  بنية uint32_spbset *bset, uint32_t for_وتھ)
{
  بنية uint32_spbset_chunk *chunk, **panchor = عدم;
  لكل(chunk = &bset->سر; chunk;
      panchor = &chunk->اگلا, chunk = chunk->اگلا) {
    إذا(uint32_spbset_خالی_تھاواں_chunk(chunk))
      اكسر;
  }
  إذا(chunk) {
    إذا(chunk == &bset->سر) { /* head chunk is empty */
      إذا(!bset->سر.اگلا || (for_وتھ < bset->سر.اگلا->وتھ)) {
        أرجع &bset->سر;
      }
      /* swap head and next, unlink */
      chunk = bset->سر.اگلا;
      حافظہ_نقل(&bset->سر, chunk, حجم(bset->سر));
      memset(chunk, 0, حجم(*chunk));
    }
    وإلا {
      *panchor = chunk->اگلا; /* unlink */
      memset(chunk, 0, حجم(*chunk));
    }
  }
  أرجع chunk;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_داخل_کرو_chunk(
  بنية uint32_spbset *bset, بنية uint32_spbset_chunk *nchunk)
{
  بنية uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  إذا(nchunk->وتھ < bset->سر.وتھ) {
    /* swap chunk and head */
    uint32_t وتھ = nchunk->وتھ;
    حافظہ_نقل(nchunk, &bset->سر, حجم(*nchunk));
    memset(&bset->سر, 0, حجم(bset->سر));
    bset->سر.اگلا = nchunk;
    bset->سر.وتھ = وتھ;
    أرجع &bset->سر;
  }
  DEBUGASSERT(nchunk->وتھ > bset->سر.وتھ);
  panchor = &bset->سر.اگلا;
  لكل(chunk = *panchor; chunk;
      panchor = &chunk->اگلا, chunk = chunk->اگلا) {
    إذا(chunk->وتھ > nchunk->وتھ) { /* insert before this chunk */
      nchunk->اگلا = chunk;
      *panchor = nchunk;
      أرجع nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  أرجع nchunk;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_get_chunk(
  بنية uint32_spbset *bset, uint32_t i, bool grow)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i_وتھ = (i & ~CURL_UINT32_SPBSET_CH_نقاب);

  إذا(!bset)
    أرجع عدم;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    إذا(chunk->وتھ == i_وتھ)
      أرجع chunk;
    وإلا إذا(chunk->وتھ > i_وتھ)
      اكسر; /* need new chunk here */
  }
  إذا(!grow) /* just a check if the chunk exists */
    أرجع عدم;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_خالی_تھاواں(bset, i_وتھ);
  إذا(chunk) {
    chunk->وتھ = i_وتھ;
    إذا(chunk == &bset->سر) /* head chunk is empty, stayed linked */
      أرجع &bset->سر;
    /* was really unlinked, need to insert below */
  }
  وإلا {
    /* need a new one */
    chunk = curlx_calloc(1, حجم(*chunk));
    إذا(!chunk)
      أرجع عدم;
    chunk->وتھ = i_وتھ;
  }

  أرجع uint32_spbset_داخل_کرو_chunk(bset, chunk);
}

bool Curl_uint32_spbset_جوڑو(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  إذا(!chunk)
    أرجع FALSE;

  DEBUGASSERT(i >= chunk->وتھ);
  i -= chunk->وتھ;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  أرجع TRUE;
}

فراغ Curl_uint32_spbset_remove(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->وتھ);
    i -= chunk->وتھ;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_شامل_اے(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->وتھ);
    i -= chunk->وتھ;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    أرجع (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  أرجع FALSE;
}

bool Curl_uint32_spbset_پہلا(بنية uint32_spbset *bset, uint32_t *pfirst)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i]) {
        *pfirst = chunk->وتھ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        أرجع TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_پہلا(بنية uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i]) {
      *pfirst = chunk->وتھ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      أرجع TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_اگلا(بنية uint32_spbset_chunk *chunk,
                                     uint32_t آخری,
                                     uint32_t *pnext)
{
  إذا(chunk->وتھ <= آخری) {
    uint64_t x;
    uint32_t i = ((آخری - chunk->وتھ) / 64);
    إذا(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (آخری % 64));
      إذا(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = آخری + CURL_CTZ64(x);
        أرجع TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      لكل(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        إذا(chunk->slots[i]) {
          *pnext = chunk->وتھ + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          أرجع TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}

bool Curl_uint32_spbset_اگلا(بنية uint32_spbset *bset, uint32_t آخری,
                             uint32_t *pnext)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t آخری_وتھ;

  ++آخری; /* look for the next higher number */
  آخری_وتھ = (آخری & ~CURL_UINT32_SPBSET_CH_نقاب);

  لكل(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    إذا(chunk->وتھ >= آخری_وتھ) {
      اكسر;
    }
  }

  إذا(chunk && (chunk->وتھ == آخری_وتھ)) {
    /* is there a number higher than last in this chunk? */
    إذا(uint32_spbset_chunk_اگلا(chunk, آخری, pnext))
      أرجع TRUE;
    /* not in this chunk */
    chunk = chunk->اگلا;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  طالما(chunk) {
    إذا(uint32_spbset_chunk_پہلا(chunk, pnext))
      أرجع TRUE;
    chunk = chunk->اگلا;
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}
