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
#అన్తర్భావ "uint-spbset.h"

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు CURL_UINT32_SPBSET_MAGIC  0x70737362
#అయితే_ముగింపు

శూన్య Curl_uint32_spbset_init(నిర్మాణం uint32_spbset *bset)
{
  memset(bset, 0, పరిమాణం(*bset));
#నిర్వచితమైతే DEBUGBUILD
  bset->init = CURL_UINT32_SPBSET_MAGIC;
#అయితే_ముగింపు
}

/* Clear the bitset, making it empty.

   @unittest 3213
 */
UNITTEST శూన్య uint32_spbset_clear(నిర్మాణం uint32_spbset *bset);
UNITTEST శూన్య uint32_spbset_clear(నిర్మాణం uint32_spbset *bset)
{
  నిర్మాణం uint32_spbset_chunk *తదుపరి, *chunk;

  కొరకు(chunk = bset->తల.తదుపరి; chunk; chunk = తదుపరి) {
    తదుపరి = chunk->తదుపరి;
    curlx_free(chunk);
  }
  memset(&bset->తల, 0, పరిమాణం(bset->తల));
}

శూన్య Curl_uint32_spbset_destroy(నిర్మాణం uint32_spbset *bset)
{
  DEBUGASSERT(bset->init == CURL_UINT32_SPBSET_MAGIC);
  uint32_spbset_clear(bset);
}

uint32_t Curl_uint32_spbset_లెక్క(నిర్మాణం uint32_spbset *bset)
{
  నిర్మాణం uint32_spbset_chunk *chunk;
  uint32_t i, n = 0;

  కొరకు(chunk = &bset->తల; chunk; chunk = chunk->తదుపరి) {
    కొరకు(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ఒకవేళ(chunk->slots[i])
        n += CURL_POPCOUNT64(chunk->slots[i]);
    }
  }
  తిరిగి n;
}

