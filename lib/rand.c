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

#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "curl_trc.h"
#ضمن "rand.h"
#ضمن "escape.h"

#ضمن "curlx/winapi.h"

#که_نه_تعریف_شوی USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ساكن CURLcode weak_random(بنية Curl_easy *معلومات,
                            بینښه حرف *entropy,
                            size_t اوږدتیا) /* always 4, size of int */
{
  بینښه صحيح r;
  DEBUGASSERT(اوږدتیا == حجم(صحيح));

  /* Trying cryptographically secure functions first */
#که_تعریف_شوی _WIN32
  (فراغ)معلومات;
  {
    CURLcode پایله = curlx_win32_random(entropy, اوږدتیا);
    إذا(پایله != CURLE_NOT_BUILT_دننه)
      أرجع پایله;
  }
#د_که_پای

#که_تعریف_شوی HAVE_ARC4RANDOM
  (فراغ)معلومات;
  r = (بینښه صحيح)arc4random();
  حافظه_نقل(entropy, &r, اوږدتیا);
#وإلا
  infof(معلومات, "WARNING: using weak random seed");
  {
    ساكن بینښه صحيح randseed;
    ساكن bool seeded = FALSE;
    بینښه صحيح rnd;
    إذا(!seeded) {
      بنية curltime now;
      curlx_pnow(&now);
      randseed += (بینښه صحيح)now.tv_usec + (بینښه صحيح)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    حافظه_نقل(entropy, &rnd, اوږدتیا);
  }
#د_که_پای
  أرجع CURLE_OK;
}
#د_که_پای

ساكن CURLcode randit(بنية Curl_easy *معلومات, بینښه صحيح *rnd,
                       bool env_override)
{
#که_تعریف_شوی DEBUGBUILD
  إذا(env_override) {
    حرف *force_entropy = getenv("CURL_ENTROPY");
    إذا(force_entropy) {
      ساكن بینښه صحيح randseed;
      ساكن bool seeded = FALSE;

      إذا(!seeded) {
        بینښه صحيح تخم = 0;
        size_t elen = مزي_اوږدوالی(force_entropy);
        size_t clen = حجم(تخم);
        size_t min = elen < clen ? elen : clen;
        حافظه_نقل((حرف *)&تخم, force_entropy, min);
        randseed = ntohl(تخم);
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
#د_که_پای

  /* data may be NULL! */
#که_تعریف_شوی USE_SSL
  أرجع Curl_ssl_random(معلومات, (بینښه حرف *)rnd, حجم(*rnd));
#وإلا
  أرجع weak_random(معلومات, (بینښه حرف *)rnd, حجم(*rnd));
#د_که_پای
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

CURLcode Curl_rand_بایټونه(بنية Curl_easy *معلومات,
#که_تعریف_شوی DEBUGBUILD
                         bool env_override,
#د_که_پای
                         بینښه حرف *rnd, size_t عدد)
{
  CURLcode پایله = CURLE_BAD_FUNCTION_دلیل;
#که_نه_تعریف_شوی DEBUGBUILD
  ثابت bool env_override = FALSE;
#د_که_پای

  DEBUGASSERT(عدد);

  طالما(عدد) {
    بینښه صحيح r;
    size_t کیڼ = عدد < حجم(بینښه صحيح) ? عدد : حجم(بینښه صحيح);

    پایله = randit(معلومات, &r, env_override);
    إذا(پایله)
      أرجع پایله;

    طالما(کیڼ) {
      *rnd++ = (بینښه حرف)(r & 0xFF);
      r >>= 8;
      --عدد;
      --کیڼ;
    }
  }

  أرجع پایله;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(بنية Curl_easy *معلومات, بینښه حرف *rnd, size_t عدد)
{
  CURLcode پایله = CURLE_BAD_FUNCTION_دلیل;
  بینښه حرف بفر[128];
  DEBUGASSERT(عدد > 1);

  إذا((عدد / 2 >= حجم(بفر)) || !(عدد & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(معلومات, "invalid buffer size with Curl_rand_hex"));
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  عدد--; /* save one for null-termination */

  پایله = Curl_rand(معلومات, بفر, عدد / 2);
  إذا(پایله)
    أرجع پایله;

  Curl_hexencode(بفر, عدد / 2, rnd, عدد + 1);
  أرجع پایله;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ساكن ثابت حرف alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(بنية Curl_easy *معلومات, بینښه حرف *rnd,
                         size_t عدد)
{
  CURLcode پایله = CURLE_OK;
  ثابت بینښه صحيح alnumspace = CURL_CSTRLEN(alnum);
  بینښه صحيح r;
  DEBUGASSERT(عدد > 1);

  عدد--; /* save one for null-termination */

  طالما(عدد) {
    افعل {
      پایله = randit(معلومات, &r, TRUE);
      إذا(پایله)
        أرجع پایله;
    } طالما(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (بینښه حرف)alnum[r % alnumspace];
    عدد--;
  }
  *rnd = 0;

  أرجع پایله;
}
