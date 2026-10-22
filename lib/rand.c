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

#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "escape.h"

#अन्तर्भाव "curlx/winapi.h"

#ifndef USE_SSL
/* ---- possibly non-cryptographic version following ---- */
स्थिर CURLcode weak_random(संरचना Curl_easy *data,
                            अचिन्हित वर्ण *entropy,
                            माप_प्रकार length) /* always 4, size of int */
{
  अचिन्हित पूर्णाङ्क r;
  DEBUGASSERT(length == परिमाण(पूर्णाङ्क));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (शून्य)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    यदि(result != CURLE_NOT_BUILT_IN)
      फर्काऊ result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (शून्य)data;
  r = (अचिन्हित पूर्णाङ्क)arc4random();
  स_नकल(entropy, &r, length);
#अन्यथा
  infof(data, "WARNING: using weak random seed");
  {
    स्थिर अचिन्हित पूर्णाङ्क randseed;
    स्थिर bool seeded = FALSE;
    अचिन्हित पूर्णाङ्क rnd;
    यदि(!seeded) {
      संरचना curltime now;
      curlx_pnow(&now);
      randseed += (अचिन्हित पूर्णाङ्क)now.tv_usec + (अचिन्हित पूर्णाङ्क)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    स_नकल(entropy, &rnd, length);
  }
#endif
  फर्काऊ CURLE_OK;
}
#endif

स्थिर CURLcode randit(संरचना Curl_easy *data, अचिन्हित पूर्णाङ्क *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  यदि(env_override) {
    वर्ण *force_entropy = दो_पर्या("CURL_ENTROPY");
    यदि(force_entropy) {
      स्थिर अचिन्हित पूर्णाङ्क randseed;
      स्थिर bool seeded = FALSE;

      यदि(!seeded) {
        अचिन्हित पूर्णाङ्क seed = 0;
        माप_प्रकार elen = म_माप(force_entropy);
        माप_प्रकार clen = परिमाण(seed);
        माप_प्रकार min = elen < clen ? elen : clen;
        स_नकल((वर्ण *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      अन्यथा
        randseed++;
      *rnd = randseed;
      फर्काऊ CURLE_OK;
    }
  }
#अन्यथा
  (शून्य)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  फर्काऊ Curl_ssl_random(data, (अचिन्हित वर्ण *)rnd, परिमाण(*rnd));
#अन्यथा
  फर्काऊ weak_random(data, (अचिन्हित वर्ण *)rnd, परिमाण(*rnd));
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

CURLcode Curl_rand_bytes(संरचना Curl_easy *data,
#ifdef DEBUGBUILD
                         bool env_override,
#endif
                         अचिन्हित वर्ण *rnd, माप_प्रकार अङ्क)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  अचर bool env_override = FALSE;
#endif

  DEBUGASSERT(अङ्क);

  जबसम्म(अङ्क) {
    अचिन्हित पूर्णाङ्क r;
    माप_प्रकार बायाँ = अङ्क < परिमाण(अचिन्हित पूर्णाङ्क) ? अङ्क : परिमाण(अचिन्हित पूर्णाङ्क);

    result = randit(data, &r, env_override);
    यदि(result)
      फर्काऊ result;

    जबसम्म(बायाँ) {
      *rnd++ = (अचिन्हित वर्ण)(r & 0xFF);
      r >>= 8;
      --अङ्क;
      --बायाँ;
    }
  }

  फर्काऊ result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(संरचना Curl_easy *data, अचिन्हित वर्ण *rnd, माप_प्रकार अङ्क)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  अचिन्हित वर्ण buffer[128];
  DEBUGASSERT(अङ्क > 1);

  यदि((अङ्क / 2 >= परिमाण(buffer)) || !(अङ्क & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अङ्क--; /* save one for null-termination */

  result = Curl_rand(data, buffer, अङ्क / 2);
  यदि(result)
    फर्काऊ result;

  Curl_hexencode(buffer, अङ्क / 2, rnd, अङ्क + 1);
  फर्काऊ result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

स्थिर अचर वर्ण alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(संरचना Curl_easy *data, अचिन्हित वर्ण *rnd,
                         माप_प्रकार अङ्क)
{
  CURLcode result = CURLE_OK;
  अचर अचिन्हित पूर्णाङ्क alnumspace = CURL_CSTRLEN(alnum);
  अचिन्हित पूर्णाङ्क r;
  DEBUGASSERT(अङ्क > 1);

  अङ्क--; /* save one for null-termination */

  जबसम्म(अङ्क) {
    गर {
      result = randit(data, &r, TRUE);
      यदि(result)
        फर्काऊ result;
    } जबसम्म(r >= (अच_पूर्णांक_उच्च - अच_पूर्णांक_उच्च % alnumspace));

    *rnd++ = (अचिन्हित वर्ण)alnum[r % alnumspace];
    अङ्क--;
  }
  *rnd = 0;

  फर्काऊ result;
}
