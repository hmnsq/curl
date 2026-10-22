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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_UINT32_BSET_MAGIC  0x62757473
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_uint32_bset_init(ರಚನೆ uint32_bset *bset)
{
  memset(bset, 0, ಗಾತ್ರ(*bset));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  bset->init = CURL_UINT32_BSET_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
}

CURLcode Curl_uint32_bset_resize(ರಚನೆ uint32_bset *bset, uint32_t nmax)
{
  uint32_t nslots = (nmax < (UINT32_MAX - 63)) ?
                    ((nmax + 63) / 64) : (UINT32_MAX / 64);

  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  ಆದರೆ(nslots != bset->nslots) {
    uint64_t *slots = curlx_calloc(nslots, ಗಾತ್ರ(uint64_t));
    ಆದರೆ(!slots)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಆದರೆ(bset->slots) {
      ಸ್ಮರಣೆ_ನಕಲು(slots, bset->slots,
             (CURLMIN(nslots, bset->nslots) * ಗಾತ್ರ(uint64_t)));
      curlx_free(bset->slots);
    }
    bset->slots = slots;
    bset->nslots = nslots;
    bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ = 0;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_uint32_bset_destroy(ರಚನೆ uint32_bset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  curlx_free(bset->slots);
  memset(bset, 0, ಗಾತ್ರ(*bset));
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
/* @unittest 3211 */
UNITTEST uint32_t uint32_bset_ಸಾಮರ್ಥ್ಯ(ರಚನೆ uint32_bset *bset);
UNITTEST uint32_t uint32_bset_ಸಾಮರ್ಥ್ಯ(ರಚನೆ uint32_bset *bset)
{
  ಹಿಂತಿರುಗಿ bset->nslots * 64;
}
#ಆದರೆ_ಅಂತ್ಯ

uint32_t Curl_uint32_bset_ಎಣಿಕೆ(ರಚನೆ uint32_bset *bset)
{
  uint32_t i;
  uint32_t n = 0;
  ಗಾಗಿ(i = bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ; i < bset->nslots; ++i) {
    ಆದರೆ(bset->slots[i])
      n += CURL_POPCOUNT64(bset->slots[i]);
  }
  ಹಿಂತಿರುಗಿ n;
}

bool Curl_uint32_bset_ಖಾಲಿಗಳು(ರಚನೆ uint32_bset *bset)
{
  uint32_t i;
  ಗಾಗಿ(i = bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ; i < bset->nslots; ++i) {
    ಆದರೆ(bset->slots[i])
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಶೂನ್ಯ Curl_uint32_bset_clear(ರಚನೆ uint32_bset *bset)
{
  ಆದರೆ(bset->nslots) {
    memset(bset->slots, 0, bset->nslots * ಗಾತ್ರ(uint64_t));
    bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ = UINT32_MAX;
  }
}

bool Curl_uint32_bset_ಕೂಡಿಸು(ರಚನೆ uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  ಆದರೆ(islot >= bset->nslots)
    ಹಿಂತಿರುಗಿ FALSE;
  bset->slots[islot] |= ((uint64_t)1 << (i % 64));
  ಆದರೆ(islot < bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ)
    bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ = islot;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಶೂನ್ಯ Curl_uint32_bset_remove(ರಚನೆ uint32_bset *bset, uint32_t i)
{
  size_t islot = i / 64;
  ಆದರೆ(islot < bset->nslots)
    bset->slots[islot] &= ~((uint64_t)1 << (i % 64));
}

bool Curl_uint32_bset_ಒಳಗೊಂಡಿದೆ(ರಚನೆ uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  ಆದರೆ(islot >= bset->nslots)
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ (bset->slots[islot] & ((uint64_t)1 << (i % 64))) != 0;
}

bool Curl_uint32_bset_ಮೊದಲ(ರಚನೆ uint32_bset *bset, uint32_t *pfirst)
{
  uint32_t i;
  ಗಾಗಿ(i = bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ; i < bset->nslots; ++i) {
    ಆದರೆ(bset->slots[i]) {
      *pfirst = (i * 64) + CURL_CTZ64(bset->slots[i]);
      bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ = i;
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  bset->ಮೊದಲ_ಸ್ಥಾನ_ಬಳಸಿದ = *pfirst = UINT32_MAX;
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_uint32_bset_ಮುಂದಿನ(ರಚನೆ uint32_bset *bset, uint32_t ಕೊನೆಯ,
                           uint32_t *pnext)
{
  uint32_t islot;
  uint64_t x;

  ++ಕೊನೆಯ; /* look for number one higher than last */
  islot = ಕೊನೆಯ / 64; /* the slot this would be in */
  ಆದರೆ(islot < bset->nslots) {
    /* shift away the bits we already iterated in this slot */
    x = (bset->slots[islot] >> (ಕೊನೆಯ % 64));
    ಆದರೆ(x) {
      /* more bits set, next is `last` + trailing 0s of the shifted slot */
      *pnext = ಕೊನೆಯ + CURL_CTZ64(x);
      ಹಿಂತಿರುಗಿ TRUE;
    }
    /* no more bits set in the last slot, scan forward */
    ಗಾಗಿ(islot = islot + 1; islot < bset->nslots; ++islot) {
      ಆದರೆ(bset->slots[islot]) {
        *pnext = (islot * 64) + CURL_CTZ64(bset->slots[islot]);
        ಹಿಂತಿರುಗಿ TRUE;
      }
    }
  }
  *pnext = UINT32_MAX; /* a value we cannot store */
  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_POPCOUNT64_IMPLEMENT
uint32_t Curl_popcount64(uint64_t x)
{
  /* Compute the "Hamming Distance" between 'x' and 0,
   * which is the number of set bits in 'x'.
   * See: https://en.wikipedia.org/wiki/Hamming_weight */
  ಸ್ಥಿರಾಂಕ uint64_t m1 = 0x5555555555555555LL; /* 0101+ */
  ಸ್ಥಿರಾಂಕ uint64_t m2 = 0x3333333333333333LL; /* 00110011+ */
  ಸ್ಥಿರಾಂಕ uint64_t m4 = 0x0f0f0f0f0f0f0f0fLL; /* 00001111+ */
  /* 1 + 256^1 + 256^2 + 256^3 + ... + 256^7 */
  ಸ್ಥಿರಾಂಕ uint64_t h01 = 0x0101010101010101LL;
  x -= (x >> 1) & m1;             /* replace every 2 bits with bits present */
  x = (x & m2) + ((x >> 2) & m2); /* replace every nibble with bits present */
  x = (x + (x >> 4)) & m4;        /* replace every byte with bits present */
  /* top 8 bits of x + (x << 8) + (x << 16) + (x << 24) + ... which makes the
   * top byte the sum of all individual 8 bytes, throw away the rest */
  ಹಿಂತಿರುಗಿ (uint32_t)((x * h01) >> 56);
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_POPCOUNT64_IMPLEMENT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CTZ64_IMPLEMENT
uint32_t Curl_ctz64(uint64_t x)
{
  /* count trailing zeros in a uint64_t.
   * divide and conquer to find the number of lower 0 bits */
  ಸ್ಥಿರಾಂಕ uint64_t ml32 = 0xFFFFFFFF; /* lower 32 bits */
  ಸ್ಥಿರಾಂಕ uint64_t ml16 = 0x0000FFFF; /* lower 16 bits */
  ಸ್ಥಿರಾಂಕ uint64_t ml8  = 0x000000FF; /* lower 8 bits */
  ಸ್ಥಿರಾಂಕ uint64_t ml4  = 0x0000000F; /* lower 4 bits */
  ಸ್ಥಿರಾಂಕ uint64_t ml2  = 0x00000003; /* lower 2 bits */
  uint32_t n;

  ಆದರೆ(!x)
    ಹಿಂತಿರುಗಿ 64;
  n = 1;
  ಆದರೆ(!(x & ml32)) {
    n = n + 32;
    x = x >> 32;
  }
  ಆದರೆ(!(x & ml16)) {
    n = n + 16;
    x = x >> 16;
  }
  ಆದರೆ(!(x & ml8)) {
    n = n + 8;
    x = x >> 8;
  }
  ಆದರೆ(!(x & ml4)) {
    n = n + 4;
    x = x >> 4;
  }
  ಆದರೆ(!(x & ml2)) {
    n = n + 2;
    x = x >> 2;
  }
  ಹಿಂತಿರುಗಿ n - (uint32_t)(x & 1);
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_CTZ64_IMPLEMENT */
