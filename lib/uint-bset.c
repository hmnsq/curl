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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "uint-bset.h"

#ifdef DEBUGBUILD
#define CURL_UINT32_BSET_MAGIC  0x62757473
#endif

வெறுமை Curl_uint32_bset_init(கட்டமைப்பு uint32_bset *bset)
{
  memset(bset, 0, அளவு(*bset));
#ifdef DEBUGBUILD
  bset->init = CURL_UINT32_BSET_MAGIC;
#endif
}

CURLcode Curl_uint32_bset_resize(கட்டமைப்பு uint32_bset *bset, uint32_t nmax)
{
  uint32_t nslots = (nmax < (UINT32_MAX - 63)) ?
                    ((nmax + 63) / 64) : (UINT32_MAX / 64);

  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  எனில்(nslots != bset->nslots) {
    uint64_t *slots = curlx_calloc(nslots, அளவு(uint64_t));
    எனில்(!slots)
      திரும்பு CURLE_OUT_OF_MEMORY;

    எனில்(bset->slots) {
      memcpy(slots, bset->slots,
             (CURLMIN(nslots, bset->nslots) * அளவு(uint64_t)));
      curlx_free(bset->slots);
    }
    bset->slots = slots;
    bset->nslots = nslots;
    bset->first_slot_used = 0;
  }
  திரும்பு CURLE_OK;
}

வெறுமை Curl_uint32_bset_destroy(கட்டமைப்பு uint32_bset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  curlx_free(bset->slots);
  memset(bset, 0, அளவு(*bset));
}

#ifdef UNITTESTS
/* @unittest 3211 */
UNITTEST uint32_t uint32_bset_capacity(கட்டமைப்பு uint32_bset *bset);
UNITTEST uint32_t uint32_bset_capacity(கட்டமைப்பு uint32_bset *bset)
{
  திரும்பு bset->nslots * 64;
}
#endif

uint32_t Curl_uint32_bset_மொத்தம்(கட்டமைப்பு uint32_bset *bset)
{
  uint32_t i;
  uint32_t n = 0;
  க்கு(i = bset->first_slot_used; i < bset->nslots; ++i) {
    எனில்(bset->slots[i])
      n += CURL_POPCOUNT64(bset->slots[i]);
  }
  திரும்பு n;
}

