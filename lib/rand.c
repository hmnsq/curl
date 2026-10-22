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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"

#ଅନ୍ତର୍ଭାଵ "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ସ୍ଥିର CURLcode weak_random(ସଂରଚନା Curl_easy *data,
                            unsigned ଅକ୍ଷର *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
  DEBUGASSERT(length == ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (ଶୂନ୍ୟ)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    ଯଦି(result != CURLE_NOT_BUILT_IN)
      ଫେରନ୍ତୁ result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (ଶୂନ୍ୟ)data;
  r = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)arc4random();
  memcpy(entropy, &r, length);
#ଅନ୍ୟଥା
  infof(data, "WARNING: using weak random seed");
  {
    ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା randseed;
    ସ୍ଥିର bool seeded = FALSE;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା rnd;
    ଯଦି(!seeded) {
      ସଂରଚନା curltime now;
      curlx_pnow(&now);
      randseed += (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)now.tv_usec + (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)now.tv_sec;
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
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif

ସ୍ଥିର CURLcode randit(ସଂରଚନା Curl_easy *data, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  ଯଦି(env_override) {
    ଅକ୍ଷର *force_entropy = getenv("CURL_ENTROPY");
    ଯଦି(force_entropy) {
      ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା randseed;
      ସ୍ଥିର bool seeded = FALSE;

      ଯଦି(!seeded) {
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = ଆକାର(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((ଅକ୍ଷର *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      ଅନ୍ୟଥା
        randseed++;
      *rnd = randseed;
      ଫେରନ୍ତୁ CURLE_OK;
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  ଫେରନ୍ତୁ Curl_ssl_random(data, (unsigned ଅକ୍ଷର *)rnd, ଆକାର(*rnd));
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ weak_random(data, (unsigned ଅକ୍ଷର *)rnd, ଆକାର(*rnd));
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

CURLcode Curl_rand_bytes(ସଂରଚନା Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned ଅକ୍ଷର *rnd, size_t ଅଙ୍କ)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  ଅଚଳ bool env_override = FALSE;
#endif

  DEBUGASSERT(ଅଙ୍କ);

  ଯେତେବେଳେ(ଅଙ୍କ) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
    size_t ବାମ = ଅଙ୍କ < ଆକାର(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ? ଅଙ୍କ : ଆକାର(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

    result = randit(data, &r, env_override);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    ଯେତେବେଳେ(ବାମ) {
      *rnd++ = (unsigned ଅକ୍ଷର)(r & 0xFF);
      r >>= 8;
      --ଅଙ୍କ;
      --ବାମ;
    }
  }

  ଫେରନ୍ତୁ result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(ସଂରଚନା Curl_easy *data, unsigned ଅକ୍ଷର *rnd, size_t ଅଙ୍କ)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned ଅକ୍ଷର buffer[128];
  DEBUGASSERT(ଅଙ୍କ > 1);

  ଯଦି((ଅଙ୍କ / 2 >= ଆକାର(buffer)) || !(ଅଙ୍କ & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ଅଙ୍କ--; /* save one for null-termination */

  result = Curl_rand(data, buffer, ଅଙ୍କ / 2);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  Curl_hexencode(buffer, ଅଙ୍କ / 2, rnd, ଅଙ୍କ + 1);
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(ସଂରଚନା Curl_easy *data, unsigned ଅକ୍ଷର *rnd,
                         size_t ଅଙ୍କ)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା alnumspace = CURL_CSTRLEN(alnum);
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
  DEBUGASSERT(ଅଙ୍କ > 1);

  ଅଙ୍କ--; /* save one for null-termination */

  ଯେତେବେଳେ(ଅଙ୍କ) {
    କର {
      result = randit(data, &r, TRUE);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    } ଯେତେବେଳେ(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned ଅକ୍ଷର)alnum[r % alnumspace];
    ଅଙ୍କ--;
  }
  *rnd = 0;

  ଫେରନ୍ତୁ result;
}
