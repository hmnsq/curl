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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "escape.h"

#अन्तर्भाव "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
स्थिर CURLcode weak_random(संरचना Curl_easy *data,
                            unsigned वर्ण *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned पूर्णाङ्क r;
  DEBUGASSERT(length == परिमाण(पूर्णाङ्क));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (शून्य)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    सचे(result != CURLE_NOT_BUILT_IN)
      पटिनिवत्त result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (शून्य)data;
  r = (unsigned पूर्णाङ्क)arc4random();
  memcpy(entropy, &r, length);
#अञ्ञथा
  infof(data, "WARNING: using weak random seed");
  {
    स्थिर unsigned पूर्णाङ्क randseed;
    स्थिर bool seeded = FALSE;
    unsigned पूर्णाङ्क rnd;
    सचे(!seeded) {
      संरचना curltime now;
      curlx_pnow(&now);
      randseed += (unsigned पूर्णाङ्क)now.tv_usec + (unsigned पूर्णाङ्क)now.tv_sec;
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
  पटिनिवत्त CURLE_OK;
}
#endif

स्थिर CURLcode randit(संरचना Curl_easy *data, unsigned पूर्णाङ्क *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  सचे(env_override) {
    वर्ण *force_entropy = getenv("CURL_ENTROPY");
    सचे(force_entropy) {
      स्थिर unsigned पूर्णाङ्क randseed;
      स्थिर bool seeded = FALSE;

      सचे(!seeded) {
        unsigned पूर्णाङ्क seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = परिमाण(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((वर्ण *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      अञ्ञथा
        randseed++;
      *rnd = randseed;
      पटिनिवत्त CURLE_OK;
    }
  }
#अञ्ञथा
  (शून्य)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  पटिनिवत्त Curl_ssl_random(data, (unsigned वर्ण *)rnd, परिमाण(*rnd));
#अञ्ञथा
  पटिनिवत्त weak_random(data, (unsigned वर्ण *)rnd, परिमाण(*rnd));
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

CURLcode Curl_rand_bytes(संरचना Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned वर्ण *rnd, size_t अङ्कः)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  ध्रुव bool env_override = FALSE;
#endif

  DEBUGASSERT(अङ्कः);

  याव(अङ्कः) {
    unsigned पूर्णाङ्क r;
    size_t वामः = अङ्कः < परिमाण(unsigned पूर्णाङ्क) ? अङ्कः : परिमाण(unsigned पूर्णाङ्क);

    result = randit(data, &r, env_override);
    सचे(result)
      पटिनिवत्त result;

    याव(वामः) {
      *rnd++ = (unsigned वर्ण)(r & 0xFF);
      r >>= 8;
      --अङ्कः;
      --वामः;
    }
  }

  पटिनिवत्त result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(संरचना Curl_easy *data, unsigned वर्ण *rnd, size_t अङ्कः)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned वर्ण buffer[128];
  DEBUGASSERT(अङ्कः > 1);

  सचे((अङ्कः / 2 >= परिमाण(buffer)) || !(अङ्कः & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अङ्कः--; /* save one for null-termination */

  result = Curl_rand(data, buffer, अङ्कः / 2);
  सचे(result)
    पटिनिवत्त result;

  Curl_hexencode(buffer, अङ्कः / 2, rnd, अङ्कः + 1);
  पटिनिवत्त result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

स्थिर ध्रुव वर्ण alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(संरचना Curl_easy *data, unsigned वर्ण *rnd,
                         size_t अङ्कः)
{
  CURLcode result = CURLE_OK;
  ध्रुव unsigned पूर्णाङ्क alnumspace = CURL_CSTRLEN(alnum);
  unsigned पूर्णाङ्क r;
  DEBUGASSERT(अङ्कः > 1);

  अङ्कः--; /* save one for null-termination */

  याव(अङ्कः) {
    करोहि {
      result = randit(data, &r, TRUE);
      सचे(result)
        पटिनिवत्त result;
    } याव(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned वर्ण)alnum[r % alnumspace];
    अङ्कः--;
  }
  *rnd = 0;

  पटिनिवत्त result;
}
