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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "uint-bset.h"

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు CURL_UINT32_BSET_MAGIC  0x62757473
#అయితే_ముగింపు

శూన్య Curl_uint32_bset_init(నిర్మాణం uint32_bset *bset)
{
  memset(bset, 0, పరిమాణం(*bset));
#నిర్వచితమైతే DEBUGBUILD
  bset->init = CURL_UINT32_BSET_MAGIC;
#అయితే_ముగింపు
}

CURLcode Curl_uint32_bset_resize(నిర్మాణం uint32_bset *bset, uint32_t nmax)
{
  uint32_t nslots = (nmax < (UINT32_MAX - 63)) ?
                    ((nmax + 63) / 64) : (UINT32_MAX / 64);

  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  ఒకవేళ(nslots != bset->nslots) {
    uint64_t *slots = curlx_calloc(nslots, పరిమాణం(uint64_t));
    ఒకవేళ(!slots)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    ఒకవేళ(bset->slots) {
      జ్ఞాపకం_నకలు(slots, bset->slots,
             (CURLMIN(nslots, bset->nslots) * పరిమాణం(uint64_t)));
      curlx_free(bset->slots);
    }
    bset->slots = slots;
    bset->nslots = nslots;
    bset->మొదటి_స్థానం_వాడిన = 0;
  }
  తిరిగి CURLE_OK;
}

శూన్య Curl_uint32_bset_destroy(నిర్మాణం uint32_bset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_BSET_MAGIC);
  curlx_free(bset->slots);
  memset(bset, 0, పరిమాణం(*bset));
}

#నిర్వచితమైతే UNITTESTS
/* @unittest 3211 */
UNITTEST uint32_t uint32_bset_సామర్థ్యం(నిర్మాణం uint32_bset *bset);
UNITTEST uint32_t uint32_bset_సామర్థ్యం(నిర్మాణం uint32_bset *bset)
{
  తిరిగి bset->nslots * 64;
}
#అయితే_ముగింపు

uint32_t Curl_uint32_bset_లెక్క(నిర్మాణం uint32_bset *bset)
{
  uint32_t i;
  uint32_t n = 0;
  కొరకు(i = bset->మొదటి_స్థానం_వాడిన; i < bset->nslots; ++i) {
    ఒకవేళ(bset->slots[i])
      n += CURL_POPCOUNT64(bset->slots[i]);
  }
  తిరిగి n;
}

