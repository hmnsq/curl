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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "escape.h"

#অন্তর্ভাব "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
স্থির CURLcode weak_random(সংরচনা Curl_easy *data,
                            unsigned বর্ণ *entropy,
                            size_t length) /* always 4, size of int */
{
  unsigned পূর্ণাঙ্ক r;
  DEBUGASSERT(length == পরিমাণ(পূর্ণাঙ্ক));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (শূন্য)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    যদি(result != CURLE_NOT_BUILT_IN)
      প্রত্যাগম result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (শূন্য)data;
  r = (unsigned পূর্ণাঙ্ক)arc4random();
  memcpy(entropy, &r, length);
#অন্যথা
  infof(data, "WARNING: using weak random seed");
  {
    স্থির unsigned পূর্ণাঙ্ক randseed;
    স্থির bool seeded = FALSE;
    unsigned পূর্ণাঙ্ক rnd;
    যদি(!seeded) {
      সংরচনা curltime now;
      curlx_pnow(&now);
      randseed += (unsigned পূর্ণাঙ্ক)now.tv_usec + (unsigned পূর্ণাঙ্ক)now.tv_sec;
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
  প্রত্যাগম CURLE_OK;
}
#endif

স্থির CURLcode randit(সংরচনা Curl_easy *data, unsigned পূর্ণাঙ্ক *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  যদি(env_override) {
    বর্ণ *force_entropy = getenv("CURL_ENTROPY");
    যদি(force_entropy) {
      স্থির unsigned পূর্ণাঙ্ক randseed;
      স্থির bool seeded = FALSE;

      যদি(!seeded) {
        unsigned পূর্ণাঙ্ক seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = পরিমাণ(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((বর্ণ *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      অন্যথা
        randseed++;
      *rnd = randseed;
      প্রত্যাগম CURLE_OK;
    }
  }
#অন্যথা
  (শূন্য)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  প্রত্যাগম Curl_ssl_random(data, (unsigned বর্ণ *)rnd, পরিমাণ(*rnd));
#অন্যথা
  প্রত্যাগম weak_random(data, (unsigned বর্ণ *)rnd, পরিমাণ(*rnd));
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

CURLcode Curl_rand_bytes(সংরচনা Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned বর্ণ *rnd, size_t অঙ্ক)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  ধ্রুব bool env_override = FALSE;
#endif

  DEBUGASSERT(অঙ্ক);

  যাবত্(অঙ্ক) {
    unsigned পূর্ণাঙ্ক r;
    size_t বাম = অঙ্ক < পরিমাণ(unsigned পূর্ণাঙ্ক) ? অঙ্ক : পরিমাণ(unsigned পূর্ণাঙ্ক);

    result = randit(data, &r, env_override);
    যদি(result)
      প্রত্যাগম result;

    যাবত্(বাম) {
      *rnd++ = (unsigned বর্ণ)(r & 0xFF);
      r >>= 8;
      --অঙ্ক;
      --বাম;
    }
  }

  প্রত্যাগম result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(সংরচনা Curl_easy *data, unsigned বর্ণ *rnd, size_t অঙ্ক)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned বর্ণ buffer[128];
  DEBUGASSERT(অঙ্ক > 1);

  যদি((অঙ্ক / 2 >= পরিমাণ(buffer)) || !(অঙ্ক & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }

  অঙ্ক--; /* save one for null-termination */

  result = Curl_rand(data, buffer, অঙ্ক / 2);
  যদি(result)
    প্রত্যাগম result;

  Curl_hexencode(buffer, অঙ্ক / 2, rnd, অঙ্ক + 1);
  প্রত্যাগম result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

স্থির ধ্রুব বর্ণ alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(সংরচনা Curl_easy *data, unsigned বর্ণ *rnd,
                         size_t অঙ্ক)
{
  CURLcode result = CURLE_OK;
  ধ্রুব unsigned পূর্ণাঙ্ক alnumspace = CURL_CSTRLEN(alnum);
  unsigned পূর্ণাঙ্ক r;
  DEBUGASSERT(অঙ্ক > 1);

  অঙ্ক--; /* save one for null-termination */

  যাবত্(অঙ্ক) {
    কুরু {
      result = randit(data, &r, TRUE);
      যদি(result)
        প্রত্যাগম result;
    } যাবত্(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned বর্ণ)alnum[r % alnumspace];
    অঙ্ক--;
  }
  *rnd = 0;

  প্রত্যাগম result;
}
