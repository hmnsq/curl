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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "escape.h"

#അന്തര്ഭാവ "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
സ്ഥിതം CURLcode weak_random(ഘടന Curl_easy *data,
                            unsigned അക്ഷരം *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned പൂർണ്ണസംഖ്യ r;
  DEBUGASSERT(length == വലുപ്പം(പൂർണ്ണസംഖ്യ));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (ശൂന്യം)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    എങ്കിൽ(result != CURLE_NOT_BUILT_IN)
      തിരികെ result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (ശൂന്യം)data;
  r = (unsigned പൂർണ്ണസംഖ്യ)arc4random();
  memcpy(entropy, &r, length);
#അല്ലെങ്കിൽ
  infof(data, "WARNING: using weak random seed");
  {
    സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ randseed;
    സ്ഥിതം bool seeded = FALSE;
    unsigned പൂർണ്ണസംഖ്യ rnd;
    എങ്കിൽ(!seeded) {
      ഘടന curltime now;
      curlx_pnow(&now);
      randseed += (unsigned പൂർണ്ണസംഖ്യ)now.tv_usec + (unsigned പൂർണ്ണസംഖ്യ)now.tv_sec;
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
  തിരികെ CURLE_OK;
}
#endif

സ്ഥിതം CURLcode randit(ഘടന Curl_easy *data, unsigned പൂർണ്ണസംഖ്യ *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  എങ്കിൽ(env_override) {
    അക്ഷരം *force_entropy = getenv("CURL_ENTROPY");
    എങ്കിൽ(force_entropy) {
      സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ randseed;
      സ്ഥിതം bool seeded = FALSE;

      എങ്കിൽ(!seeded) {
        unsigned പൂർണ്ണസംഖ്യ seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = വലുപ്പം(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((അക്ഷരം *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      അല്ലെങ്കിൽ
        randseed++;
      *rnd = randseed;
      തിരികെ CURLE_OK;
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  തിരികെ Curl_ssl_random(data, (unsigned അക്ഷരം *)rnd, വലുപ്പം(*rnd));
#അല്ലെങ്കിൽ
  തിരികെ weak_random(data, (unsigned അക്ഷരം *)rnd, വലുപ്പം(*rnd));
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

CURLcode Curl_rand_bytes(ഘടന Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned അക്ഷരം *rnd, size_t അക്കം)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  സ്ഥിരം bool env_override = FALSE;
#endif

  DEBUGASSERT(അക്കം);

  ആകുമ്പോൾ(അക്കം) {
    unsigned പൂർണ്ണസംഖ്യ r;
    size_t ഇടത് = അക്കം < വലുപ്പം(unsigned പൂർണ്ണസംഖ്യ) ? അക്കം : വലുപ്പം(unsigned പൂർണ്ണസംഖ്യ);

    result = randit(data, &r, env_override);
    എങ്കിൽ(result)
      തിരികെ result;

    ആകുമ്പോൾ(ഇടത്) {
      *rnd++ = (unsigned അക്ഷരം)(r & 0xFF);
      r >>= 8;
      --അക്കം;
      --ഇടത്;
    }
  }

  തിരികെ result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(ഘടന Curl_easy *data, unsigned അക്ഷരം *rnd, size_t അക്കം)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned അക്ഷരം buffer[128];
  DEBUGASSERT(അക്കം > 1);

  എങ്കിൽ((അക്കം / 2 >= വലുപ്പം(buffer)) || !(അക്കം & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  അക്കം--; /* save one for null-termination */

  result = Curl_rand(data, buffer, അക്കം / 2);
  എങ്കിൽ(result)
    തിരികെ result;

  Curl_hexencode(buffer, അക്കം / 2, rnd, അക്കം + 1);
  തിരികെ result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

സ്ഥിതം സ്ഥിരം അക്ഷരം alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(ഘടന Curl_easy *data, unsigned അക്ഷരം *rnd,
                         size_t അക്കം)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ alnumspace = CURL_CSTRLEN(alnum);
  unsigned പൂർണ്ണസംഖ്യ r;
  DEBUGASSERT(അക്കം > 1);

  അക്കം--; /* save one for null-termination */

  ആകുമ്പോൾ(അക്കം) {
    ചെയ്യുക {
      result = randit(data, &r, TRUE);
      എങ്കിൽ(result)
        തിരികെ result;
    } ആകുമ്പോൾ(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned അക്ഷരം)alnum[r % alnumspace];
    അക്കം--;
  }
  *rnd = 0;

  തിരികെ result;
}
