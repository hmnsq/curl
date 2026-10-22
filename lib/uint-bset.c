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
#समावेश "curl_setup.h"

#समावेश "uint-bset.h"

#अगर_घोषित DEBUGBUILD
#घोषणा CURL_UINT32_BSET_MAGIC  0x62757473
#पूर्ण_अगर

व्योम Curl_uint32_bset_init(काष्ठा uint32_bset *bset)
{
  स_रखो(bset, 0, माप(*bset));
#अगर_घोषित DEBUGBUILD
  bset->init = CURL_UINT32_BSET_MAGIC;
#पूर्ण_अगर
}

CURLcode Curl_uint32_bset_resize(काष्ठा uint32_bset *bset, uint32_t nmax)
{
  uint32_t nslots = (nmax < (UINT32_MAX - 63)) ?
                    ((nmax + 63) / 64) : (UINT32_MAX / 64);

  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  अगर(nslots != bset->nslots) {
    uint64_t *slots = curlx_calloc(nslots, माप(uint64_t));
    अगर(!slots)
      वापस CURLE_OUT_OF_MEMORY;

    अगर(bset->slots) {
      स_नकल(slots, bset->slots,
             (CURLMIN(nslots, bset->nslots) * माप(uint64_t)));
      curlx_free(bset->slots);
    }
    bset->slots = slots;
    bset->nslots = nslots;
    bset->first_slot_used = 0;
  }
  वापस CURLE_OK;
}

व्योम Curl_uint32_bset_destroy(काष्ठा uint32_bset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  curlx_free(bset->slots);
  स_रखो(bset, 0, माप(*bset));
}

#अगर_घोषित UNITTESTS
/* @unittest 3211 */
UNITTEST uint32_t uint32_bset_capacity(काष्ठा uint32_bset *bset);
UNITTEST uint32_t uint32_bset_capacity(काष्ठा uint32_bset *bset)
{
  वापस bset->nslots * 64;
}
#पूर्ण_अगर

uint32_t Curl_uint32_bset_गिनती(काष्ठा uint32_bset *bset)
{
  uint32_t i;
  uint32_t n = 0;
  क्रम(i = bset->first_slot_used; i < bset->nslots; ++i) {
    अगर(bset->slots[i])
      n += CURL_POPCOUNT64(bset->slots[i]);
  }
  वापस n;
}

bool Curl_uint32_bset_empty(काष्ठा uint32_bset *bset)
{
  uint32_t i;
  क्रम(i = bset->first_slot_used; i < bset->nslots; ++i) {
    अगर(bset->slots[i])
      वापस FALSE;
  }
  वापस TRUE;
}

व्योम Curl_uint32_bset_clear(काष्ठा uint32_bset *bset)
{
  अगर(bset->nslots) {
    स_रखो(bset->slots, 0, bset->nslots * माप(uint64_t));
    bset->first_slot_used = UINT32_MAX;
  }
}

bool Curl_uint32_bset_add(काष्ठा uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  अगर(islot >= bset->nslots)
    वापस FALSE;
  bset->slots[islot] |= ((uint64_t)1 << (i % 64));
  अगर(islot < bset->first_slot_used)
    bset->first_slot_used = islot;
  वापस TRUE;
}

व्योम Curl_uint32_bset_remove(काष्ठा uint32_bset *bset, uint32_t i)
{
  माप_प्रकार islot = i / 64;
  अगर(islot < bset->nslots)
    bset->slots[islot] &= ~((uint64_t)1 << (i % 64));
}

bool Curl_uint32_bset_contains(काष्ठा uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  अगर(islot >= bset->nslots)
    वापस FALSE;
  वापस (bset->slots[islot] & ((uint64_t)1 << (i % 64))) != 0;
}

bool Curl_uint32_bset_first(काष्ठा uint32_bset *bset, uint32_t *pfirst)
{
  uint32_t i;
  क्रम(i = bset->first_slot_used; i < bset->nslots; ++i) {
    अगर(bset->slots[i]) {
      *pfirst = (i * 64) + CURL_CTZ64(bset->slots[i]);
      bset->first_slot_used = i;
      वापस TRUE;
    }
  }
  bset->first_slot_used = *pfirst = UINT32_MAX;
  वापस FALSE;
}

