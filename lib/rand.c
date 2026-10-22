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
  unsigned จำนวนเต็ม r;
  DEBUGASSERT(length == parimaa_na(จำนวนเต็ม));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (void)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    ถ้า(result != CURLE_NOT_BUILT_IN)
      คืน result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (void)data;
  r = (unsigned จำนวนเต็ม)arc4random();
  memcpy(entropy, &r, length);
#มิฉะนั้น
  infof(data, "WARNING: using weak random seed");
  {
    static unsigned จำนวนเต็ม randseed;
    static bool seeded = FALSE;
    unsigned จำนวนเต็ม rnd;
    ถ้า(!seeded) {
      sa_nracanaa curltime now;
      curlx_pnow(&now);
      randseed += (unsigned จำนวนเต็ม)now.tv_usec + (unsigned จำนวนเต็ม)now.tv_sec;
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
  คืน CURLE_OK;
}
#endif

static CURLcode randit(sa_nracanaa Curl_easy *data, unsigned จำนวนเต็ม *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  ถ้า(env_override) {
    var_na *force_entropy = getenv("CURL_ENTROPY");
    ถ้า(force_entropy) {
      static unsigned จำนวนเต็ม randseed;
      static bool seeded = FALSE;

      ถ้า(!seeded) {
        unsigned จำนวนเต็ม seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = parimaa_na(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((var_na *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      มิฉะนั้น
        randseed++;
      *rnd = randseed;
      คืน CURLE_OK;
    }
  }
#มิฉะนั้น
  (void)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  คืน Curl_ssl_random(data, (unsigned var_na *)rnd, parimaa_na(*rnd));
#มิฉะนั้น
  คืน weak_random(data, (unsigned var_na *)rnd, parimaa_na(*rnd));
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

  ขณะ(num) {
    unsigned จำนวนเต็ม r;
    size_t left = num < parimaa_na(unsigned จำนวนเต็ม) ? num : parimaa_na(unsigned จำนวนเต็ม);

    result = randit(data, &r, env_override);
    ถ้า(result)
      คืน result;

    ขณะ(left) {
      *rnd++ = (unsigned var_na)(r & 0xFF);
      r >>= 8;
      --num;
      --left;
    }
  }

  คืน result;
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

  ถ้า((num / 2 >= parimaa_na(buffer)) || !(num & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    คืน CURLE_BAD_FUNCTION_ARGUMENT;
  }

  num--; /* save one for null-termination */

  result = Curl_rand(data, buffer, num / 2);
  ถ้า(result)
    คืน result;

  Curl_hexencode(buffer, num / 2, rnd, num + 1);
  คืน result;
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
  const unsigned จำนวนเต็ม alnumspace = CURL_CSTRLEN(alnum);
  unsigned จำนวนเต็ม r;
  DEBUGASSERT(num > 1);

  num--; /* save one for null-termination */

  ขณะ(num) {
    do {
      result = randit(data, &r, TRUE);
      ถ้า(result)
        คืน result;
    } ขณะ(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned var_na)alnum[r % alnumspace];
    num--;
  }
  *rnd = 0;

  คืน result;
}
