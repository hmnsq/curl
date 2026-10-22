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
#شامل "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif

#شامل "urldata.h"
#شامل "vtls/vtls.h"
#شامل "curl_trc.h"
#شامل "rand.h"
#شامل "escape.h"

#شامل "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ایستا CURLcode weak_random(ساختار Curl_easy *data,
                            unsigned نویسه *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned صحیح r;
  DEBUGASSERT(length == اندازه(صحیح));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (تهی)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    اگر(result != CURLE_NOT_BUILT_IN)
      بازگشت result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (تهی)data;
  r = (unsigned صحیح)arc4random();
  memcpy(entropy, &r, length);
#وگرنه
  infof(data, "WARNING: using weak random seed");
  {
    ایستا unsigned صحیح randseed;
    ایستا bool seeded = FALSE;
    unsigned صحیح rnd;
    اگر(!seeded) {
      ساختار curltime now;
      curlx_pnow(&now);
      randseed += (unsigned صحیح)now.tv_usec + (unsigned صحیح)now.tv_sec;
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
  بازگشت CURLE_OK;
}
#endif

ایستا CURLcode randit(ساختار Curl_easy *data, unsigned صحیح *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  اگر(env_override) {
    نویسه *force_entropy = getenv("CURL_ENTROPY");
    اگر(force_entropy) {
      ایستا unsigned صحیح randseed;
      ایستا bool seeded = FALSE;

      اگر(!seeded) {
        unsigned صحیح seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = اندازه(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((نویسه *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      وگرنه
        randseed++;
      *rnd = randseed;
      بازگشت CURLE_OK;
    }
  }
#وگرنه
  (تهی)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  بازگشت Curl_ssl_random(data, (unsigned نویسه *)rnd, اندازه(*rnd));
#وگرنه
  بازگشت weak_random(data, (unsigned نویسه *)rnd, اندازه(*rnd));
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

CURLcode Curl_rand_bytes(ساختار Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned نویسه *rnd, size_t رقم)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  ثابت bool env_override = FALSE;
#endif

  DEBUGASSERT(رقم);

  وقتی(رقم) {
    unsigned صحیح r;
    size_t چپ = رقم < اندازه(unsigned صحیح) ? رقم : اندازه(unsigned صحیح);

    result = randit(data, &r, env_override);
    اگر(result)
      بازگشت result;

    وقتی(چپ) {
      *rnd++ = (unsigned نویسه)(r & 0xFF);
      r >>= 8;
      --رقم;
      --چپ;
    }
  }

  بازگشت result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(ساختار Curl_easy *data, unsigned نویسه *rnd, size_t رقم)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned نویسه buffer[128];
  DEBUGASSERT(رقم > 1);

  اگر((رقم / 2 >= اندازه(buffer)) || !(رقم & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  رقم--; /* save one for null-termination */

  result = Curl_rand(data, buffer, رقم / 2);
  اگر(result)
    بازگشت result;

  Curl_hexencode(buffer, رقم / 2, rnd, رقم + 1);
  بازگشت result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ایستا ثابت نویسه alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(ساختار Curl_easy *data, unsigned نویسه *rnd,
                         size_t رقم)
{
  CURLcode result = CURLE_OK;
  ثابت unsigned صحیح alnumspace = CURL_CSTRLEN(alnum);
  unsigned صحیح r;
  DEBUGASSERT(رقم > 1);

  رقم--; /* save one for null-termination */

  وقتی(رقم) {
    انجامبده {
      result = randit(data, &r, TRUE);
      اگر(result)
        بازگشت result;
    } وقتی(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned نویسه)alnum[r % alnumspace];
    رقم--;
  }
  *rnd = 0;

  بازگشت result;
}
