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

#اگر_تعریف_شده DEBUGBUILD
#تعریف_کن CURL_UINT32_SPBSET_MAGIC  0x70737362
#پایان_اگر

فراغ Curl_uint32_spbset_init(بنية uint32_spbset *bset)
{
  memset(bset, 0, حجم(*bset));
#اگر_تعریف_شده DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#پایان_اگر
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset);
UNITTEST فراغ uint32_spbset_clear(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *بعدی, *chunk;

  لكل(chunk = bset->سر.بعدی; chunk; chunk = بعدی) {
    بعدی = chunk->بعدی;
    curlx_free(chunk);
  }
  memset(&bset->سر, 0, حجم(bset->سر));
}

فراغ Curl_uint32_spbset_destroy(بنية uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_شمار(بنية uint32_spbset *bset)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بعدی) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  أرجع n;
}

ساكن bool uint32_spbset_خالی_chunk(بنية uint32_spbset_chunk *chunk)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i])
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_unlink_خالی(
  بنية uint32_spbset *bset, uint32_t for_جابه_جایی)
{
  بنية uint32_spbset_chunk *chunk, **panchor = عدم;
  لكل(chunk = &bset->سر; chunk;
      panchor = &chunk->بعدی, chunk = chunk->بعدی) {
    إذا(uint32_spbset_خالی_chunk(chunk))
      اكسر;
  }
  إذا(chunk) {
    إذا(chunk == &bset->سر) { /* head chunk is empty */
      إذا(!bset->سر.بعدی || (for_جابه_جایی < bset->سر.بعدی->جابه_جایی)) {
        أرجع &bset->سر;
      }
      /* swap head and next, unlink */
      chunk = bset->سر.بعدی;
      کپی_حافظه(&bset->سر, chunk, حجم(bset->سر));
      memset(chunk, 0, حجم(*chunk));
    }
    وإلا {
      *panchor = chunk->بعدی; /* unlink */
      memset(chunk, 0, حجم(*chunk));
    }
  }
  أرجع chunk;
}

ساكن بنية uint32_spbset_chunk *uint32_spbset_درج_chunk(
  بنية uint32_spbset *bset, بنية uint32_spbset_chunk *nchunk)
{
  بنية uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  إذا(nchunk->جابه_جایی < bset->سر.جابه_جایی) {
    /* swap chunk and head */
    uint32_t جابه_جایی = nchunk->جابه_جایی;
    کپی_حافظه(nchunk, &bset->سر, حجم(*nchunk));
    memset(&bset->سر, 0, حجم(bset->سر));
    bset->سر.بعدی = nchunk;
    bset->سر.جابه_جایی = جابه_جایی;
    أرجع &bset->سر;
  }
  DEBUGASSERT(nchunk->جابه_جایی > bset->سر.جابه_جایی);
  panchor = &bset->سر.بعدی;
  لكل(chunk = *panchor; chunk;
      panchor = &chunk->بعدی, chunk = chunk->بعدی) {
    إذا(chunk->جابه_جایی > nchunk->جابه_جایی) { /* insert before this chunk */
      nchunk->بعدی = chunk;
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
  uint32_t i_جابه_جایی = (i & ~CURL_UINT32_SPBSET_CH_ماسک);

  إذا(!bset)
    أرجع عدم;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بعدی) {
    إذا(chunk->جابه_جایی == i_جابه_جایی)
      أرجع chunk;
    وإلا إذا(chunk->جابه_جایی > i_جابه_جایی)
      اكسر; /* need new chunk here */
  }
  إذا(!grow) /* just a check if the chunk exists */
    أرجع عدم;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_خالی(bset, i_جابه_جایی);
  إذا(chunk) {
    chunk->جابه_جایی = i_جابه_جایی;
    إذا(chunk == &bset->سر) /* head chunk is empty, stayed linked */
      أرجع &bset->سر;
    /* was really unlinked, need to insert below */
  }
  وإلا {
    /* need a new one */
    chunk = curlx_calloc(1, حجم(*chunk));
    إذا(!chunk)
      أرجع عدم;
    chunk->جابه_جایی = i_جابه_جایی;
  }

  أرجع uint32_spbset_درج_chunk(bset, chunk);
}

bool Curl_uint32_spbset_جمع_کن(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  إذا(!chunk)
    أرجع FALSE;

  DEBUGASSERT(i >= chunk->جابه_جایی);
  i -= chunk->جابه_جایی;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  أرجع TRUE;
}

فراغ Curl_uint32_spbset_remove(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->جابه_جایی);
    i -= chunk->جابه_جایی;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_شامل_است(بنية uint32_spbset *bset, uint32_t i)
{
  بنية uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  إذا(chunk) {
    DEBUGASSERT(i >= chunk->جابه_جایی);
    i -= chunk->جابه_جایی;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    أرجع (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  أرجع FALSE;
}

bool Curl_uint32_spbset_اولی(بنية uint32_spbset *bset, uint32_t *pfirst)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t i;

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بعدی) {
    لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      إذا(chunk->slots[i]) {
        *pfirst = chunk->جابه_جایی + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        أرجع TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_اولی(بنية uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  لكل(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    إذا(chunk->slots[i]) {
      *pfirst = chunk->جابه_جایی + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      أرجع TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  أرجع FALSE;
}

ساكن bool uint32_spbset_chunk_بعدی(بنية uint32_spbset_chunk *chunk,
                                     uint32_t آخرین,
                                     uint32_t *pnext)
{
  إذا(chunk->جابه_جایی <= آخرین) {
    uint64_t x;
    uint32_t i = ((آخرین - chunk->جابه_جایی) / 64);
    إذا(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (آخرین % 64));
      إذا(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = آخرین + CURL_CTZ64(x);
        أرجع TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      لكل(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        إذا(chunk->slots[i]) {
          *pnext = chunk->جابه_جایی + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          أرجع TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}

bool Curl_uint32_spbset_بعدی(بنية uint32_spbset *bset, uint32_t آخرین,
                             uint32_t *pnext)
{
  بنية uint32_spbset_chunk *chunk;
  uint32_t آخرین_جابه_جایی;

  ++آخرین; /* look for the next higher number */
  آخرین_جابه_جایی = (آخرین & ~CURL_UINT32_SPBSET_CH_ماسک);

  لكل(chunk = &bset->سر; chunk; chunk = chunk->بعدی) {
    إذا(chunk->جابه_جایی >= آخرین_جابه_جایی) {
      اكسر;
    }
  }

  إذا(chunk && (chunk->جابه_جایی == آخرین_جابه_جایی)) {
    /* is there a number higher than last in this chunk? */
    إذا(uint32_spbset_chunk_بعدی(chunk, آخرین, pnext))
      أرجع TRUE;
    /* not in this chunk */
    chunk = chunk->بعدی;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  طالما(chunk) {
    إذا(uint32_spbset_chunk_اولی(chunk, pnext))
      أرجع TRUE;
    chunk = chunk->بعدی;
  }
  *pnext = UINT32_MAX;
  أرجع FALSE;
}
