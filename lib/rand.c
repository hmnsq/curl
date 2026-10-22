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

#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "curl_trc.h"
#ضمن "rand.h"
#ضمن "escape.h"

#ضمن "curlx/winapi.h"

#جے_بنا_تعریف USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ساكن CURLcode weak_random(بنية Curl_easy *انکڑے,
                            بنانشانی حرف *entropy,
                            size_t لمائی) /* always 4, size of int */
{
  بنانشانی صحيح r;
  DEBUGASSERT(لمائی == حجم(صحيح));

  /* Trying cryptographically secure functions first */
#جے_تعریف_شدہ _WIN32
  (فراغ)انکڑے;
  {
    CURLcode نتیجہ = curlx_win32_random(entropy, لمائی);
    إذا(نتیجہ != CURLE_NOT_BUILT_وچ)
      أرجع نتیجہ;
  }
#جے_انت

#جے_تعریف_شدہ HAVE_ARC4RANDOM
  (فراغ)انکڑے;
  r = (بنانشانی صحيح)arc4random();
  حافظہ_نقل(entropy, &r, لمائی);
#وإلا
  infof(انکڑے, "WARNING: using weak random seed");
  {
    ساكن بنانشانی صحيح randseed;
    ساكن bool seeded = FALSE;
    بنانشانی صحيح rnd;
    إذا(!seeded) {
      بنية curltime now;
      curlx_pnow(&now);
      randseed += (بنانشانی صحيح)now.tv_usec + (بنانشانی صحيح)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    حافظہ_نقل(entropy, &rnd, لمائی);
  }
#جے_انت
  أرجع CURLE_OK;
}
#جے_انت

ساكن CURLcode randit(بنية Curl_easy *انکڑے, بنانشانی صحيح *rnd,
                       bool env_override)
{
#جے_تعریف_شدہ DEBUGBUILD
  إذا(env_override) {
    حرف *force_entropy = getenv("CURL_ENTROPY");
    إذا(force_entropy) {
      ساكن بنانشانی صحيح randseed;
      ساكن bool seeded = FALSE;

      إذا(!seeded) {
        بنانشانی صحيح بیج = 0;
        size_t elen = لڑی_لمبائی(force_entropy);
        size_t clen = حجم(بیج);
        size_t min = elen < clen ? elen : clen;
        حافظہ_نقل((حرف *)&بیج, force_entropy, min);
        randseed = ntohl(بیج);
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
#جے_انت

  /* data may be NULL! */
#جے_تعریف_شدہ USE_SSL
  أرجع Curl_ssl_random(انکڑے, (بنانشانی حرف *)rnd, حجم(*rnd));
#وإلا
  أرجع weak_random(انکڑے, (بنانشانی حرف *)rnd, حجم(*rnd));
#جے_انت
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

CURLcode Curl_rand_بائٹاں(بنية Curl_easy *انکڑے,
#جے_تعریف_شدہ DEBUGBUILD
                         bool env_override,
#جے_انت
                         بنانشانی حرف *rnd, size_t عدد)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
#جے_بنا_تعریف DEBUGBUILD
  ثابت bool env_override = FALSE;
#جے_انت

  DEBUGASSERT(عدد);

  طالما(عدد) {
    بنانشانی صحيح r;
    size_t بایاں = عدد < حجم(بنانشانی صحيح) ? عدد : حجم(بنانشانی صحيح);

    نتیجہ = randit(انکڑے, &r, env_override);
    إذا(نتیجہ)
      أرجع نتیجہ;

    طالما(بایاں) {
      *rnd++ = (بنانشانی حرف)(r & 0xFF);
      r >>= 8;
      --عدد;
      --بایاں;
    }
  }

  أرجع نتیجہ;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(بنية Curl_easy *انکڑے, بنانشانی حرف *rnd, size_t عدد)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  بنانشانی حرف بفر[128];
  DEBUGASSERT(عدد > 1);

  إذا((عدد / 2 >= حجم(بفر)) || !(عدد & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(انکڑے, "invalid buffer size with Curl_rand_hex"));
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  عدد--; /* save one for null-termination */

  نتیجہ = Curl_rand(انکڑے, بفر, عدد / 2);
  إذا(نتیجہ)
    أرجع نتیجہ;

  Curl_hexencode(بفر, عدد / 2, rnd, عدد + 1);
  أرجع نتیجہ;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ساكن ثابت حرف alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(بنية Curl_easy *انکڑے, بنانشانی حرف *rnd,
                         size_t عدد)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت بنانشانی صحيح alnumspace = CURL_CSTRLEN(alnum);
  بنانشانی صحيح r;
  DEBUGASSERT(عدد > 1);

  عدد--; /* save one for null-termination */

  طالما(عدد) {
    افعل {
      نتیجہ = randit(انکڑے, &r, TRUE);
      إذا(نتیجہ)
        أرجع نتیجہ;
    } طالما(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (بنانشانی حرف)alnum[r % alnumspace];
    عدد--;
  }
  *rnd = 0;

  أرجع نتیجہ;
}
