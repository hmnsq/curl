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

#که_تعریف_شوی DEBUGBUILD
#تعریف_کړه CURL_UINT32_SPBSET_MAGIC  0x70737362
#د_که_پای

فراغ Curl_uint32_spbset_init(بنية uint32_spbset *bset)
{
  memset(bset, 0, حجم(*bset));
#که_تعریف_شوی DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#د_که_پای
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset);
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *بل, *chunk;

  لكل(chunk = bset->سر.بل; chunk; chunk = بل) {
    بل = chunk->بل;
    curlx_free(chunk);
  }
  memset(&bset->سر, 0, حجم(bset->سر));
}

فراغ Curl_uint32_spbset_destroy(بنية uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_شمېر(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بل) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  أرجع n;
}

ساكن bool uint32_spbset_تشه_chunk(بنية uint32_spbset_chunk *chunk)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i])
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_unlink_تشه(
  بنية uint32_spbset *bset, uint32_t for_دننه_واټن)
{
  بنية uint32_spbset_chunk *chunk, **panchor = عدم;
  لكل(chunk = &bset->سر; chunk;
      panchor = &chunk->بل, chunk = chunk->بل) {
    إذا(uint32_spbset_تشه_chunk(chunk))
      اكسر;
  }
  إذا(chunk) {
    إذا(chunk == &bset->سر) { /* head chunk is empty */
      إذا(!bset->سر.بل || (for_دننه_واټن < bset->سر.بل->دننه_واټن)) {
        أرجع &bset->سر;
      }
      /* swap head and next, unlink */
      chunk = bset->سر.بل;
      حافظه_نقل(&bset->سر, chunk, حجم(bset->سر));
      memset(chunk, 0, حجم(*chunk));
    }
    وإلا {
      *panchor = chunk->بل; /* unlink */
      memset(chunk, 0, حجم(*chunk));
    }
  }
  أرجع chunk;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_ورننباسه_chunk(
  بنية uint32_spbset *bset, بنية uint32_spbset_chunk *nchunk)
{
  بنية uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  إذا(nchunk->دننه_واټن < bset->سر.دننه_واټن) {
    /* swap chunk and head */
    uint32_t دننه_واټن = nchunk->دننه_واټن;
    حافظه_نقل(nchunk, &bset->سر, حجم(*nchunk));
    memset(&bset->سر, 0, حجم(bset->سر));
    bset->سر.بل = nchunk;
    bset->سر.دننه_واټن = دننه_واټن;
    أرجع &bset->سر;
  }
  DEBUGASSERT(nchunk->دننه_واټن > bset->سر.دننه_واټن);
  panchor = &bset->سر.بل;
  لكل(chunk = *panchor; chunk;
      panchor = &chunk->بل, chunk = chunk->بل) {
    إذا(chunk->دننه_واټن > nchunk->دننه_واټن) { /* insert before this chunk */
      nchunk->بل = chunk;
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
  uint32_t i_دننه_واټن = (i & ~CURL_UINT32_SPBSET_CH_ماسک);

  إذا(!bset)
    أرجع عدم;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بل) {
    إذا(chunk->دننه_واټن == i_دننه_واټن)
      أرجع chunk;
    وإلا إذا(chunk->دننه_واټن > i_دننه_واټن)
      اكسر; /* need new chunk here */
  }
  إذا(!grow) /* just a check if the chunk exists */
    أرجع عدم;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_تشه(bset, i_دننه_واټن);
  إذا(chunk) {
    chunk->دننه_واټن = i_دننه_واټن;
    إذا(chunk == &bset->سر) /* head chunk is empty, stayed linked */
      أرجع &bset->سر;
    /* was really unlinked, need to insert below */
  }
  وإلا {
    /* need a new one */
    chunk = curlx_calloc(1, حجم(*chunk));
    إذا(!chunk)
      أرجع عدم;
    chunk->دننه_واټن = i_دننه_واټن;
  }

  أرجع uint32_spbset_ورننباسه_chunk(bset, chunk);
}

bool Curl_uint32_spbset_جمع_کړه(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  إذا(!chunk)
    أرجع FALSE;

  DEBUGASSERT(i >= chunk->دننه_واټن);
  i -= chunk->دننه_واټن;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  أرجع TRUE;
}

فراغ Curl_uint32_spbset_remove(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->دننه_واټن);
    i -= chunk->دننه_واټن;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_لري(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->دننه_واټن);
    i -= chunk->دننه_واټن;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    أرجع (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  أرجع FALSE;
}

bool Curl_uint32_spbset_لومړی(بنية uint32_spbset *bset, uint32_t *pfirst)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بل) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i]) {
        *pfirst = chunk->دننه_واټن + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        أرجع TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_لومړی(بنية uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i]) {
      *pfirst = chunk->دننه_واټن + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      أرجع TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_بل(بنية uint32_spbset_chunk *chunk,
                                     uint32_t وروستی,
                                     uint32_t *pnext)
{
  إذا(chunk->دننه_واټن <= وروستی) {
    uint64_t x;
    uint32_t i = ((وروستی - chunk->دننه_واټن) / 64);
    إذا(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (وروستی % 64));
      إذا(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = وروستی + CURL_CTZ64(x);
        أرجع TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      لكل(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        إذا(chunk->slots[i]) {
          *pnext = chunk->دننه_واټن + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          أرجع TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}

bool Curl_uint32_spbset_بل(بنية uint32_spbset *bset, uint32_t وروستی,
                             uint32_t *pnext)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t وروستی_دننه_واټن;

  ++وروستی; /* look for the next higher number */
  وروستی_دننه_واټن = (وروستی & ~CURL_UINT32_SPBSET_CH_ماسک);

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بل) {
    إذا(chunk->دننه_واټن >= وروستی_دننه_واټن) {
      اكسر;
    }
  }

  إذا(chunk && (chunk->دننه_واټن == وروستی_دننه_واټن)) {
    /* is there a number higher than last in this chunk? */
    إذا(uint32_spbset_chunk_بل(chunk, وروستی, pnext))
      أرجع TRUE;
    /* not in this chunk */
    chunk = chunk->بل;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  طالما(chunk) {
    إذا(uint32_spbset_chunk_لومړی(chunk, pnext))
      أرجع TRUE;
    chunk = chunk->بل;
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}
