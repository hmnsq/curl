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

#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "vtls/vtls.h"
#شامل "curl_trc.h"
#شامل "rand.h"
#شامل "escape.h"

#شامل "curlx/winapi.h"

#اگر_غیر_تعریف_شدہ USE_SSL
/* ---- possibly non-cryptographic version following ---- */
ساکن CURLcode weak_random(ساخت Curl_easy *ڈیٹا,
                            غیرعلامتی حرف *entropy,
                            size_t لمبائی) /* always 4, size of int */
{
  غیرعلامتی صحیح r;
  DEBUGASSERT(لمبائی == حجم(صحیح));

  /* Trying cryptographically secure functions first */
#اگر_تعریف_شدہ _WIN32
  (خالی)ڈیٹا;
  {
    CURLcode نتیجہ = curlx_win32_random(entropy, لمبائی);
    اگر(نتیجہ != CURLE_NOT_BUILT_اندر)
      واپس نتیجہ;
  }
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ARC4RANDOM
  (خالی)ڈیٹا;
  r = (غیرعلامتی صحیح)arc4random();
  حافظہ_نقل(entropy, &r, لمبائی);
#ورنہ
  infof(ڈیٹا, "WARNING: using weak random seed");
  {
    ساکن غیرعلامتی صحیح randseed;
    ساکن bool seeded = FALSE;
    غیرعلامتی صحیح rnd;
    اگر(!seeded) {
      ساخت curltime now;
      curlx_pnow(&now);
      randseed += (غیرعلامتی صحیح)now.tv_usec + (غیرعلامتی صحیح)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    حافظہ_نقل(entropy, &rnd, لمبائی);
  }
#اگر_ختم
  واپس CURLE_OK;
}
#اگر_ختم

ساکن CURLcode randit(ساخت Curl_easy *ڈیٹا, غیرعلامتی صحیح *rnd,
                       bool env_override)
{
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(env_override) {
    حرف *force_entropy = getenv("CURL_ENTROPY");
    اگر(force_entropy) {
      ساکن غیرعلامتی صحیح randseed;
      ساکن bool seeded = FALSE;

      اگر(!seeded) {
        غیرعلامتی صحیح بیج = 0;
        size_t elen = لڑی_لمبائی(force_entropy);
        size_t clen = حجم(بیج);
        size_t min = elen < clen ? elen : clen;
        حافظہ_نقل((حرف *)&بیج, force_entropy, min);
        randseed = ntohl(بیج);
        seeded = TRUE;
      }
      ورنہ
        randseed++;
      *rnd = randseed;
      واپس CURLE_OK;
    }
  }
#ورنہ
  (خالی)env_override;
#اگر_ختم

  /* data may be NULL! */
#اگر_تعریف_شدہ USE_SSL
  واپس Curl_ssl_random(ڈیٹا, (غیرعلامتی حرف *)rnd, حجم(*rnd));
#ورنہ
  واپس weak_random(ڈیٹا, (غیرعلامتی حرف *)rnd, حجم(*rnd));
#اگر_ختم
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

CURLcode Curl_rand_بائٹیں(ساخت Curl_easy *ڈیٹا,
#اگر_تعریف_شدہ DEBUGBUILD
                         bool env_override,
#اگر_ختم
                         غیرعلامتی حرف *rnd, size_t عدد)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
#اگر_غیر_تعریف_شدہ DEBUGBUILD
  ثابت bool env_override = FALSE;
#اگر_ختم

  DEBUGASSERT(عدد);

  جب(عدد) {
    غیرعلامتی صحیح r;
    size_t بایاں = عدد < حجم(غیرعلامتی صحیح) ? عدد : حجم(غیرعلامتی صحیح);

    نتیجہ = randit(ڈیٹا, &r, env_override);
    اگر(نتیجہ)
      واپس نتیجہ;

    جب(بایاں) {
      *rnd++ = (غیرعلامتی حرف)(r & 0xFF);
      r >>= 8;
      --عدد;
      --بایاں;
    }
  }

  واپس نتیجہ;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(ساخت Curl_easy *ڈیٹا, غیرعلامتی حرف *rnd, size_t عدد)
{
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;
  غیرعلامتی حرف بفر[128];
  DEBUGASSERT(عدد > 1);

  اگر((عدد / 2 >= حجم(بفر)) || !(عدد & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(ڈیٹا, "invalid buffer size with Curl_rand_hex"));
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  عدد--; /* save one for null-termination */

  نتیجہ = Curl_rand(ڈیٹا, بفر, عدد / 2);
  اگر(نتیجہ)
    واپس نتیجہ;

  Curl_hexencode(بفر, عدد / 2, rnd, عدد + 1);
  واپس نتیجہ;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

ساکن ثابت حرف alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(ساخت Curl_easy *ڈیٹا, غیرعلامتی حرف *rnd,
                         size_t عدد)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت غیرعلامتی صحیح alnumspace = CURL_CSTRLEN(alnum);
  غیرعلامتی صحیح r;
  DEBUGASSERT(عدد > 1);

  عدد--; /* save one for null-termination */

  جب(عدد) {
    کرو {
      نتیجہ = randit(ڈیٹا, &r, TRUE);
      اگر(نتیجہ)
        واپس نتیجہ;
    } جب(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (غیرعلامتی حرف)alnum[r % alnumspace];
    عدد--;
  }
  *rnd = 0;

  واپس نتیجہ;
}
