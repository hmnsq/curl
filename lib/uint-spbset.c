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
#شامل "curl_setup.h"

#شامل "uint-bset.h"
#شامل "uint-spbset.h"

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو CURL_UINT32_SPBSET_MAGIC  0x70737362
#اگر_ختم

خالی Curl_uint32_spbset_init(ساخت uint32_spbset *bset)
{
  memset(bset, 0, حجم(*bset));
#اگر_تعریف_شدہ DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#اگر_ختم
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST خالی uint32_spbset_clear(ساخت uint32_spbset *bset);
UNITTEST خالی uint32_spbset_clear(ساخت uint32_spbset *bset)
{
  ساخت uint32_spbset_chunk *اگلا, *chunk;

  برائے(chunk = bset->سر.اگلا; chunk; chunk = اگلا) {
    اگلا = chunk->اگلا;
    curlx_free(chunk);
  }
  memset(&bset->سر, 0, حجم(bset->سر));
}

خالی Curl_uint32_spbset_destroy(ساخت uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_گنتی(ساخت uint32_spbset *bset)
{
  ساخت uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  برائے(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    برائے(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      اگر(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  واپس n;
}

ساکن bool uint32_spbset_خالی_خانے_chunk(ساخت uint32_spbset_chunk *chunk)
{
  uint32_t i;
  برائے(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    اگر(chunk->slots[i])
      واپس FALSE;
  }
  واپس TRUE;
}

ساکن ساخت uint32_spbset_chunk *uint32_spbset_unlink_خالی_خانے(
  ساخت uint32_spbset *bset, uint32_t for_انحراف)
{
  ساخت uint32_spbset_chunk *chunk, **panchor = تہی;
  برائے(chunk = &bset->سر; chunk;
      panchor = &chunk->اگلا, chunk = chunk->اگلا) {
    اگر(uint32_spbset_خالی_خانے_chunk(chunk))
      توڑو;
  }
  اگر(chunk) {
    اگر(chunk == &bset->سر) { /* head chunk is empty */
      اگر(!bset->سر.اگلا || (for_انحراف < bset->سر.اگلا->انحراف)) {
        واپس &bset->سر;
      }
      /* swap head and next, unlink */
      chunk = bset->سر.اگلا;
      حافظہ_نقل(&bset->سر, chunk, حجم(bset->سر));
      memset(chunk, 0, حجم(*chunk));
    }
    ورنہ {
      *panchor = chunk->اگلا; /* unlink */
      memset(chunk, 0, حجم(*chunk));
    }
  }
  واپس chunk;
}

ساکن ساخت uint32_spbset_chunk *uint32_spbset_داخل_کرو_chunk(
  ساخت uint32_spbset *bset, ساخت uint32_spbset_chunk *nchunk)
{
  ساخت uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  اگر(nchunk->انحراف < bset->سر.انحراف) {
    /* swap chunk and head */
    uint32_t انحراف = nchunk->انحراف;
    حافظہ_نقل(nchunk, &bset->سر, حجم(*nchunk));
    memset(&bset->سر, 0, حجم(bset->سر));
    bset->سر.اگلا = nchunk;
    bset->سر.انحراف = انحراف;
    واپس &bset->سر;
  }
  DEBUGASSERT(nchunk->انحراف > bset->سر.انحراف);
  panchor = &bset->سر.اگلا;
  برائے(chunk = *panchor; chunk;
      panchor = &chunk->اگلا, chunk = chunk->اگلا) {
    اگر(chunk->انحراف > nchunk->انحراف) { /* insert before this chunk */
      nchunk->اگلا = chunk;
      *panchor = nchunk;
      واپس nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  واپس nchunk;
}

ساکن ساخت uint32_spbset_chunk *uint32_spbset_get_chunk(
  ساخت uint32_spbset *bset, uint32_t i, bool grow)
{
  ساخت uint32_spbset_chunk *chunk;
  uint32_t i_انحراف = (i & ~CURL_UINT32_SPBSET_CH_نقاب);

  اگر(!bset)
    واپس تہی;

  برائے(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    اگر(chunk->انحراف == i_انحراف)
      واپس chunk;
    ورنہ اگر(chunk->انحراف > i_انحراف)
      توڑو; /* need new chunk here */
  }
  اگر(!grow) /* just a check if the chunk exists */
    واپس تہی;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_خالی_خانے(bset, i_انحراف);
  اگر(chunk) {
    chunk->انحراف = i_انحراف;
    اگر(chunk == &bset->سر) /* head chunk is empty, stayed linked */
      واپس &bset->سر;
    /* was really unlinked, need to insert below */
  }
  ورنہ {
    /* need a new one */
    chunk = curlx_calloc(1, حجم(*chunk));
    اگر(!chunk)
      واپس تہی;
    chunk->انحراف = i_انحراف;
  }

  واپس uint32_spbset_داخل_کرو_chunk(bset, chunk);
}

bool Curl_uint32_spbset_جوڑو(ساخت uint32_spbset *bset, uint32_t i)
{
  ساخت uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  اگر(!chunk)
    واپس FALSE;

  DEBUGASSERT(i >= chunk->انحراف);
  i -= chunk->انحراف;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  واپس TRUE;
}

خالی Curl_uint32_spbset_remove(ساخت uint32_spbset *bset, uint32_t i)
{
  ساخت uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  اگر(chunk) {
    DEBUGASSERT(i >= chunk->انحراف);
    i -= chunk->انحراف;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_شامل_ہے(ساخت uint32_spbset *bset, uint32_t i)
{
  ساخت uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  اگر(chunk) {
    DEBUGASSERT(i >= chunk->انحراف);
    i -= chunk->انحراف;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    واپس (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  واپس FALSE;
}

bool Curl_uint32_spbset_پہلا(ساخت uint32_spbset *bset, uint32_t *pfirst)
{
  ساخت uint32_spbset_chunk *chunk;
  uint32_t i;

  برائے(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    برائے(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      اگر(chunk->slots[i]) {
        *pfirst = chunk->انحراف + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        واپس TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  واپس FALSE;
}

ساکن bool uint32_spbset_chunk_پہلا(ساخت uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  برائے(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    اگر(chunk->slots[i]) {
      *pfirst = chunk->انحراف + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      واپس TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  واپس FALSE;
}

ساکن bool uint32_spbset_chunk_اگلا(ساخت uint32_spbset_chunk *chunk,
                                     uint32_t آخری,
                                     uint32_t *pnext)
{
  اگر(chunk->انحراف <= آخری) {
    uint64_t x;
    uint32_t i = ((آخری - chunk->انحراف) / 64);
    اگر(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (آخری % 64));
      اگر(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = آخری + CURL_CTZ64(x);
        واپس TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      برائے(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        اگر(chunk->slots[i]) {
          *pnext = chunk->انحراف + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          واپس TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  واپس FALSE;
}

bool Curl_uint32_spbset_اگلا(ساخت uint32_spbset *bset, uint32_t آخری,
                             uint32_t *pnext)
{
  ساخت uint32_spbset_chunk *chunk;
  uint32_t آخری_انحراف;

  ++آخری; /* look for the next higher number */
  آخری_انحراف = (آخری & ~CURL_UINT32_SPBSET_CH_نقاب);

  برائے(chunk = &bset->سر; chunk; chunk = chunk->اگلا) {
    اگر(chunk->انحراف >= آخری_انحراف) {
      توڑو;
    }
  }

  اگر(chunk && (chunk->انحراف == آخری_انحراف)) {
    /* is there a number higher than last in this chunk? */
    اگر(uint32_spbset_chunk_اگلا(chunk, آخری, pnext))
      واپس TRUE;
    /* not in this chunk */
    chunk = chunk->اگلا;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  جب(chunk) {
    اگر(uint32_spbset_chunk_پہلا(chunk, pnext))
      واپس TRUE;
    chunk = chunk->اگلا;
  }
  *pnext = UINT32_MAX;
  واپس FALSE;
}