bool Curl_uint32_bset_अग्र(काष्ठा uint32_bset *bset, uint32_t last,
                           uint32_t *pnext)
{
  uint32_t islot;
  uint64_t x;

  ++last; /* look for number one higher than last */
  islot = last / 64; /* the slot this would be in */
  अगर(islot < bset->nslots) {
    /* shift away the bits we already iterated in this slot */
    x = (bset->slots[islot] >> (last % 64));
    अगर(x) {
      /* more bits set, next is `last` + trailing 0s of the shifted slot */
      *pnext = last + CURL_CTZ64(x);
      वापस TRUE;
    }
    /* no more bits set in the last slot, scan forward */
    क्रम(islot = islot + 1; islot < bset->nslots; ++islot) {
      अगर(bset->slots[islot]) {
        *pnext = (islot * 64) + CURL_CTZ64(bset->slots[islot]);
        वापस TRUE;
      }
    }
  }
  *pnext = UINT32_MAX; /* a value we cannot store */
  वापस FALSE;
}

#अगर_घोषित CURL_POPCOUNT64_IMPLEMENT
uint32_t Curl_popcount64(uint64_t x)
{
  /* Compute the "Hamming Distance" between 'x' and 0,
   * which is the number of set bits in 'x'.
   * See: https://en.wikipedia.org/wiki/Hamming_weight */
  स्थिर uint64_t m1 = 0x5555555555555555LL; /* 0101+ */
  स्थिर uint64_t m2 = 0x3333333333333333LL; /* 00110011+ */
  स्थिर uint64_t m4 = 0x0f0f0f0f0f0f0f0fLL; /* 00001111+ */
  /* 1 + 256^1 + 256^2 + 256^3 + ... + 256^7 */
  स्थिर uint64_t h01 = 0x0101010101010101LL;
  x -= (x >> 1) & m1;             /* replace every 2 bits with bits present */
  x = (x & m2) + ((x >> 2) & m2); /* replace every nibble with bits present */
  x = (x + (x >> 4)) & m4;        /* replace every byte with bits present */
  /* top 8 bits of x + (x << 8) + (x << 16) + (x << 24) + ... which makes the
   * top byte the sum of all individual 8 bytes, throw away the rest */
  वापस (uint32_t)((x * h01) >> 56);
}
#पूर्ण_अगर /* CURL_POPCOUNT64_IMPLEMENT */

#अगर_घोषित CURL_CTZ64_IMPLEMENT
uint32_t Curl_ctz64(uint64_t x)
{
  /* count trailing zeros in a uint64_t.
   * divide and conquer to find the number of lower 0 bits */
  स्थिर uint64_t ml32 = 0xFFFFFFFF; /* lower 32 bits */
  स्थिर uint64_t ml16 = 0x0000FFFF; /* lower 16 bits */
  स्थिर uint64_t ml8  = 0x000000FF; /* lower 8 bits */
  स्थिर uint64_t ml4  = 0x0000000F; /* lower 4 bits */
  स्थिर uint64_t ml2  = 0x00000003; /* lower 2 bits */
  uint32_t n;

  अगर(!x)
    वापस 64;
  n = 1;
  अगर(!(x & ml32)) {
    n = n + 32;
    x = x >> 32;
  }
  अगर(!(x & ml16)) {
    n = n + 16;
    x = x >> 16;
  }
  अगर(!(x & ml8)) {
    n = n + 8;
    x = x >> 8;
  }
  अगर(!(x & ml4)) {
    n = n + 4;
    x = x >> 4;
  }
  अगर(!(x & ml2)) {
    n = n + 2;
    x = x >> 2;
  }
  वापस n - (uint32_t)(x & 1);
}
#पूर्ण_अगर /* CURL_CTZ64_IMPLEMENT */
