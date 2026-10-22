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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "escape.h"

#અન્તર્ભાવ "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
સ્થિર CURLcode weak_random(સંરચના Curl_easy *data,
                            unsigned અક્ષર *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned પૂર્ણાંક r;
  DEBUGASSERT(length == કદ(પૂર્ણાંક));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (શૂન્ય)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    જો(result != CURLE_NOT_BUILT_IN)
      પાછા result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (શૂન્ય)data;
  r = (unsigned પૂર્ણાંક)arc4random();
  memcpy(entropy, &r, length);
#નહીંતર
  infof(data, "WARNING: using weak random seed");
  {
    સ્થિર unsigned પૂર્ણાંક randseed;
    સ્થિર bool seeded = FALSE;
    unsigned પૂર્ણાંક rnd;
    જો(!seeded) {
      સંરચના curltime now;
      curlx_pnow(&now);
      randseed += (unsigned પૂર્ણાંક)now.tv_usec + (unsigned પૂર્ણાંક)now.tv_sec;
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
  પાછા CURLE_OK;
}
#endif

સ્થિર CURLcode randit(સંરચના Curl_easy *data, unsigned પૂર્ણાંક *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  જો(env_override) {
    અક્ષર *force_entropy = getenv("CURL_ENTROPY");
    જો(force_entropy) {
      સ્થિર unsigned પૂર્ણાંક randseed;
      સ્થિર bool seeded = FALSE;

      જો(!seeded) {
        unsigned પૂર્ણાંક seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = કદ(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((અક્ષર *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      નહીંતર
        randseed++;
      *rnd = randseed;
      પાછા CURLE_OK;
    }
  }
#નહીંતર
  (શૂન્ય)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  પાછા Curl_ssl_random(data, (unsigned અક્ષર *)rnd, કદ(*rnd));
#નહીંતર
  પાછા weak_random(data, (unsigned અક્ષર *)rnd, કદ(*rnd));
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

CURLcode Curl_rand_bytes(સંરચના Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned અક્ષર *rnd, size_t અંક)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  અચલ bool env_override = FALSE;
#endif

  DEBUGASSERT(અંક);

  જ્યારે(અંક) {
    unsigned પૂર્ણાંક r;
    size_t ડાબું = અંક < કદ(unsigned પૂર્ણાંક) ? અંક : કદ(unsigned પૂર્ણાંક);

    result = randit(data, &r, env_override);
    જો(result)
      પાછા result;

    જ્યારે(ડાબું) {
      *rnd++ = (unsigned અક્ષર)(r & 0xFF);
      r >>= 8;
      --અંક;
      --ડાબું;
    }
  }

  પાછા result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(સંરચના Curl_easy *data, unsigned અક્ષર *rnd, size_t અંક)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned અક્ષર buffer[128];
  DEBUGASSERT(અંક > 1);

  જો((અંક / 2 >= કદ(buffer)) || !(અંક & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  અંક--; /* save one for null-termination */

  result = Curl_rand(data, buffer, અંક / 2);
  જો(result)
    પાછા result;

  Curl_hexencode(buffer, અંક / 2, rnd, અંક + 1);
  પાછા result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

સ્થિર અચલ અક્ષર alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(સંરચના Curl_easy *data, unsigned અક્ષર *rnd,
                         size_t અંક)
{
  CURLcode result = CURLE_OK;
  અચલ unsigned પૂર્ણાંક alnumspace = CURL_CSTRLEN(alnum);
  unsigned પૂર્ણાંક r;
  DEBUGASSERT(અંક > 1);

  અંક--; /* save one for null-termination */

  જ્યારે(અંક) {
    કરો {
      result = randit(data, &r, TRUE);
      જો(result)
        પાછા result;
    } જ્યારે(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned અક્ષર)alnum[r % alnumspace];
    અંક--;
  }
  *rnd = 0;

  પાછા result;
}