bool Curl_uint32_bset_empty(கட்டமைப்பு uint32_bset *bset)
{
  uint32_t i;
  க்கு(i = bset->first_slot_used; i < bset->nslots; ++i) {
    எனில்(bset->slots[i])
      திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

வெறுமை Curl_uint32_bset_clear(கட்டமைப்பு uint32_bset *bset)
{
  எனில்(bset->nslots) {
    memset(bset->slots, 0, bset->nslots * அளவு(uint64_t));
    bset->first_slot_used = UINT32_MAX;
  }
}

bool Curl_uint32_bset_add(கட்டமைப்பு uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  எனில்(islot >= bset->nslots)
    திரும்பு FALSE;
  bset->slots[islot] |= ((uint64_t)1 << (i % 64));
  எனில்(islot < bset->first_slot_used)
    bset->first_slot_used = islot;
  திரும்பு TRUE;
}

வெறுமை Curl_uint32_bset_remove(கட்டமைப்பு uint32_bset *bset, uint32_t i)
{
  size_t islot = i / 64;
  எனில்(islot < bset->nslots)
    bset->slots[islot] &= ~((uint64_t)1 << (i % 64));
}

bool Curl_uint32_bset_contains(கட்டமைப்பு uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  எனில்(islot >= bset->nslots)
    திரும்பு FALSE;
  திரும்பு (bset->slots[islot] & ((uint64_t)1 << (i % 64))) != 0;
}

bool Curl_uint32_bset_first(கட்டமைப்பு uint32_bset *bset, uint32_t *pfirst)
{
  uint32_t i;
  க்கு(i = bset->first_slot_used; i < bset->nslots; ++i) {
    எனில்(bset->slots[i]) {
      *pfirst = (i * 64) + CURL_CTZ64(bset->slots[i]);
      bset->first_slot_used = i;
      திரும்பு TRUE;
    }
  }
  bset->first_slot_used = *pfirst = UINT32_MAX;
  திரும்பு FALSE;
}

bool Curl_uint32_bset_next(கட்டமைப்பு uint32_bset *bset, uint32_t last,
                           uint32_t *pnext)
{
  uint32_t islot;
  uint64_t x;

  ++last; /* look for number one higher than last */
  islot = last / 64; /* the slot this would be in */
  எனில்(islot < bset->nslots) {
    /* shift away the bits we already iterated in this slot */
    x = (bset->slots[islot] >> (last % 64));
    எனில்(x) {
      /* more bits set, next is `last` + trailing 0s of the shifted slot */
      *pnext = last + CURL_CTZ64(x);
      திரும்பு TRUE;
    }
    /* no more bits set in the last slot, scan forward */
    க்கு(islot = islot + 1; islot < bset->nslots; ++islot) {
      எனில்(bset->slots[islot]) {
        *pnext = (islot * 64) + CURL_CTZ64(bset->slots[islot]);
        திரும்பு TRUE;
      }
    }
  }
  *pnext = UINT32_MAX; /* a value we cannot store */
  திரும்பு FALSE;
}

#ifdef CURL_POPCOUNT64_IMPLEMENT
uint32_t Curl_popcount64(uint64_t x)
{
  /* Compute the "Hamming Distance" between 'x' and 0,
   * which is the number of set bits in 'x'.
   * See: https://en.wikipedia.org/wiki/Hamming_weight */
  மாறிலி uint64_t m1 = 0x5555555555555555LL; /* 0101+ */
  மாறிலி uint64_t m2 = 0x3333333333333333LL; /* 00110011+ */
  மாறிலி uint64_t m4 = 0x0f0f0f0f0f0f0f0fLL; /* 00001111+ */
  /* 1 + 256^1 + 256^2 + 256^3 + ... + 256^7 */
  மாறிலி uint64_t h01 = 0x0101010101010101LL;
  x -= (x >> 1) & m1;             /* replace every 2 bits with bits present */
  x = (x & m2) + ((x >> 2) & m2); /* replace every nibble with bits present */
  x = (x + (x >> 4)) & m4;        /* replace every byte with bits present */
  /* top 8 bits of x + (x << 8) + (x << 16) + (x << 24) + ... which makes the
   * top byte the sum of all individual 8 bytes, throw away the rest */
  திரும்பு (uint32_t)((x * h01) >> 56);
}
#endif /* CURL_POPCOUNT64_IMPLEMENT */

#ifdef CURL_CTZ64_IMPLEMENT
uint32_t Curl_ctz64(uint64_t x)
{
  /* count trailing zeros in a uint64_t.
   * divide and conquer to find the number of lower 0 bits */
  மாறிலி uint64_t ml32 = 0xFFFFFFFF; /* lower 32 bits */
  மாறிலி uint64_t ml16 = 0x0000FFFF; /* lower 16 bits */
  மாறிலி uint64_t ml8  = 0x000000FF; /* lower 8 bits */
  மாறிலி uint64_t ml4  = 0x0000000F; /* lower 4 bits */
  மாறிலி uint64_t ml2  = 0x00000003; /* lower 2 bits */
  uint32_t n;

  எனில்(!x)
    திரும்பு 64;
  n = 1;
  எனில்(!(x & ml32)) {
    n = n + 32;
    x = x >> 32;
  }
  எனில்(!(x & ml16)) {
    n = n + 16;
    x = x >> 16;
  }
  எனில்(!(x & ml8)) {
    n = n + 8;
    x = x >> 8;
  }
  எனில்(!(x & ml4)) {
    n = n + 4;
    x = x >> 4;
  }
  எனில்(!(x & ml2)) {
    n = n + 2;
    x = x >> 2;
  }
  திரும்பு n - (uint32_t)(x & 1);
}
#endif /* CURL_CTZ64_IMPLEMENT */
