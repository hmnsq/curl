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

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو CURL_UINT32_SPBSET_MAGIC  0x70737362
#جيڪڏهن_پورو

فراغ Curl_uint32_spbset_init(بنية uint32_spbset *bset)
{
  memset(bset, 0, حجم(*bset));
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#جيڪڏهن_پورو
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset);
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *ايندڙ, *chunk;

  لكل(chunk = bset->سر.ايندڙ; chunk; chunk = ايندڙ) {
    ايندڙ = chunk->ايندڙ;
    curlx_free(chunk);
  }
  memset(&bset->سر, 0, حجم(bset->سر));
}

فراغ Curl_uint32_spbset_destroy(بنية uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_ڳڻپ(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->ايندڙ) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  أرجع n;
}

ساكن bool uint32_spbset_خالي_جايون_chunk(بنية uint32_spbset_chunk *chunk)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i])
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_unlink_خالي_جايون(
  بنية uint32_spbset *bset, uint32_t for_وٿي)
{
  بنية uint32_spbset_chunk *chunk, **panchor = عدم;
  لكل(chunk = &bset->سر; chunk;
      panchor = &chunk->ايندڙ, chunk = chunk->ايندڙ) {
    إذا(uint32_spbset_خالي_جايون_chunk(chunk))
      اكسر;
  }
  إذا(chunk) {
    إذا(chunk == &bset->سر) { /* head chunk is empty */
      إذا(!bset->سر.ايندڙ || (for_وٿي < bset->سر.ايندڙ->وٿي)) {
        أرجع &bset->سر;
      }
      /* swap head and next, unlink */
      chunk = bset->سر.ايندڙ;
      يادگيري_نقل(&bset->سر, chunk, حجم(bset->سر));
      memset(chunk, 0, حجم(*chunk));
    }
    وإلا {
      *panchor = chunk->ايندڙ; /* unlink */
      memset(chunk, 0, حجم(*chunk));
    }
  }
  أرجع chunk;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_وجهو_chunk(
  بنية uint32_spbset *bset, بنية uint32_spbset_chunk *nchunk)
{
  بنية uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  إذا(nchunk->وٿي < bset->سر.وٿي) {
    /* swap chunk and head */
    uint32_t وٿي = nchunk->وٿي;
    يادگيري_نقل(nchunk, &bset->سر, حجم(*nchunk));
    memset(&bset->سر, 0, حجم(bset->سر));
    bset->سر.ايندڙ = nchunk;
    bset->سر.وٿي = وٿي;
    أرجع &bset->سر;
  }
  DEBUGASSERT(nchunk->وٿي > bset->سر.وٿي);
  panchor = &bset->سر.ايندڙ;
  لكل(chunk = *panchor; chunk;
      panchor = &chunk->ايندڙ, chunk = chunk->ايندڙ) {
    إذا(chunk->وٿي > nchunk->وٿي) { /* insert before this chunk */
      nchunk->ايندڙ = chunk;
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
  uint32_t i_وٿي = (i & ~CURL_UINT32_SPBSET_CH_نقاب);

  إذا(!bset)
    أرجع عدم;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->ايندڙ) {
    إذا(chunk->وٿي == i_وٿي)
      أرجع chunk;
    وإلا إذا(chunk->وٿي > i_وٿي)
      اكسر; /* need new chunk here */
  }
  إذا(!grow) /* just a check if the chunk exists */
    أرجع عدم;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_خالي_جايون(bset, i_وٿي);
  إذا(chunk) {
    chunk->وٿي = i_وٿي;
    إذا(chunk == &bset->سر) /* head chunk is empty, stayed linked */
      أرجع &bset->سر;
    /* was really unlinked, need to insert below */
  }
  وإلا {
    /* need a new one */
    chunk = curlx_calloc(1, حجم(*chunk));
    إذا(!chunk)
      أرجع عدم;
    chunk->وٿي = i_وٿي;
  }

  أرجع uint32_spbset_وجهو_chunk(bset, chunk);
}

bool Curl_uint32_spbset_جوڙيو(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  إذا(!chunk)
    أرجع FALSE;

  DEBUGASSERT(i >= chunk->وٿي);
  i -= chunk->وٿي;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  أرجع TRUE;
}

فراغ Curl_uint32_spbset_remove(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->وٿي);
    i -= chunk->وٿي;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_شامل_آهي(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->وٿي);
    i -= chunk->وٿي;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    أرجع (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  أرجع FALSE;
}

bool Curl_uint32_spbset_پهريون(بنية uint32_spbset *bset, uint32_t *pfirst)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->ايندڙ) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i]) {
        *pfirst = chunk->وٿي + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        أرجع TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_پهريون(بنية uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i]) {
      *pfirst = chunk->وٿي + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      أرجع TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_ايندڙ(بنية uint32_spbset_chunk *chunk,
                                     uint32_t آخري,
                                     uint32_t *pnext)
{
  إذا(chunk->وٿي <= آخري) {
    uint64_t x;
    uint32_t i = ((آخري - chunk->وٿي) / 64);
    إذا(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (آخري % 64));
      إذا(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = آخري + CURL_CTZ64(x);
        أرجع TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      لكل(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        إذا(chunk->slots[i]) {
          *pnext = chunk->وٿي + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          أرجع TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}

bool Curl_uint32_spbset_ايندڙ(بنية uint32_spbset *bset, uint32_t آخري,
                             uint32_t *pnext)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t آخري_وٿي;

  ++آخري; /* look for the next higher number */
  آخري_وٿي = (آخري & ~CURL_UINT32_SPBSET_CH_نقاب);

  لكل(chunk = &bset->سر; chunk; chunk = chunk->ايندڙ) {
    إذا(chunk->وٿي >= آخري_وٿي) {
      اكسر;
    }
  }

  إذا(chunk && (chunk->وٿي == آخري_وٿي)) {
    /* is there a number higher than last in this chunk? */
    إذا(uint32_spbset_chunk_ايندڙ(chunk, آخري, pnext))
      أرجع TRUE;
    /* not in this chunk */
    chunk = chunk->ايندڙ;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  طالما(chunk) {
    إذا(uint32_spbset_chunk_پهريون(chunk, pnext))
      أرجع TRUE;
    chunk = chunk->ايندڙ;
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}