స్థిర bool uint32_spbset_ఖాళీలు_chunk(నిర్మాణం uint32_spbset_chunk *chunk)
{
  uint32_t i;
  కొరకు(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ఒకవేళ(chunk->slots[i])
      తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిర నిర్మాణం uint32_spbset_chunk *uint32_spbset_unlink_ఖాళీలు(
  నిర్మాణం uint32_spbset *bset, uint32_t for_స్థానభ్రంశం)
{
  నిర్మాణం uint32_spbset_chunk *chunk, **panchor = ఖాళీ;
  కొరకు(chunk = &bset->తల; chunk;
      panchor = &chunk->తదుపరి, chunk = chunk->తదుపరి) {
    ఒకవేళ(uint32_spbset_ఖాళీలు_chunk(chunk))
      ఆపు;
  }
  ఒకవేళ(chunk) {
    ఒకవేళ(chunk == &bset->తల) { /* head chunk is empty */
      ఒకవేళ(!bset->తల.తదుపరి || (for_స్థానభ్రంశం < bset->తల.తదుపరి->స్థానభ్రంశం)) {
        తిరిగి &bset->తల;
      }
      /* swap head and next, unlink */
      chunk = bset->తల.తదుపరి;
      జ్ఞాపకం_నకలు(&bset->తల, chunk, పరిమాణం(bset->తల));
      memset(chunk, 0, పరిమాణం(*chunk));
    }
    లేకపోతే {
      *panchor = chunk->తదుపరి; /* unlink */
      memset(chunk, 0, పరిమాణం(*chunk));
    }
  }
  తిరిగి chunk;
}

స్థిర నిర్మాణం uint32_spbset_chunk *uint32_spbset_చొప్పించు_chunk(
  నిర్మాణం uint32_spbset *bset, నిర్మాణం uint32_spbset_chunk *nchunk)
{
  నిర్మాణం uint32_spbset_chunk *chunk, **panchor;

  /* insert nchunk into set's ordered chunk list */
  ఒకవేళ(nchunk->స్థానభ్రంశం < bset->తల.స్థానభ్రంశం) {
    /* swap chunk and head */
    uint32_t స్థానభ్రంశం = nchunk->స్థానభ్రంశం;
    జ్ఞాపకం_నకలు(nchunk, &bset->తల, పరిమాణం(*nchunk));
    memset(&bset->తల, 0, పరిమాణం(bset->తల));
    bset->తల.తదుపరి = nchunk;
    bset->తల.స్థానభ్రంశం = స్థానభ్రంశం;
    తిరిగి &bset->తల;
  }
  DEBUGASSERT(nchunk->స్థానభ్రంశం > bset->తల.స్థానభ్రంశం);
  panchor = &bset->తల.తదుపరి;
  కొరకు(chunk = *panchor; chunk;
      panchor = &chunk->తదుపరి, chunk = chunk->తదుపరి) {
    ఒకవేళ(chunk->స్థానభ్రంశం > nchunk->స్థానభ్రంశం) { /* insert before this chunk */
      nchunk->తదుపరి = chunk;
      *panchor = nchunk;
      తిరిగి nchunk;
    }
  }
  /* no chunk with larger offset, append */
  *panchor = nchunk;
  తిరిగి nchunk;
}

స్థిర నిర్మాణం uint32_spbset_chunk *uint32_spbset_get_chunk(
  నిర్మాణం uint32_spbset *bset, uint32_t i, bool grow)
{
  నిర్మాణం uint32_spbset_chunk *chunk;
  uint32_t i_స్థానభ్రంశం = (i & ~CURL_UINT32_SPBSET_CH_ముసుగు);

  ఒకవేళ(!bset)
    తిరిగి ఖాళీ;

  కొరకు(chunk = &bset->తల; chunk; chunk = chunk->తదుపరి) {
    ఒకవేళ(chunk->స్థానభ్రంశం == i_స్థానభ్రంశం)
      తిరిగి chunk;
    లేకపోతే ఒకవేళ(chunk->స్థానభ్రంశం > i_స్థానభ్రంశం)
      ఆపు; /* need new chunk here */
  }
  ఒకవేళ(!grow) /* just a check if the chunk exists */
    తిరిగి ఖాళీ;

  /* Is there an empty chunk to reuse? */
  chunk = uint32_spbset_unlink_ఖాళీలు(bset, i_స్థానభ్రంశం);
  ఒకవేళ(chunk) {
    chunk->స్థానభ్రంశం = i_స్థానభ్రంశం;
    ఒకవేళ(chunk == &bset->తల) /* head chunk is empty, stayed linked */
      తిరిగి &bset->తల;
    /* was really unlinked, need to insert below */
  }
  లేకపోతే {
    /* need a new one */
    chunk = curlx_calloc(1, పరిమాణం(*chunk));
    ఒకవేళ(!chunk)
      తిరిగి ఖాళీ;
    chunk->స్థానభ్రంశం = i_స్థానభ్రంశం;
  }

  తిరిగి uint32_spbset_చొప్పించు_chunk(bset, chunk);
}

bool Curl_uint32_spbset_కూడు(నిర్మాణం uint32_spbset *bset, uint32_t i)
{
  నిర్మాణం uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, TRUE);
  ఒకవేళ(!chunk)
    తిరిగి FALSE;

  DEBUGASSERT(i >= chunk->స్థానభ్రంశం);
  i -= chunk->స్థానభ్రంశం;
  DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
  chunk->slots[(i / 64)] |= ((uint64_t)1 << (i % 64));
  తిరిగి TRUE;
}

శూన్య Curl_uint32_spbset_remove(నిర్మాణం uint32_spbset *bset, uint32_t i)
{
  నిర్మాణం uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ఒకవేళ(chunk) {
    DEBUGASSERT(i >= chunk->స్థానభ్రంశం);
    i -= chunk->స్థానభ్రంశం;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    chunk->slots[(i / 64)] &= ~((uint64_t)1 << (i % 64));
  }
}

bool Curl_uint32_spbset_కలిగియుంది(నిర్మాణం uint32_spbset *bset, uint32_t i)
{
  నిర్మాణం uint32_spbset_chunk *chunk = uint32_spbset_get_chunk(bset, i, FALSE);
  ఒకవేళ(chunk) {
    DEBUGASSERT(i >= chunk->స్థానభ్రంశం);
    i -= chunk->స్థానభ్రంశం;
    DEBUGASSERT(i < (CURL_UINT32_SPBSET_CH_SLOTS * 64));
    తిరిగి (chunk->slots[i / 64] & ((uint64_t)1 << (i % 64))) != 0;
  }
  తిరిగి FALSE;
}

bool Curl_uint32_spbset_మొదటి(నిర్మాణం uint32_spbset *bset, uint32_t *pfirst)
{
  నిర్మాణం uint32_spbset_chunk *chunk;
  uint32_t i;

  కొరకు(chunk = &bset->తల; chunk; chunk = chunk->తదుపరి) {
    కొరకు(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
      ఒకవేళ(chunk->slots[i]) {
        *pfirst = chunk->స్థానభ్రంశం + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
        తిరిగి TRUE;
      }
    }
  }
  *pfirst = 0; /* give it a defined value even if it should not be used */
  తిరిగి FALSE;
}

స్థిర bool uint32_spbset_chunk_మొదటి(నిర్మాణం uint32_spbset_chunk *chunk,
                                      uint32_t *pfirst)
{
  uint32_t i;
  కొరకు(i = 0; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
    ఒకవేళ(chunk->slots[i]) {
      *pfirst = chunk->స్థానభ్రంశం + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
      తిరిగి TRUE;
    }
  }
  *pfirst = UINT32_MAX; /* a value we cannot store */
  తిరిగి FALSE;
}

స్థిర bool uint32_spbset_chunk_తదుపరి(నిర్మాణం uint32_spbset_chunk *chunk,
                                     uint32_t చివరి,
                                     uint32_t *pnext)
{
  ఒకవేళ(chunk->స్థానభ్రంశం <= చివరి) {
    uint64_t x;
    uint32_t i = ((చివరి - chunk->స్థానభ్రంశం) / 64);
    ఒకవేళ(i < CURL_UINT32_SPBSET_CH_SLOTS) {
      x = (chunk->slots[i] >> (చివరి % 64));
      ఒకవేళ(x) {
        /* more bits set, next is `last` + trailing 0s of the shifted slot */
        *pnext = చివరి + CURL_CTZ64(x);
        తిరిగి TRUE;
      }
      /* no more bits set in the last slot, scan forward */
      కొరకు(i = i + 1; i < CURL_UINT32_SPBSET_CH_SLOTS; ++i) {
        ఒకవేళ(chunk->slots[i]) {
          *pnext = chunk->స్థానభ్రంశం + ((i * 64) + CURL_CTZ64(chunk->slots[i]));
          తిరిగి TRUE;
        }
      }
    }
  }
  *pnext = UINT32_MAX;
  తిరిగి FALSE;
}

bool Curl_uint32_spbset_తదుపరి(నిర్మాణం uint32_spbset *bset, uint32_t చివరి,
                             uint32_t *pnext)
{
  నిర్మాణం uint32_spbset_chunk *chunk;
  uint32_t చివరి_స్థానభ్రంశం;

  ++చివరి; /* look for the next higher number */
  చివరి_స్థానభ్రంశం = (చివరి & ~CURL_UINT32_SPBSET_CH_ముసుగు);

  కొరకు(chunk = &bset->తల; chunk; chunk = chunk->తదుపరి) {
    ఒకవేళ(chunk->స్థానభ్రంశం >= చివరి_స్థానభ్రంశం) {
      ఆపు;
    }
  }

  ఒకవేళ(chunk && (chunk->స్థానభ్రంశం == చివరి_స్థానభ్రంశం)) {
    /* is there a number higher than last in this chunk? */
    ఒకవేళ(uint32_spbset_chunk_తదుపరి(chunk, చివరి, pnext))
      తిరిగి TRUE;
    /* not in this chunk */
    chunk = chunk->తదుపరి;
  }
  /* look for the first in the "higher" chunks, if there are any. */
  అయితే(chunk) {
    ఒకవేళ(uint32_spbset_chunk_మొదటి(chunk, pnext))
      తిరిగి TRUE;
    chunk = chunk->తదుపరి;
  }
  *pnext = UINT32_MAX;
  తిరిగి FALSE;
}
