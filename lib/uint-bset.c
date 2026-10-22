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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_UINT32_BSET_MAGIC  0x62757473
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_uint32_bset_init(ਬਣਤਰ uint32_bset *bset)
{
  memset(bset, 0, ਆਕਾਰ(*bset));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  bset->init = CURL_UINT32_BSET_MAGIC;
#ਜੇ_ਅੰਤ
}

CURLcode Curl_uint32_bset_resize(ਬਣਤਰ uint32_bset *bset, uint32_t nmax)
{
  uint32_t nslots = (nmax < (UINT32_MAX - 63)) ?
                    ((nmax + 63) / 64) : (UINT32_MAX / 64);

  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  ਜੇ(nslots != bset->nslots) {
    uint64_t *slots = curlx_calloc(nslots, ਆਕਾਰ(uint64_t));
    ਜੇ(!slots)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਜੇ(bset->slots) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(slots, bset->slots,
             (CURLMIN(nslots, bset->nslots) * ਆਕਾਰ(uint64_t)));
      curlx_free(bset->slots);
    }
    bset->slots = slots;
    bset->nslots = nslots;
    bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ = 0;
  }
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_uint32_bset_destroy(ਬਣਤਰ uint32_bset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  curlx_free(bset->slots);
  memset(bset, 0, ਆਕਾਰ(*bset));
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
/* @unittest 3211 */
UNITTEST uint32_t uint32_bset_ਸਮਰੱਥਾ(ਬਣਤਰ uint32_bset *bset);
UNITTEST uint32_t uint32_bset_ਸਮਰੱਥਾ(ਬਣਤਰ uint32_bset *bset)
{
  ਵਾਪਸ bset->nslots * 64;
}
#ਜੇ_ਅੰਤ

uint32_t Curl_uint32_bset_ਗਿਣਤੀ(ਬਣਤਰ uint32_bset *bset)
{
  uint32_t i;
  uint32_t n = 0;
  ਲਈ(i = bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ; i < bset->nslots; ++i) {
    ਜੇ(bset->slots[i])
      n += CURL_POPCOUNT64(bset->slots[i]);
  }
  ਵਾਪਸ n;
}

bool Curl_uint32_bset_ਖਾਲੀ_ਥਾਵਾਂ(ਬਣਤਰ uint32_bset *bset)
{
  uint32_t i;
  ਲਈ(i = bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ; i < bset->nslots; ++i) {
    ਜੇ(bset->slots[i])
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਖਾਲੀ Curl_uint32_bset_clear(ਬਣਤਰ uint32_bset *bset)
{
  ਜੇ(bset->nslots) {
    memset(bset->slots, 0, bset->nslots * ਆਕਾਰ(uint64_t));
    bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ = UINT32_MAX;
  }
}

bool Curl_uint32_bset_ਜੋੜੋ(ਬਣਤਰ uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  ਜੇ(islot >= bset->nslots)
    ਵਾਪਸ FALSE;
  bset->slots[islot] |= ((uint64_t)1 << (i % 64));
  ਜੇ(islot < bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ)
    bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ = islot;
  ਵਾਪਸ TRUE;
}

ਖਾਲੀ Curl_uint32_bset_remove(ਬਣਤਰ uint32_bset *bset, uint32_t i)
{
  size_t islot = i / 64;
  ਜੇ(islot < bset->nslots)
    bset->slots[islot] &= ~((uint64_t)1 << (i % 64));
}

bool Curl_uint32_bset_ਸ਼ਾਮਲ_ਹੈ(ਬਣਤਰ uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  ਜੇ(islot >= bset->nslots)
    ਵਾਪਸ FALSE;
  ਵਾਪਸ (bset->slots[islot] & ((uint64_t)1 << (i % 64))) != 0;
}

bool Curl_uint32_bset_ਪਹਿਲਾ(ਬਣਤਰ uint32_bset *bset, uint32_t *pfirst)
{
  uint32_t i;
  ਲਈ(i = bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ; i < bset->nslots; ++i) {
    ਜੇ(bset->slots[i]) {
      *pfirst = (i * 64) + CURL_CTZ64(bset->slots[i]);
      bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ = i;
      ਵਾਪਸ TRUE;
    }
  }
  bset->ਪਹਿਲਾ_ਥਾਂ_ਵਰਤਿਆ = *pfirst = UINT32_MAX;
  ਵਾਪਸ FALSE;
}

bool Curl_uint32_bset_ਅਗਲਾ(ਬਣਤਰ uint32_bset *bset, uint32_t ਆਖਰੀ,
                           uint32_t *pnext)
{
  uint32_t islot;
  uint64_t x;

  ++ਆਖਰੀ; /* look for number one higher than last */
  islot = ਆਖਰੀ / 64; /* the slot this would be in */
  ਜੇ(islot < bset->nslots) {
    /* shift away the bits we already iterated in this slot */
    x = (bset->slots[islot] >> (ਆਖਰੀ % 64));
    ਜੇ(x) {
      /* more bits set, next is `last` + trailing 0s of the shifted slot */
      *pnext = ਆਖਰੀ + CURL_CTZ64(x);
      ਵਾਪਸ TRUE;
    }
    /* no more bits set in the last slot, scan forward */
    ਲਈ(islot = islot + 1; islot < bset->nslots; ++islot) {
      ਜੇ(bset->slots[islot]) {
        *pnext = (islot * 64) + CURL_CTZ64(bset->slots[islot]);
        ਵਾਪਸ TRUE;
      }
    }
  }
  *pnext = UINT32_MAX; /* a value we cannot store */
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_POPCOUNT64_IMPLEMENT
uint32_t Curl_popcount64(uint64_t x)
{
  /* Compute the "Hamming Distance" between 'x' and 0,
   * which is the number of set bits in 'x'.
   * See: https://en.wikipedia.org/wiki/Hamming_weight */
  ਅਚਲ uint64_t m1 = 0x5555555555555555LL; /* 0101+ */
  ਅਚਲ uint64_t m2 = 0x3333333333333333LL; /* 00110011+ */
  ਅਚਲ uint64_t m4 = 0x0f0f0f0f0f0f0f0fLL; /* 00001111+ */
  /* 1 + 256^1 + 256^2 + 256^3 + ... + 256^7 */
  ਅਚਲ uint64_t h01 = 0x0101010101010101LL;
  x -= (x >> 1) & m1;             /* replace every 2 bits with bits present */
  x = (x & m2) + ((x >> 2) & m2); /* replace every nibble with bits present */
  x = (x + (x >> 4)) & m4;        /* replace every byte with bits present */
  /* top 8 bits of x + (x << 8) + (x << 16) + (x << 24) + ... which makes the
   * top byte the sum of all individual 8 bytes, throw away the rest */
  ਵਾਪਸ (uint32_t)((x * h01) >> 56);
}
#ਜੇ_ਅੰਤ /* CURL_POPCOUNT64_IMPLEMENT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CTZ64_IMPLEMENT
uint32_t Curl_ctz64(uint64_t x)
{
  /* count trailing zeros in a uint64_t.
   * divide and conquer to find the number of lower 0 bits */
  ਅਚਲ uint64_t ml32 = 0xFFFFFFFF; /* lower 32 bits */
  ਅਚਲ uint64_t ml16 = 0x0000FFFF; /* lower 16 bits */
  ਅਚਲ uint64_t ml8  = 0x000000FF; /* lower 8 bits */
  ਅਚਲ uint64_t ml4  = 0x0000000F; /* lower 4 bits */
  ਅਚਲ uint64_t ml2  = 0x00000003; /* lower 2 bits */
  uint32_t n;

  ਜੇ(!x)
    ਵਾਪਸ 64;
  n = 1;
  ਜੇ(!(x & ml32)) {
    n = n + 32;
    x = x >> 32;
  }
  ਜੇ(!(x & ml16)) {
    n = n + 16;
    x = x >> 16;
  }
  ਜੇ(!(x & ml8)) {
    n = n + 8;
    x = x >> 8;
  }
  ਜੇ(!(x & ml4)) {
    n = n + 4;
    x = x >> 4;
  }
  ਜੇ(!(x & ml2)) {
    n = n + 2;
    x = x >> 2;
  }
  ਵਾਪਸ n - (uint32_t)(x & 1);
}
#ਜੇ_ਅੰਤ /* CURL_CTZ64_IMPLEMENT */
