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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "vtls/vtls.h"
#समावेश "curl_trc.h"
#समावेश "rand.h"
#समावेश "escape.h"

#समावेश "curlx/winapi.h"

#अगर_अघोषित USE_SSL
/* ---- possibly non-cryptographic version following ---- */
अटल CURLcode weak_random(काष्ठा Curl_easy *तथ्य,
                            अचिन्हित अक्षर *entropy,
                            माप_प्रकार लंबाई) /* always 4, size of int */
{
  अचिन्हित पूर्णांक r;
  DEBUGASSERT(लंबाई == माप(पूर्णांक));

  /* Trying cryptographically secure functions first */
#अगर_घोषित _WIN32
  (व्योम)तथ्य;
  {
    CURLcode परिणाम = curlx_win32_random(entropy, लंबाई);
    अगर(परिणाम != CURLE_NOT_BUILT_IN)
      वापस परिणाम;
  }
#पूर्ण_अगर

#अगर_घोषित HAVE_ARC4RANDOM
  (व्योम)तथ्य;
  r = (अचिन्हित पूर्णांक)arc4random();
  स_नकल(entropy, &r, लंबाई);
#अन्यथा
  infof(तथ्य, "WARNING: using weak random seed");
  {
    अटल अचिन्हित पूर्णांक randseed;
    अटल bool seeded = FALSE;
    अचिन्हित पूर्णांक rnd;
    अगर(!seeded) {
      काष्ठा curltime now;
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
    स_नकल(entropy, &rnd, लंबाई);
  }
#पूर्ण_अगर
  वापस CURLE_OK;
}
#पूर्ण_अगर

अटल CURLcode randit(काष्ठा Curl_easy *तथ्य, अचिन्हित पूर्णांक *rnd,
                       bool env_override)
{
#अगर_घोषित DEBUGBUILD
  अगर(env_override) {
    अक्षर *force_entropy = दो_पर्या("CURL_ENTROPY");
    अगर(force_entropy) {
      अटल अचिन्हित पूर्णांक randseed;
      अटल bool seeded = FALSE;

      अगर(!seeded) {
        अचिन्हित पूर्णांक seed = 0;
        माप_प्रकार elen = म_माप(force_entropy);
        माप_प्रकार clen = माप(seed);
        माप_प्रकार min = elen < clen ? elen : clen;
        स_नकल((अक्षर *)&seed, force_entropy, min);
        randseed = ntohl(seed);
        seeded = TRUE;
      }
      अन्यथा
        randseed++;
      *rnd = randseed;
      वापस CURLE_OK;
    }
  }
#अन्यथा
  (व्योम)env_override;
#पूर्ण_अगर

  /* data may be NULL! */
#अगर_घोषित USE_SSL
  वापस Curl_ssl_random(तथ्य, (अचिन्हित अक्षर *)rnd, माप(*rnd));
#अन्यथा
  वापस weak_random(तथ्य, (अचिन्हित अक्षर *)rnd, माप(*rnd));
#पूर्ण_अगर
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

CURLcode Curl_rand_bytes(काष्ठा Curl_easy *तथ्य,
#अगर_घोषित DEBUGBUILD
                         bool env_override,
#पूर्ण_अगर
                         अचिन्हित अक्षर *rnd, माप_प्रकार अंक)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
#अगर_अघोषित DEBUGBUILD
  स्थिर bool env_override = FALSE;
#पूर्ण_अगर

  DEBUGASSERT(अंक);

  जबतक(अंक) {
    अचिन्हित पूर्णांक r;
    माप_प्रकार बायाँ = अंक < माप(अचिन्हित पूर्णांक) ? अंक : माप(अचिन्हित पूर्णांक);

    परिणाम = randit(तथ्य, &r, env_override);
    अगर(परिणाम)
      वापस परिणाम;

    जबतक(बायाँ) {
      *rnd++ = (अचिन्हित अक्षर)(r & 0xFF);
      r >>= 8;
      --अंक;
      --बायाँ;
    }
  }

  वापस परिणाम;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(काष्ठा Curl_easy *तथ्य, अचिन्हित अक्षर *rnd, माप_प्रकार अंक)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
  अचिन्हित अक्षर बफर[128];
  DEBUGASSERT(अंक > 1);

  अगर((अंक / 2 >= माप(बफर)) || !(अंक & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(तथ्य, "invalid buffer size with Curl_rand_hex"));
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  अंक--; /* save one for null-termination */

  परिणाम = Curl_rand(तथ्य, बफर, अंक / 2);
  अगर(परिणाम)
    वापस परिणाम;

  Curl_hexencode(बफर, अंक / 2, rnd, अंक + 1);
  वापस परिणाम;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

अटल स्थिर अक्षर alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(काष्ठा Curl_easy *तथ्य, अचिन्हित अक्षर *rnd,
                         माप_प्रकार अंक)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अचिन्हित पूर्णांक alnumspace = CURL_CSTRLEN(alnum);
  अचिन्हित पूर्णांक r;
  DEBUGASSERT(अंक > 1);

  अंक--; /* save one for null-termination */

  जबतक(अंक) {
    करो {
      परिणाम = randit(तथ्य, &r, TRUE);
      अगर(परिणाम)
        वापस परिणाम;
    } जबतक(r >= (अच_पूर्णांक_उच्च - अच_पूर्णांक_उच्च % alnumspace));

    *rnd++ = (अचिन्हित अक्षर)alnum[r % alnumspace];
    अंक--;
  }
  *rnd = 0;

  वापस परिणाम;
}
