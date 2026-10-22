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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "escape.h"

#அந்தர்பாவ "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
நிலையான CURLcode weak_random(கட்டமைப்பு Curl_easy *data,
                            unsigned எழுத்து *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned முழு r;
  DEBUGASSERT(length == அளவு(முழு));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (வெறுமை)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    எனில்(result != CURLE_NOT_BUILT_IN)
      திரும்பு result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (வெறுமை)data;
  r = (unsigned முழு)arc4random();
  memcpy(entropy, &r, length);
#இல்லையெனில்
  infof(data, "WARNING: using weak random seed");
  {
    நிலையான unsigned முழு randseed;
    நிலையான bool seeded = FALSE;
    unsigned முழு rnd;
    எனில்(!seeded) {
      கட்டமைப்பு curltime now;
      curlx_pnow(&now);
      randseed += (unsigned முழு)now.tv_usec + (unsigned முழு)now.tv_sec;
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
  திரும்பு CURLE_OK;
}
#endif

நிலையான CURLcode randit(கட்டமைப்பு Curl_easy *data, unsigned முழு *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  எனில்(env_override) {
    எழுத்து *force_entropy = getenv("CURL_ENTROPY");
    எனில்(force_entropy) {
      நிலையான unsigned முழு randseed;
      நிலையான bool seeded = FALSE;

      எனில்(!seeded) {
        unsigned முழு seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = அளவு(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((எழுத்து *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      இல்லையெனில்
        randseed++;
      *rnd = randseed;
      திரும்பு CURLE_OK;
    }
  }
#இல்லையெனில்
  (வெறுமை)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  திரும்பு Curl_ssl_random(data, (unsigned எழுத்து *)rnd, அளவு(*rnd));
#இல்லையெனில்
  திரும்பு weak_random(data, (unsigned எழுத்து *)rnd, அளவு(*rnd));
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

CURLcode Curl_rand_bytes(கட்டமைப்பு Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned எழுத்து *rnd, size_t இலக்கம்)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  மாறிலி bool env_override = FALSE;
#endif

  DEBUGASSERT(இலக்கம்);

  போது(இலக்கம்) {
    unsigned முழு r;
    size_t இடது = இலக்கம் < அளவு(unsigned முழு) ? இலக்கம் : அளவு(unsigned முழு);

    result = randit(data, &r, env_override);
    எனில்(result)
      திரும்பு result;

    போது(இடது) {
      *rnd++ = (unsigned எழுத்து)(r & 0xFF);
      r >>= 8;
      --இலக்கம்;
      --இடது;
    }
  }

  திரும்பு result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(கட்டமைப்பு Curl_easy *data, unsigned எழுத்து *rnd, size_t இலக்கம்)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned எழுத்து buffer[128];
  DEBUGASSERT(இலக்கம் > 1);

  எனில்((இலக்கம் / 2 >= அளவு(buffer)) || !(இலக்கம் & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  இலக்கம்--; /* save one for null-termination */

  result = Curl_rand(data, buffer, இலக்கம் / 2);
  எனில்(result)
    திரும்பு result;

  Curl_hexencode(buffer, இலக்கம் / 2, rnd, இலக்கம் + 1);
  திரும்பு result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

நிலையான மாறிலி எழுத்து alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(கட்டமைப்பு Curl_easy *data, unsigned எழுத்து *rnd,
                         size_t இலக்கம்)
{
  CURLcode result = CURLE_OK;
  மாறிலி unsigned முழு alnumspace = CURL_CSTRLEN(alnum);
  unsigned முழு r;
  DEBUGASSERT(இலக்கம் > 1);

  இலக்கம்--; /* save one for null-termination */

  போது(இலக்கம்) {
    செய் {
      result = randit(data, &r, TRUE);
      எனில்(result)
        திரும்பு result;
    } போது(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned எழுத்து)alnum[r % alnumspace];
    இலக்கம்--;
  }
  *rnd = 0;

  திரும்பு result;
}
