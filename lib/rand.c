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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/winapi.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ਸਥਿਰ CURLcode weak_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy,
                            size_t ਲੰਮਾਈ) /* always 4, size of int */
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ r;
  DEBUGASSERT(ਲੰਮਾਈ == ਆਕਾਰ(ਪੂਰਨ));

  /* Trying cryptographically secure functions first */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  (ਖਾਲੀ)ਅੰਕੜੇ;
  {
    CURLcode ਨਤੀਜਾ = curlx_win32_random(entropy, ਲੰਮਾਈ);
    ਜੇ(ਨਤੀਜਾ != CURLE_NOT_BUILT_ਵਿੱਚ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARC4RANDOM
  (ਖਾਲੀ)ਅੰਕੜੇ;
  r = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)arc4random();
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(entropy, &r, ਲੰਮਾਈ);
#ਨਹੀਂਤਾਂ
  infof(ਅੰਕੜੇ, "WARNING: using weak random seed");
  {
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ randseed;
    ਸਥਿਰ bool seeded = FALSE;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ rnd;
    ਜੇ(!seeded) {
      ਬਣਤਰ curltime now;
      curlx_pnow(&now);
      randseed += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)now.tv_usec + (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(entropy, &rnd, ਲੰਮਾਈ);
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode randit(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *rnd,
                       bool env_override)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(env_override) {
    ਅੱਖਰ *force_entropy = getenv("CURL_ENTROPY");
    ਜੇ(force_entropy) {
      ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ randseed;
      ਸਥਿਰ bool seeded = FALSE;

      ਜੇ(!seeded) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਬੀਜ = 0;
        size_t elen = ਲੜੀ_ਲੰਬਾਈ(force_entropy);
        size_t clen = ਆਕਾਰ(ਬੀਜ);
        size_t min = elen < clen ? elen : clen;
        ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)&ਬੀਜ, force_entropy, min);
        randseed = ntohl(ਬੀਜ);
        seeded = TRUE;
      }
      ਨਹੀਂਤਾਂ
        randseed++;
      *rnd = randseed;
      ਵਾਪਸ CURLE_OK;
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)env_override;
#ਜੇ_ਅੰਤ

  /* data may be NULL! */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਵਾਪਸ Curl_ssl_random(ਅੰਕੜੇ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)rnd, ਆਕਾਰ(*rnd));
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ weak_random(ਅੰਕੜੇ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)rnd, ਆਕਾਰ(*rnd));
#ਜੇ_ਅੰਤ
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

CURLcode Curl_rand_ਬਾਈਟਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
                         bool env_override,
#ਜੇ_ਅੰਤ
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *rnd, size_t ਅੰਕ)
{
  CURLcode ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਅਚਲ bool env_override = FALSE;
#ਜੇ_ਅੰਤ

  DEBUGASSERT(ਅੰਕ);

  ਜਦੋਂ(ਅੰਕ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ r;
    size_t ਖੱਬਾ = ਅੰਕ < ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ) ? ਅੰਕ : ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ);

    ਨਤੀਜਾ = randit(ਅੰਕੜੇ, &r, env_override);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਜਦੋਂ(ਖੱਬਾ) {
      *rnd++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(r & 0xFF);
      r >>= 8;
      --ਅੰਕ;
      --ਖੱਬਾ;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *rnd, size_t ਅੰਕ)
{
  CURLcode ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਬਫਰ[128];
  DEBUGASSERT(ਅੰਕ > 1);

  ਜੇ((ਅੰਕ / 2 >= ਆਕਾਰ(ਬਫਰ)) || !(ਅੰਕ & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(ਅੰਕੜੇ, "invalid buffer size with Curl_rand_hex"));
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਅੰਕ--; /* save one for null-termination */

  ਨਤੀਜਾ = Curl_rand(ਅੰਕੜੇ, ਬਫਰ, ਅੰਕ / 2);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  Curl_hexencode(ਬਫਰ, ਅੰਕ / 2, rnd, ਅੰਕ + 1);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *rnd,
                         size_t ਅੰਕ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ alnumspace = CURL_CSTRLEN(alnum);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ r;
  DEBUGASSERT(ਅੰਕ > 1);

  ਅੰਕ--; /* save one for null-termination */

  ਜਦੋਂ(ਅੰਕ) {
    ਕਰੋ {
      ਨਤੀਜਾ = randit(ਅੰਕੜੇ, &r, TRUE);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    } ਜਦੋਂ(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)alnum[r % alnumspace];
    ਅੰਕ--;
  }
  *rnd = 0;

  ਵਾਪਸ ਨਤੀਜਾ;
}
