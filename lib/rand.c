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
স্থির CURLcode weak_random(কাঠামো Curl_easy *তথ্য,
                            unsigned অক্ষর *entropy,
                            size_t দৈর্ঘ্য) /* always 4, size of int */
{
  unsigned পূর্ণসংখ্যা r;
  DEBUGASSERT(দৈর্ঘ্য == আকার(পূর্ণসংখ্যা));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (শূন্য)তথ্য;
  {
    CURLcode ফলাফল = curlx_win32_random(entropy, দৈর্ঘ্য);
    যদি(ফলাফল != CURLE_NOT_BUILT_IN)
      ফেরত ফলাফল;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (শূন্য)তথ্য;
  r = (unsigned পূর্ণসংখ্যা)arc4random();
  memcpy(entropy, &r, দৈর্ঘ্য);
#অন্যথা
  infof(তথ্য, "WARNING: using weak random seed");
  {
    স্থির unsigned পূর্ণসংখ্যা randseed;
    স্থির bool seeded = FALSE;
    unsigned পূর্ণসংখ্যা rnd;
    যদি(!seeded) {
      কাঠামো curltime now;
      curlx_pnow(&now);
      randseed += (unsigned পূর্ণসংখ্যা)now.tv_usec + (unsigned পূর্ণসংখ্যা)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    memcpy(entropy, &rnd, দৈর্ঘ্য);
  }
#endif
  ফেরত CURLE_OK;
}
#endif

স্থির CURLcode randit(কাঠামো Curl_easy *তথ্য, unsigned পূর্ণসংখ্যা *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  যদি(env_override) {
    অক্ষর *force_entropy = getenv("CURL_ENTROPY");
    যদি(force_entropy) {
      স্থির unsigned পূর্ণসংখ্যা randseed;
      স্থির bool seeded = FALSE;

      যদি(!seeded) {
        unsigned পূর্ণসংখ্যা seed = 0;
        size_t elen = strlen(force_entropy);
        size_t clen = আকার(seed);
        size_t min = elen < clen ? elen : clen;
        memcpy((অক্ষর *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      অন্যথা
        randseed++;
      *rnd = randseed;
      ফেরত CURLE_OK;
    }
  }
#অন্যথা
  (শূন্য)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  ফেরত Curl_ssl_random(তথ্য, (unsigned অক্ষর *)rnd, আকার(*rnd));
#অন্যথা
  ফেরত weak_random(তথ্য, (unsigned অক্ষর *)rnd, আকার(*rnd));
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

CURLcode Curl_rand_bytes(কাঠামো Curl_easy *তথ্য,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         unsigned অক্ষর *rnd, size_t অঙ্ক)
{
  CURLcode ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  ধ্রুবক bool env_override = FALSE;
#endif

  DEBUGASSERT(অঙ্ক);

  যখন(অঙ্ক) {
    unsigned পূর্ণসংখ্যা r;
    size_t বাম = অঙ্ক < আকার(unsigned পূর্ণসংখ্যা) ? অঙ্ক : আকার(unsigned পূর্ণসংখ্যা);

    ফলাফল = randit(তথ্য, &r, env_override);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    যখন(বাম) {
      *rnd++ = (unsigned অক্ষর)(r & 0xFF);
      r >>= 8;
      --অঙ্ক;
      --বাম;
    }
  }

  ফেরত ফলাফল;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(কাঠামো Curl_easy *তথ্য, unsigned অক্ষর *rnd, size_t অঙ্ক)
{
  CURLcode ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
  unsigned অক্ষর বাফার[128];
  DEBUGASSERT(অঙ্ক > 1);

  যদি((অঙ্ক / 2 >= আকার(বাফার)) || !(অঙ্ক & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(তথ্য, "invalid buffer size with Curl_rand_hex"));
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  অঙ্ক--; /* save one for null-termination */

  ফলাফল = Curl_rand(তথ্য, বাফার, অঙ্ক / 2);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  Curl_hexencode(বাফার, অঙ্ক / 2, rnd, অঙ্ক + 1);
  ফেরত ফলাফল;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

স্থির ধ্রুবক অক্ষর alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(কাঠামো Curl_easy *তথ্য, unsigned অক্ষর *rnd,
                         size_t অঙ্ক)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক unsigned পূর্ণসংখ্যা alnumspace = CURL_CSTRLEN(alnum);
  unsigned পূর্ণসংখ্যা r;
  DEBUGASSERT(অঙ্ক > 1);

  অঙ্ক--; /* save one for null-termination */

  যখন(অঙ্ক) {
    করো {
      ফলাফল = randit(তথ্য, &r, TRUE);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    } যখন(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (unsigned অক্ষর)alnum[r % alnumspace];
    অঙ্ক--;
  }
  *rnd = 0;

  ফেরত ফলাফল;
}
