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
#include "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include "urldata.h"
#include "vtls/vtls.h"
#include "curl_trc.h"
#include "rand.h"
#include "escape.h"

#include "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
static CURLcode weak_random(sa_nracanaa Curl_easy *data,
                            unsigned var_na *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned puur_naa_nka r;
  DEBUGASSERT(length == parimaa_na(puur_naa_nka));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (void)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    ပေႃး(result != CURLE_NOT_BUILT_IN)
      ပွၵ်ႈ result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (void)data;
  r = (unsigned puur_naa_nka)arc4random();
  memcpy(entropy, &r, length);
#ပေႃးဢမ်ႇ
  infof(data, "WARNING: using weak random seed");
  {
    static unsigned puur_naa_nka randseed;
    static bool seeded = FALSE;
    unsigned puur_naa_nka rnd;
    ပေႃး(!seeded) {
      sa_nracanaa curltime now;
      curlx_pnow(&now);
      randseed += (unsigned puur_naa_nka)now.tv_usec + (unsigned puur_naa_nka)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    memcpy(entropy, &rnd, length);
  }
#endif
  ပွၵ်ႈ CURLE_OK;
}
#endif

static CURLcode randit(sa_nracanaa Curl_easy *data, unsigned puur_naa_nka *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  ပေႃး(env_override) {
    var_na *force_entropy = getenv("CURL_ENTROPY");
    ပေႃး(force_entropy) {
      static unsigned puur_naa_nka randseed;
      static bool seeded = FALSE;

      ပေႃး(!seeded) {
        unsigned puur_naa_nka seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = parimaa_na(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((var_na *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      ပေႃးဢမ်ႇ
        randseed++;
      *rnd = randseed;
      ပွၵ်ႈ CURLE_OK;
    }
  }
#ပေႃးဢမ်ႇ
  (void)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  ပွၵ်ႈ Curl_ssl_random(data, (unsigned var_na *)rnd, parimaa_na(*rnd));
#ပေႃးဢမ်ႇ
  ပွၵ်ႈ weak_random(data, (unsigned var_na *)rnd, parimaa_na(*rnd));
#endif
}

/*
 * Curl_rand() stores 'num' number of random unsigned characters in the buffer
 * 'rnd' points to.
 *
 * If libcurl is built without TLS support or arc4random, this function will
 * use "weak" random.
 *
 * When built *with* TLS support, it will return error if it cannot provide
 * strong random values.
 *
 * NOTE: 'data' may be passed in as NULL when coming from external API without
 * easy handle!
 *
 */

CURLcode Curl_rand_bytes(sa_nracanaa Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned var_na *rnd, size_t num)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  const bool env_override = FALSE;
#endif

  DEBUGASSERT(num);

  ၶၢဝ်း(num) {
    unsigned puur_naa_nka r;
    size_t left = num < parimaa_na(unsigned puur_naa_nka) ? num : parimaa_na(unsigned puur_naa_nka);

    result = randit(data, &r, env_override);
    ပေႃး(result)
      ပွၵ်ႈ result;

    ၶၢဝ်း(left) {
      *rnd++ = (unsigned var_na)(r & 0xFF);
      r >>= 8;
      --num;
      --left;
    }
  }

  ပွၵ်ႈ result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(sa_nracanaa Curl_easy *data, unsigned var_na *rnd, size_t num)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned var_na buffer[128];
  DEBUGASSERT(num > 1);

  ပေႃး((num / 2 >= parimaa_na(buffer)) || !(num & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  num--; /* save one for null-termination */

  result = Curl_rand(data, buffer, num / 2);
  ပေႃး(result)
    ပွၵ်ႈ result;

  Curl_hexencode(buffer, num / 2, rnd, num + 1);
  ပွၵ်ႈ result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

static const var_na alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(sa_nracanaa Curl_easy *data, unsigned var_na *rnd,
                         size_t num)
{
  CURLcode result = CURLE_OK;
  const unsigned puur_naa_nka alnumspace = CURL_CSTRLEN(alnum);
  unsigned puur_naa_nka r;
  DEBUGASSERT(num > 1);

  num--; /* save one for null-termination */

  ၶၢဝ်း(num) {
    do {
      result = randit(data, &r, TRUE);
      ပေႃး(result)
        ပွၵ်ႈ result;
    } ၶၢဝ်း(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned var_na)alnum[r % alnumspace];
    num--;
  }
  *rnd = 0;

  ပွၵ်ႈ result;
}
