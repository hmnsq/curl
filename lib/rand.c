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
static CURLcode weak_random(struct Curl_easy *data,
                            unsigned char *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned 정수 r;
  DEBUGASSERT(length == sizeof(정수));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (void)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    만약(result != CURLE_NOT_BUILT_IN)
      반환 result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (void)data;
  r = (unsigned 정수)arc4random();
  memcpy(entropy, &r, length);
#아니면
  infof(data, "WARNING: using weak random seed");
  {
    static unsigned 정수 randseed;
    static bool seeded = FALSE;
    unsigned 정수 rnd;
    만약(!seeded) {
      struct curltime now;
      curlx_pnow(&now);
      randseed += (unsigned 정수)now.tv_usec + (unsigned 정수)now.tv_sec;
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
  반환 CURLE_OK;
}
#endif

static CURLcode randit(struct Curl_easy *data, unsigned 정수 *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  만약(env_override) {
    char *force_entropy = getenv("CURL_ENTROPY");
    만약(force_entropy) {
      static unsigned 정수 randseed;
      static bool seeded = FALSE;

      만약(!seeded) {
        unsigned 정수 seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = sizeof(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((char *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      아니면
        randseed++;
      *rnd = randseed;
      반환 CURLE_OK;
    }
  }
#아니면
  (void)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  반환 Curl_ssl_random(data, (unsigned char *)rnd, sizeof(*rnd));
#아니면
  반환 weak_random(data, (unsigned char *)rnd, sizeof(*rnd));
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

CURLcode Curl_rand_bytes(struct Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned char *rnd, size_t num)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  const bool env_override = FALSE;
#endif

  DEBUGASSERT(num);

  동안(num) {
    unsigned 정수 r;
    size_t left = num < sizeof(unsigned 정수) ? num : sizeof(unsigned 정수);

    result = randit(data, &r, env_override);
    만약(result)
      반환 result;

    동안(left) {
      *rnd++ = (unsigned char)(r & 0xFF);
      r >>= 8;
      --num;
      --left;
    }
  }

  반환 result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(struct Curl_easy *data, unsigned char *rnd, size_t num)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned char buffer[128];
  DEBUGASSERT(num > 1);

  만약((num / 2 >= sizeof(buffer)) || !(num & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    반환 CURLE_BAD_FUNCTION_ARGUMENT;
  }

  num--; /* save one for null-termination */

  result = Curl_rand(data, buffer, num / 2);
  만약(result)
    반환 result;

  Curl_hexencode(buffer, num / 2, rnd, num + 1);
  반환 result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

static const char alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(struct Curl_easy *data, unsigned char *rnd,
                         size_t num)
{
  CURLcode result = CURLE_OK;
  const unsigned 정수 alnumspace = CURL_CSTRLEN(alnum);
  unsigned 정수 r;
  DEBUGASSERT(num > 1);

  num--; /* save one for null-termination */

  동안(num) {
    do {
      result = randit(data, &r, TRUE);
      만약(result)
        반환 result;
    } 동안(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned char)alnum[r % alnumspace];
    num--;
  }
  *rnd = 0;

  반환 result;
}
