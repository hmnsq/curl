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

#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "curl_trc.h"
#ضمن "rand.h"
#ضمن "escape.h"

#ضمن "curlx/winapi.h"

#جيڪڏهن_وصف_نه_ٿيل USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ساكن CURLcode weak_random(بنية Curl_easy *معلومات,
                            بنانشان حرف *entropy,
                            size_t ڊيگهائي) /* always 4, size of int */
{
  بنانشان صحيح r;
  DEBUGASSERT(ڊيگهائي == حجم(صحيح));

  /* Trying cryptographically secure functions first */
#جيڪڏهن_وصف_ٿيل _WIN32
  (فراغ)معلومات;
  {
    CURLcode نتيجو = curlx_win32_random(entropy, ڊيگهائي);
    إذا(نتيجو != CURLE_NOT_BUILT_اندريون)
      أرجع نتيجو;
  }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_ARC4RANDOM
  (فراغ)معلومات;
  r = (بنانشان صحيح)arc4random();
  يادگيري_نقل(entropy, &r, ڊيگهائي);
#وإلا
  infof(معلومات, "WARNING: using weak random seed");
  {
    ساكن بنانشان صحيح randseed;
    ساكن bool seeded = FALSE;
    بنانشان صحيح rnd;
    إذا(!seeded) {
      بنية curltime now;
      curlx_pnow(&now);
      randseed += (بنانشان صحيح)now.tv_usec + (بنانشان صحيح)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    يادگيري_نقل(entropy, &rnd, ڊيگهائي);
  }
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو

ساكن CURLcode randit(بنية Curl_easy *معلومات, بنانشان صحيح *rnd,
                       bool env_override)
{
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(env_override) {
    حرف *force_entropy = getenv("CURL_ENTROPY");
    إذا(force_entropy) {
      ساكن بنانشان صحيح randseed;
      ساكن bool seeded = FALSE;

      إذا(!seeded) {
        بنانشان صحيح ٻج = 0;
        size_t elen = لڙي_ڊيگهه(force_entropy);
        size_t clen = حجم(ٻج);
        size_t min = elen < clen ? elen : clen;
        يادگيري_نقل((حرف *)&ٻج, force_entropy, min);
        randseed = ntohl(ٻج);
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
#جيڪڏهن_پورو

  /* data may be NULL! */
#جيڪڏهن_وصف_ٿيل USE_SSL
  أرجع Curl_ssl_random(معلومات, (بنانشان حرف *)rnd, حجم(*rnd));
#وإلا
  أرجع weak_random(معلومات, (بنانشان حرف *)rnd, حجم(*rnd));
#جيڪڏهن_پورو
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

CURLcode Curl_rand_بائيٽون(بنية Curl_easy *معلومات,
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
                         bool env_override,
#جيڪڏهن_پورو
                         بنانشان حرف *rnd, size_t انگ)
{
  CURLcode نتيجو = CURLE_BAD_FUNCTION_دليل;
#جيڪڏهن_وصف_نه_ٿيل DEBUGBUILD
  ثابت bool env_override = FALSE;
#جيڪڏهن_پورو

  DEBUGASSERT(انگ);

  طالما(انگ) {
    بنانشان صحيح r;
    size_t کاٻو = انگ < حجم(بنانشان صحيح) ? انگ : حجم(بنانشان صحيح);

    نتيجو = randit(معلومات, &r, env_override);
    إذا(نتيجو)
      أرجع نتيجو;

    طالما(کاٻو) {
      *rnd++ = (بنانشان حرف)(r & 0xFF);
      r >>= 8;
      --انگ;
      --کاٻو;
    }
  }

  أرجع نتيجو;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(بنية Curl_easy *معلومات, بنانشان حرف *rnd, size_t انگ)
{
  CURLcode نتيجو = CURLE_BAD_FUNCTION_دليل;
  بنانشان حرف بفر[128];
  DEBUGASSERT(انگ > 1);

  إذا((انگ / 2 >= حجم(بفر)) || !(انگ & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(معلومات, "invalid buffer size with Curl_rand_hex"));
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  انگ--; /* save one for null-termination */

  نتيجو = Curl_rand(معلومات, بفر, انگ / 2);
  إذا(نتيجو)
    أرجع نتيجو;

  Curl_hexencode(بفر, انگ / 2, rnd, انگ + 1);
  أرجع نتيجو;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ساكن ثابت حرف alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(بنية Curl_easy *معلومات, بنانشان حرف *rnd,
                         size_t انگ)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت بنانشان صحيح alnumspace = CURL_CSTRLEN(alnum);
  بنانشان صحيح r;
  DEBUGASSERT(انگ > 1);

  انگ--; /* save one for null-termination */

  طالما(انگ) {
    افعل {
      نتيجو = randit(معلومات, &r, TRUE);
      إذا(نتيجو)
        أرجع نتيجو;
    } طالما(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (بنانشان حرف)alnum[r % alnumspace];
    انگ--;
  }
  *rnd = 0;

  أرجع نتيجو;
}
