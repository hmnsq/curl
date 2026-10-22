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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "escape.h"

#अन्तर्भाव "curlx/winapi.h"

#यदि_अपरिभाषितम् USE_SSL
/* ---- possibly non-cryptographic version following ---- */
स्थिर CURLcode weak_random(संरचना Curl_easy *दत्त,
                            अचिह्नित वर्ण *entropy,
                            size_t दीर्घता) /* always 4, size of int */
{
  अचिह्नित पूर्णाङ्क r;
  DEBUGASSERT(दीर्घता == परिमाण(पूर्णाङ्क));

  /* Trying cryptographically secure functions first */
#यदि_परिभाषितम् _WIN32
  (शून्य)दत्त;
  {
    CURLcode परिणाम = curlx_win32_random(entropy, दीर्घता);
    यदि(परिणाम != CURLE_NOT_BUILT_अन्तर्)
      प्रत्यागम परिणाम;
  }
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_ARC4RANDOM
  (शून्य)दत्त;
  r = (अचिह्नित पूर्णाङ्क)arc4random();
  स्मृतिं_प्रतिलिख(entropy, &r, दीर्घता);
#अन्यथा
  infof(दत्त, "WARNING: using weak random seed");
  {
    स्थिर अचिह्नित पूर्णाङ्क randseed;
    स्थिर bool seeded = FALSE;
    अचिह्नित पूर्णाङ्क rnd;
    यदि(!seeded) {
      संरचना curltime now;
      curlx_pnow(&now);
      randseed += (अचिह्नित पूर्णाङ्क)now.tv_usec + (अचिह्नित पूर्णाङ्क)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    स्मृतिं_प्रतिलिख(entropy, &rnd, दीर्घता);
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः

स्थिर CURLcode randit(संरचना Curl_easy *दत्त, अचिह्नित पूर्णाङ्क *rnd,
                       bool env_override)
{
#यदि_परिभाषितम् DEBUGBUILD
  यदि(env_override) {
    वर्ण *force_entropy = getenv("CURL_ENTROPY");
    यदि(force_entropy) {
      स्थिर अचिह्नित पूर्णाङ्क randseed;
      स्थिर bool seeded = FALSE;

      यदि(!seeded) {
        अचिह्नित पूर्णाङ्क बीजम् = 0;
        size_t elen = वाक्यदैर्घ्यम्(force_entropy);
        size_t clen = परिमाण(बीजम्);
        size_t min = elen < clen ? elen : clen;
        स्मृतिं_प्रतिलिख((वर्ण *)&बीजम्, force_entropy, min);
        randseed = ntohl(बीजम्);
        seeded = TRUE;
      }
      अन्यथा
        randseed++;
      *rnd = randseed;
      प्रत्यागम CURLE_OK;
    }
  }
#अन्यथा
  (शून्य)env_override;
#यदि_अन्तः

  /* data may be NULL! */
#यदि_परिभाषितम् USE_SSL
  प्रत्यागम Curl_ssl_random(दत्त, (अचिह्नित वर्ण *)rnd, परिमाण(*rnd));
#अन्यथा
  प्रत्यागम weak_random(दत्त, (अचिह्नित वर्ण *)rnd, परिमाण(*rnd));
#यदि_अन्तः
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

CURLcode Curl_rand_अष्टकानि(संरचना Curl_easy *दत्त,
#यदि_परिभाषितम् DEBUGBUILD
                         bool env_override,
#यदि_अन्तः
                         अचिह्नित वर्ण *rnd, size_t अङ्कः)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_तर्कः;
#यदि_अपरिभाषितम् DEBUGBUILD
  ध्रुव bool env_override = FALSE;
#यदि_अन्तः

  DEBUGASSERT(अङ्कः);

  यावत्(अङ्कः) {
    अचिह्नित पूर्णाङ्क r;
    size_t वामः = अङ्कः < परिमाण(अचिह्नित पूर्णाङ्क) ? अङ्कः : परिमाण(अचिह्नित पूर्णाङ्क);

    परिणाम = randit(दत्त, &r, env_override);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    यावत्(वामः) {
      *rnd++ = (अचिह्नित वर्ण)(r & 0xFF);
      r >>= 8;
      --अङ्कः;
      --वामः;
    }
  }

  प्रत्यागम परिणाम;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(संरचना Curl_easy *दत्त, अचिह्नित वर्ण *rnd, size_t अङ्कः)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_तर्कः;
  अचिह्नित वर्ण बफर[128];
  DEBUGASSERT(अङ्कः > 1);

  यदि((अङ्कः / 2 >= परिमाण(बफर)) || !(अङ्कः & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(दत्त, "invalid buffer size with Curl_rand_hex"));
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  अङ्कः--; /* save one for null-termination */

  परिणाम = Curl_rand(दत्त, बफर, अङ्कः / 2);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  Curl_hexencode(बफर, अङ्कः / 2, rnd, अङ्कः + 1);
  प्रत्यागम परिणाम;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

स्थिर ध्रुव वर्ण alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(संरचना Curl_easy *दत्त, अचिह्नित वर्ण *rnd,
                         size_t अङ्कः)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव अचिह्नित पूर्णाङ्क alnumspace = CURL_CSTRLEN(alnum);
  अचिह्नित पूर्णाङ्क r;
  DEBUGASSERT(अङ्कः > 1);

  अङ्कः--; /* save one for null-termination */

  यावत्(अङ्कः) {
    कुरु {
      परिणाम = randit(दत्त, &r, TRUE);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    } यावत्(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (अचिह्नित वर्ण)alnum[r % alnumspace];
    अङ्कः--;
  }
  *rnd = 0;

  प्रत्यागम परिणाम;
}
