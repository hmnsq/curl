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
                            अचिन्हित अक्षर *entropy,
                            माप_प्रकार length) /* always 4, size of int */
{
  अचिन्हित पूर्णांक r;
  DEBUGASSERT(length == आकार(पूर्णांक));

  /* Trying cryptographically secure functions first */
#ifdef _WIN32
  (शून्य)data;
  {
    CURLcode result = curlx_win32_random(entropy, length);
    जर(result != CURLE_NOT_BUILT_IN)
      परत result;
  }
#endif

#ifdef HAVE_ARC4RANDOM
  (शून्य)data;
  r = (अचिन्हित पूर्णांक)arc4random();
  स_नकल(entropy, &r, length);
#अन्यथा
  infof(data, "WARNING: using weak random seed");
  {
    स्थिर अचिन्हित पूर्णांक randseed;
    स्थिर bool seeded = FALSE;
    अचिन्हित पूर्णांक rnd;
    जर(!seeded) {
      संरचना curltime now;
      curlx_pnow(&now);
      randseed += (अचिन्हित पूर्णांक)now.tv_usec + (अचिन्हित पूर्णांक)now.tv_sec;
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
  परत CURLE_OK;
}
#endif

स्थिर CURLcode randit(संरचना Curl_easy *data, अचिन्हित पूर्णांक *rnd,
                       bool env_override)
{
#ifdef DEBUGBUILD
  जर(env_override) {
    अक्षर *force_entropy = दो_पर्या("CURL_ENTROPY");
    जर(force_entropy) {
      स्थिर अचिन्हित पूर्णांक randseed;
      स्थिर bool seeded = FALSE;

      जर(!seeded) {
        अचिन्हित पूर्णांक seed = 0;
        माप_प्रकार elen = म_माप(force_entropy);
        माप_प्रकार clen = आकार(seed);
        माप_प्रकार min = elen < clen ? elen : clen;
        स_नकल((अक्षर *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      अन्यथा
        randseed++;
      *rnd = randseed;
      परत CURLE_OK;
    }
  }
#अन्यथा
  (शून्य)env_override;
#endif

  /* data may be NULL! */
#ifdef USE_SSL
  परत Curl_ssl_random(data, (अचिन्हित अक्षर *)rnd, आकार(*rnd));
#अन्यथा
  परत weak_random(data, (अचिन्हित अक्षर *)rnd, आकार(*rnd));
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
                         अचिन्हित अक्षर *rnd, माप_प्रकार अंक)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
#ifndef DEBUGBUILD
  स्थिरांक bool env_override = FALSE;
#endif

  DEBUGASSERT(अंक);

  पर्यंत(अंक) {
    अचिन्हित पूर्णांक r;
    माप_प्रकार डावा = अंक < आकार(अचिन्हित पूर्णांक) ? अंक : आकार(अचिन्हित पूर्णांक);

    result = randit(data, &r, env_override);
    जर(result)
      परत result;

    पर्यंत(डावा) {
      *rnd++ = (अचिन्हित अक्षर)(r & 0xFF);
      r >>= 8;
      --अंक;
      --डावा;
    }
  }

  परत result;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(संरचना Curl_easy *data, अचिन्हित अक्षर *rnd, माप_प्रकार अंक)
{
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;
  अचिन्हित अक्षर buffer[128];
  DEBUGASSERT(अंक > 1);

  जर((अंक / 2 >= आकार(buffer)) || !(अंक & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(data, "invalid buffer size with Curl_rand_hex"));
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अंक--; /* save one for null-termination */

  result = Curl_rand(data, buffer, अंक / 2);
  जर(result)
    परत result;

  Curl_hexencode(buffer, अंक / 2, rnd, अंक + 1);
  परत result;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

स्थिर स्थिरांक अक्षर alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(संरचना Curl_easy *data, अचिन्हित अक्षर *rnd,
                         माप_प्रकार अंक)
{
  CURLcode result = CURLE_OK;
  स्थिरांक अचिन्हित पूर्णांक alnumspace = CURL_CSTRLEN(alnum);
  अचिन्हित पूर्णांक r;
  DEBUGASSERT(अंक > 1);

  अंक--; /* save one for null-termination */

  पर्यंत(अंक) {
    करा {
      result = randit(data, &r, TRUE);
      जर(result)
        परत result;
    } पर्यंत(r >= (अच_पूर्णांक_उच्च - अच_पूर्णांक_उच्च % alnumspace));

    *rnd++ = (अचिन्हित अक्षर)alnum[r % alnumspace];
    अंक--;
  }
  *rnd = 0;

  परत result;
}