bool Curl_uint32_bset_ఖాళీలు(నిర్మాణం uint32_bset *bset)
{
  uint32_t i;
  కొరకు(i = bset->మొదటి_స్థానం_వాడిన; i < bset->nslots; ++i) {
    ఒకవేళ(bset->slots[i])
      తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

శూన్య Curl_uint32_bset_clear(నిర్మాణం uint32_bset *bset)
{
  ఒకవేళ(bset->nslots) {
    memset(bset->slots, 0, bset->nslots * పరిమాణం(uint64_t));
    bset->మొదటి_స్థానం_వాడిన = UINT32_MAX;
  }
}

bool Curl_uint32_bset_కూడు(నిర్మాణం uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  ఒకవేళ(islot >= bset->nslots)
    తిరిగి FALSE;
  bset->slots[islot] |= ((uint64_t)1 << (i % 64));
  ఒకవేళ(islot < bset->మొదటి_స్థానం_వాడిన)
    bset->మొదటి_స్థానం_వాడిన = islot;
  తిరిగి TRUE;
}

శూన్య Curl_uint32_bset_remove(నిర్మాణం uint32_bset *bset, uint32_t i)
{
  size_t islot = i / 64;
  ఒకవేళ(islot < bset->nslots)
    bset->slots[islot] &= ~((uint64_t)1 << (i % 64));
}

bool Curl_uint32_bset_కలిగియుంది(నిర్మాణం uint32_bset *bset, uint32_t i)
{
  uint32_t islot = i / 64;
  ఒకవేళ(islot >= bset->nslots)
    తిరిగి FALSE;
  తిరిగి (bset->slots[islot] & ((uint64_t)1 << (i % 64))) != 0;
}

bool Curl_uint32_bset_మొదటి(నిర్మాణం uint32_bset *bset, uint32_t *pfirst)
{
  uint32_t i;
  కొరకు(i = bset->మొదటి_స్థానం_వాడిన; i < bset->nslots; ++i) {
    ఒకవేళ(bset->slots[i]) {
      *pfirst = (i * 64) + CURL_CTZ64(bset->slots[i]);
      bset->మొదటి_స్థానం_వాడిన = i;
      తిరిగి TRUE;
    }
  }
  bset->మొదటి_స్థానం_వాడిన = *pfirst = UINT32_MAX;
  తిరిగి FALSE;
}

bool Curl_uint32_bset_తదుపరి(నిర్మాణం uint32_bset *bset, uint32_t చివరి,
                           uint32_t *pnext)
{
  uint32_t islot;
  uint64_t x;

  ++చివరి; /* look for number one higher than last */
  islot = చివరి / 64; /* the slot this would be in */
  ఒకవేళ(islot < bset->nslots) {
    /* shift away the bits we already iterated in this slot */
    x = (bset->slots[islot] >> (చివరి % 64));
    ఒకవేళ(x) {
      /* more bits set, next is `last` + trailing 0s of the shifted slot */
      *pnext = చివరి + CURL_CTZ64(x);
      తిరిగి TRUE;
    }
    /* no more bits set in the last slot, scan forward */
    కొరకు(islot = islot + 1; islot < bset->nslots; ++islot) {
      ఒకవేళ(bset->slots[islot]) {
        *pnext = (islot * 64) + CURL_CTZ64(bset->slots[islot]);
        తిరిగి TRUE;
      }
    }
  }
  *pnext = UINT32_MAX; /* a value we cannot store */
  తిరిగి FALSE;
}

#నిర్వచితమైతే CURL_POPCOUNT64_IMPLEMENT
uint32_t Curl_popcount64(uint64_t x)
{
  /* Compute the "Hamming Distance" between 'x' and 0,
   * which is the number of set bits in 'x'.
   * See: https://en.wikipedia.org/wiki/Hamming_weight */
  స్థిరరాశి uint64_t m1 = 0x5555555555555555LL; /* 0101+ */
  స్థిరరాశి uint64_t m2 = 0x3333333333333333LL; /* 00110011+ */
  స్థిరరాశి uint64_t m4 = 0x0f0f0f0f0f0f0f0fLL; /* 00001111+ */
  /* 1 + 256^1 + 256^2 + 256^3 + ... + 256^7 */
  స్థిరరాశి uint64_t h01 = 0x0101010101010101LL;
  x -= (x >> 1) & m1;             /* replace every 2 bits with bits present */
  x = (x & m2) + ((x >> 2) & m2); /* replace every nibble with bits present */
  x = (x + (x >> 4)) & m4;        /* replace every byte with bits present */
  /* top 8 bits of x + (x << 8) + (x << 16) + (x << 24) + ... which makes the
   * top byte the sum of all individual 8 bytes, throw away the rest */
  తిరిగి (uint32_t)((x * h01) >> 56);
}
#అయితే_ముగింపు /* CURL_POPCOUNT64_IMPLEMENT */

#నిర్వచితమైతే CURL_CTZ64_IMPLEMENT
uint32_t Curl_ctz64(uint64_t x)
{
  /* count trailing zeros in a uint64_t.
   * divide and conquer to find the number of lower 0 bits */
  స్థిరరాశి uint64_t ml32 = 0xFFFFFFFF; /* lower 32 bits */
  స్థిరరాశి uint64_t ml16 = 0x0000FFFF; /* lower 16 bits */
  స్థిరరాశి uint64_t ml8  = 0x000000FF; /* lower 8 bits */
  స్థిరరాశి uint64_t ml4  = 0x0000000F; /* lower 4 bits */
  స్థిరరాశి uint64_t ml2  = 0x00000003; /* lower 2 bits */
  uint32_t n;

  ఒకవేళ(!x)
    తిరిగి 64;
  n = 1;
  ఒకవేళ(!(x & ml32)) {
    n = n + 32;
    x = x >> 32;
  }
  ఒకవేళ(!(x & ml16)) {
    n = n + 16;
    x = x >> 16;
  }
  ఒకవేళ(!(x & ml8)) {
    n = n + 8;
    x = x >> 8;
  }
  ఒకవేళ(!(x & ml4)) {
    n = n + 4;
    x = x >> 4;
  }
  ఒకవేళ(!(x & ml2)) {
    n = n + 2;
    x = x >> 2;
  }
  తిరిగి n - (uint32_t)(x & 1);
}
#అయితే_ముగింపు /* CURL_CTZ64_IMPLEMENT */
