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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "escape.h"

#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ಸ್ಥಿರ CURLcode weak_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy,
                            size_t ಉದ್ದಳತೆ) /* always 4, size of int */
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ r;
  DEBUGASSERT(ಉದ್ದಳತೆ == ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));

  /* Trying cryptographically secure functions first */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  {
    CURLcode ಫಲಿತಾಂಶ = curlx_win32_random(entropy, ಉದ್ದಳತೆ);
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_NOT_BUILT_ಒಳ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARC4RANDOM
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  r = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)arc4random();
  ಸ್ಮರಣೆ_ನಕಲು(entropy, &r, ಉದ್ದಳತೆ);
#ಇಲ್ಲದಿದ್ದರೆ
  infof(ದತ್ತಾಂಶ, "WARNING: using weak random seed");
  {
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ randseed;
    ಸ್ಥಿರ bool seeded = FALSE;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ rnd;
    ಆದರೆ(!seeded) {
      ರಚನೆ curltime now;
      curlx_pnow(&now);
      randseed += (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)now.tv_usec + (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    ಸ್ಮರಣೆ_ನಕಲು(entropy, &rnd, ಉದ್ದಳತೆ);
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode randit(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *rnd,
                       bool env_override)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(env_override) {
    ಅಕ್ಷರ *force_entropy = getenv("CURL_ENTROPY");
    ಆದರೆ(force_entropy) {
      ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ randseed;
      ಸ್ಥಿರ bool seeded = FALSE;

      ಆದರೆ(!seeded) {
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಬೀಜ = 0;
        size_t elen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(force_entropy);
        size_t clen = ಗಾತ್ರ(ಬೀಜ);
        size_t min = elen < clen ? elen : clen;
        ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)&ಬೀಜ, force_entropy, min);
        randseed = ntohl(ಬೀಜ);
        seeded = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        randseed++;
      *rnd = randseed;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)env_override;
#ಆದರೆ_ಅಂತ್ಯ

  /* data may be NULL! */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಹಿಂತಿರುಗಿ Curl_ssl_random(ದತ್ತಾಂಶ, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)rnd, ಗಾತ್ರ(*rnd));
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ weak_random(ದತ್ತಾಂಶ, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)rnd, ಗಾತ್ರ(*rnd));
#ಆದರೆ_ಅಂತ್ಯ
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

CURLcode Curl_rand_ಬೈಟುಗಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
                         bool env_override,
#ಆದರೆ_ಅಂತ್ಯ
                         ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *rnd, size_t ಅಂಕೆ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEBUGBUILD
  ಸ್ಥಿರಾಂಕ bool env_override = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

  DEBUGASSERT(ಅಂಕೆ);

  ಆಗ(ಅಂಕೆ) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ r;
    size_t ಎಡ = ಅಂಕೆ < ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ) ? ಅಂಕೆ : ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ);

    ಫಲಿತಾಂಶ = randit(ದತ್ತಾಂಶ, &r, env_override);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಆಗ(ಎಡ) {
      *rnd++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(r & 0xFF);
      r >>= 8;
      --ಅಂಕೆ;
      --ಎಡ;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *rnd, size_t ಅಂಕೆ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಬಫರ್[128];
  DEBUGASSERT(ಅಂಕೆ > 1);

  ಆದರೆ((ಅಂಕೆ / 2 >= ಗಾತ್ರ(ಬಫರ್)) || !(ಅಂಕೆ & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(ದತ್ತಾಂಶ, "invalid buffer size with Curl_rand_hex"));
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  ಅಂಕೆ--; /* save one for null-termination */

  ಫಲಿತಾಂಶ = Curl_rand(ದತ್ತಾಂಶ, ಬಫರ್, ಅಂಕೆ / 2);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  Curl_hexencode(ಬಫರ್, ಅಂಕೆ / 2, rnd, ಅಂಕೆ + 1);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *rnd,
                         size_t ಅಂಕೆ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ alnumspace = CURL_CSTRLEN(alnum);
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ r;
  DEBUGASSERT(ಅಂಕೆ > 1);

  ಅಂಕೆ--; /* save one for null-termination */

  ಆಗ(ಅಂಕೆ) {
    ಮಾಡು {
      ಫಲಿತಾಂಶ = randit(ದತ್ತಾಂಶ, &r, TRUE);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    } ಆಗ(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)alnum[r % alnumspace];
    ಅಂಕೆ--;
  }
  *rnd = 0;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
