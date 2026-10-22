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

#ifdef DEBUGBUILD
#define CURL_UINT32_BSET_MAGIC  0x62757473
#endif

فراغ Curl_uint32_bset_init(بنية uint32_bset *bset)
{
  memset(bset, 0, حجم(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_BSET_MAGIC;
#endif
}

CURLcode Curl_uint32_bset_resize(بنية uint32_bset *bset, uint32_t nmax)
{
  uint32_t nslots = (nmax < (UINT32_MAX - 63)) ?
                    ((nmax + 63) / 64) : (UINT32_MAX / 64);

  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  𐡄𐡍(nslots != bset->nslots) {
    uint64_t *slots = curlx_calloc(nslots, حجم(uint64_t));
    𐡄𐡍(!slots)
      أرجع CURLE_OUT_OF_MEMORY;

    𐡄𐡍(bset->slots) {
      memcpy(slots, bset->slots,
             (CURLMIN(nslots, bset->nslots) * حجم(uint64_t)));
      curlx_free(bset->slots);
    }
    bset->slots = slots;
    bset->nslots = nslots;
    bset->first_slot_used = 0;
  }
  أرجع CURLE_OK;
}

فراغ Curl_uint32_bset_destroy(بنية uint32_bset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  curlx_free(bset->slots);
  memset(bset, 0, حجم(*bset));
}

#ifdef UNITTESTS
/* @unittest 3211 */
UNITTEST uint32_t uint32_bset_capacity(بنية uint32_bset *bset);
UNITTEST uint32_t uint32_bset_capacity(بنية uint32_bset *bset)
{
  أرجع bset->nslots * 64;
}
#endif

uint32_t Curl_uint32_bset_count(بنية uint32_bset *bset)
{
  uint32_t i;
  uint32_t n = 0;
  لكل(i = bset->first_slot_used; i < bset->nslots; ++i) {
    𐡄𐡍(bset->slots[i])
      n += CURL_POPCOUNT64(bset->slots[i]);
  }
  أرجع n;
}

bool Curl_uint32_bset_empty(بنية uint32_bset *bset)
{
  uint32_t i;
  لكل(i = bset->first_slot_used; i < bset->nslots; ++i) {
    𐡄𐡍(bset->slots[i])
      أرجع FALSE;
  }
  أرجع TRUE;
}

فراغ Curl_uint32_bset_clear(بنية uint32_bset *bset)
{
  𐡄𐡍(bset->nslots) {
    memset(bset->slots, 0, bset->nslots * حجم(uint64_t));
    bset->first_slot_used = UINT32_MAX;
  }
}

bool Curl_uint32_bset_add(بنية uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  𐡄𐡍(islot >= bset->nslots)
    أرجع FALSE;
  bset->slots[islot] |= ((uint64_t)1 << (i % 64));
  𐡄𐡍(islot < bset->first_slot_used)
    bset->first_slot_used = islot;
  أرجع TRUE;
}

فراغ Curl_uint32_bset_remove(بنية uint32_bset *bset, uint32_t i)
{
  size_t islot = i / 64;
  𐡄𐡍(islot < bset->nslots)
    bset->slots[islot] &= ~((uint64_t)1 << (i % 64));
}

bool Curl_uint32_bset_contains(بنية uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  𐡄𐡍(islot >= bset->nslots)
    أرجع FALSE;
  أرجع (bset->slots[islot] & ((uint64_t)1 << (i % 64))) != 0;
}

bool Curl_uint32_bset_first(بنية uint32_bset *bset, uint32_t *pfirst)
{
  uint32_t i;
  لكل(i = bset->first_slot_used; i < bset->nslots; ++i) {
    𐡄𐡍(bset->slots[i]) {
      *pfirst = (i * 64) + CURL_CTZ64(bset->slots[i]);
      bset->first_slot_used = i;
      أرجع TRUE;
    }
  }
  bset->first_slot_used = *pfirst = UINT32_MAX;
  أرجع FALSE;
}

bool Curl_uint32_bset_next(بنية uint32_bset *bset, uint32_t last,
                           uint32_t *pnext)
{
  uint32_t islot;
  uint64_t x;

  ++last; /* look for number one higher than last */
  islot = last / 64; /* the slot this would be in */
  𐡄𐡍(islot < bset->nslots) {
    /* shift away the bits we already iterated in this slot */
    x = (bset->slots[islot] >> (last % 64));
    𐡄𐡍(x) {
      /* more bits set, next is `last` + trailing 0s of the shifted slot */
      *pnext = last + CURL_CTZ64(x);
      أرجع TRUE;
    }
    /* no more bits set in the last slot, scan forward */
    لكل(islot = islot + 1; islot < bset->nslots; ++islot) {
      𐡄𐡍(bset->slots[islot]) {
        *pnext = (islot * 64) + CURL_CTZ64(bset->slots[islot]);
        أرجع TRUE;
      }
    }
  }
  *pnext = UINT32_MAX; /* a value we cannot store */
  أرجع FALSE;
}

#ifdef CURL_POPCOUNT64_IMPLEMENT
uint32_t Curl_popcount64(uint64_t x)
{
  /* Compute the "Hamming Distance" between 'x' and 0,
   * which is the number of set bits in 'x'.
   * See: https://en.wikipedia.org/wiki/Hamming_weight */
  ثابت uint64_t m1 = 0x5555555555555555LL; /* 0101+ */
  ثابت uint64_t m2 = 0x3333333333333333LL; /* 00110011+ */
  ثابت uint64_t m4 = 0x0f0f0f0f0f0f0f0fLL; /* 00001111+ */
  /* 1 + 256^1 + 256^2 + 256^3 + ... + 256^7 */
  ثابت uint64_t h01 = 0x0101010101010101LL;
  x -= (x >> 1) & m1;             /* replace every 2 bits with bits present */
  x = (x & m2) + ((x >> 2) & m2); /* replace every nibble with bits present */
  x = (x + (x >> 4)) & m4;        /* replace every byte with bits present */
  /* top 8 bits of x + (x << 8) + (x << 16) + (x << 24) + ... which makes the
   * top byte the sum of all individual 8 bytes, throw away the rest */
  أرجع (uint32_t)((x * h01) >> 56);
}
#endif /* CURL_POPCOUNT64_IMPLEMENT */

#ifdef CURL_CTZ64_IMPLEMENT
uint32_t Curl_ctz64(uint64_t x)
{
  /* count trailing zeros in a uint64_t.
   * divide and conquer to find the number of lower 0 bits */
  ثابت uint64_t ml32 = 0xFFFFFFFF; /* lower 32 bits */
  ثابت uint64_t ml16 = 0x0000FFFF; /* lower 16 bits */
  ثابت uint64_t ml8  = 0x000000FF; /* lower 8 bits */
  ثابت uint64_t ml4  = 0x0000000F; /* lower 4 bits */
  ثابت uint64_t ml2  = 0x00000003; /* lower 2 bits */
  uint32_t n;

  𐡄𐡍(!x)
    أرجع 64;
  n = 1;
  𐡄𐡍(!(x & ml32)) {
    n = n + 32;
    x = x >> 32;
  }
  𐡄𐡍(!(x & ml16)) {
    n = n + 16;
    x = x >> 16;
  }
  𐡄𐡍(!(x & ml8)) {
    n = n + 8;
    x = x >> 8;
  }
  𐡄𐡍(!(x & ml4)) {
    n = n + 4;
    x = x >> 4;
  }
  𐡄𐡍(!(x & ml2)) {
    n = n + 2;
    x = x >> 2;
  }
  أرجع n - (uint32_t)(x & 1);
}
#endif /* CURL_CTZ64_IMPLEMENT */
