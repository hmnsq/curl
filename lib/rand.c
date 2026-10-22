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

#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "escape.h"

#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ಸ್ಥಿರ CURLcode weak_random(ಸಂರಚನಾ Curl_easy *data,
                            unsigned ವರ್ಣ *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned ಪೂರ್ಣಾಙ್ಕ r;
  DEBUGASSERT(length == ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (ಶೂನ್ಯ)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    ಯದಿ(result != CURLE_NOT_BUILT_IN)
      ಪ್ರತ್ಯಾಗಮ result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (ಶೂನ್ಯ)data;
  r = (unsigned ಪೂರ್ಣಾಙ್ಕ)arc4random();
  memcpy(entropy, &r, length);
#ಅನ್ಯಥಾ
  infof(data, "WARNING: using weak random seed");
  {
    ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ randseed;
    ಸ್ಥಿರ bool seeded = FALSE;
    unsigned ಪೂರ್ಣಾಙ್ಕ rnd;
    ಯದಿ(!seeded) {
      ಸಂರಚನಾ curltime now;
      curlx_pnow(&now);
      randseed += (unsigned ಪೂರ್ಣಾಙ್ಕ)now.tv_usec + (unsigned ಪೂರ್ಣಾಙ್ಕ)now.tv_sec;
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
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

ಸ್ಥಿರ CURLcode randit(ಸಂರಚನಾ Curl_easy *data, unsigned ಪೂರ್ಣಾಙ್ಕ *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  ಯದಿ(env_override) {
    ವರ್ಣ *force_entropy = getenv("CURL_ENTROPY");
    ಯದಿ(force_entropy) {
      ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ randseed;
      ಸ್ಥಿರ bool seeded = FALSE;

      ಯದಿ(!seeded) {
        unsigned ಪೂರ್ಣಾಙ್ಕ seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = ಪರಿಮಾಣ(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((ವರ್ಣ *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      ಅನ್ಯಥಾ
        randseed++;
      *rnd = randseed;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_random(data, (unsigned ವರ್ಣ *)rnd, ಪರಿಮಾಣ(*rnd));
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ weak_random(data, (unsigned ವರ್ಣ *)rnd, ಪರಿಮಾಣ(*rnd));
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

CURLcode Curl_rand_bytes(ಸಂರಚನಾ Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned ವರ್ಣ *rnd, size_t ಅಂಕೆ)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  ಧ್ರುವ bool env_override = FALSE;
#endif

  DEBUGASSERT(ಅಂಕೆ);

  ಯಾವತ್(ಅಂಕೆ) {
    unsigned ಪೂರ್ಣಾಙ್ಕ r;
    size_t ಎಡ = ಅಂಕೆ < ಪರಿಮಾಣ(unsigned ಪೂರ್ಣಾಙ್ಕ) ? ಅಂಕೆ : ಪರಿಮಾಣ(unsigned ಪೂರ್ಣಾಙ್ಕ);

    result = randit(data, &r, env_override);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ಯಾವತ್(ಎಡ) {
      *rnd++ = (unsigned ವರ್ಣ)(r & 0xFF);
      r >>= 8;
      --ಅಂಕೆ;
      --ಎಡ;
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(ಸಂರಚನಾ Curl_easy *data, unsigned ವರ್ಣ *rnd, size_t ಅಂಕೆ)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned ವರ್ಣ buffer[128];
  DEBUGASSERT(ಅಂಕೆ > 1);

  ಯದಿ((ಅಂಕೆ / 2 >= ಪರಿಮಾಣ(buffer)) || !(ಅಂಕೆ & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ಅಂಕೆ--; /* save one for null-termination */

  result = Curl_rand(data, buffer, ಅಂಕೆ / 2);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  Curl_hexencode(buffer, ಅಂಕೆ / 2, rnd, ಅಂಕೆ + 1);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(ಸಂರಚನಾ Curl_easy *data, unsigned ವರ್ಣ *rnd,
                         size_t ಅಂಕೆ)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ alnumspace = CURL_CSTRLEN(alnum);
  unsigned ಪೂರ್ಣಾಙ್ಕ r;
  DEBUGASSERT(ಅಂಕೆ > 1);

  ಅಂಕೆ--; /* save one for null-termination */

  ಯಾವತ್(ಅಂಕೆ) {
    ಕುರು {
      result = randit(data, &r, TRUE);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    } ಯಾವತ್(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned ವರ್ಣ)alnum[r % alnumspace];
    ಅಂಕೆ--;
  }
  *rnd = 0;

  ಪ್ರತ್ಯಾಗಮ result;
}
