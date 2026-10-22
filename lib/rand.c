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
#ضمن "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "curl_trc.h"
#ضمن "rand.h"
#ضمن "escape.h"

#ضمن "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ساكن CURLcode weak_random(بنية Curl_easy *data,
                            unsigned حرف *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned صحيح r;
  DEBUGASSERT(length == حجم(صحيح));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (فراغ)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    إذا(result != CURLE_NOT_BUILT_IN)
      أرجع result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (فراغ)data;
  r = (unsigned صحيح)arc4random();
  memcpy(entropy, &r, length);
#وإلا
  infof(data, "WARNING: using weak random seed");
  {
    ساكن unsigned صحيح randseed;
    ساكن bool seeded = FALSE;
    unsigned صحيح rnd;
    إذا(!seeded) {
      بنية curltime now;
      curlx_pnow(&now);
      randseed += (unsigned صحيح)now.tv_usec + (unsigned صحيح)now.tv_sec;
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
  أرجع CURLE_OK;
}
#endif

ساكن CURLcode randit(بنية Curl_easy *data, unsigned صحيح *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  إذا(env_override) {
    حرف *force_entropy = getenv("CURL_ENTROPY");
    إذا(force_entropy) {
      ساكن unsigned صحيح randseed;
      ساكن bool seeded = FALSE;

      إذا(!seeded) {
        unsigned صحيح seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = حجم(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((حرف *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      وإلا
        randseed++;
      *rnd = randseed;
      أرجع CURLE_OK;
    }
  }
#وإلا
  (فراغ)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  أرجع Curl_ssl_random(data, (unsigned حرف *)rnd, حجم(*rnd));
#وإلا
  أرجع weak_random(data, (unsigned حرف *)rnd, حجم(*rnd));
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

CURLcode Curl_rand_bytes(بنية Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned حرف *rnd, size_t رقم)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  ثابت bool env_override = FALSE;
#endif

  DEBUGASSERT(رقم);

  طالما(رقم) {
    unsigned صحيح r;
    size_t يسار = رقم < حجم(unsigned صحيح) ? رقم : حجم(unsigned صحيح);

    result = randit(data, &r, env_override);
    إذا(result)
      أرجع result;

    طالما(يسار) {
      *rnd++ = (unsigned حرف)(r & 0xFF);
      r >>= 8;
      --رقم;
      --يسار;
    }
  }

  أرجع result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(بنية Curl_easy *data, unsigned حرف *rnd, size_t رقم)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned حرف buffer[128];
  DEBUGASSERT(رقم > 1);

  إذا((رقم / 2 >= حجم(buffer)) || !(رقم & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }

  رقم--; /* save one for null-termination */

  result = Curl_rand(data, buffer, رقم / 2);
  إذا(result)
    أرجع result;

  Curl_hexencode(buffer, رقم / 2, rnd, رقم + 1);
  أرجع result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ساكن ثابت حرف alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(بنية Curl_easy *data, unsigned حرف *rnd,
                         size_t رقم)
{
  CURLcode result = CURLE_OK;
  ثابت unsigned صحيح alnumspace = CURL_CSTRLEN(alnum);
  unsigned صحيح r;
  DEBUGASSERT(رقم > 1);

  رقم--; /* save one for null-termination */

  طالما(رقم) {
    افعل {
      result = randit(data, &r, TRUE);
      إذا(result)
        أرجع result;
    } طالما(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned حرف)alnum[r % alnumspace];
    رقم--;
  }
  *rnd = 0;

  أرجع result;
}
