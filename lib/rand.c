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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "escape.h"

#అన్తర్భావ "curlx/winapi.h"

#నిర్వచితం_కాకపోతే USE_SSL
/* ---- possibly non-cryptographic version following ---- */
స్థిర CURLcode weak_random(నిర్మాణం Curl_easy *దత్తాంశం,
                            గుర్తులేని అక్షరం *entropy,
                            size_t నిడివి) /* always 4, size of int */
{
  గుర్తులేని పూర్ణాంకం r;
  DEBUGASSERT(నిడివి == పరిమాణం(పూర్ణాంకం));

  /* Trying cryptographically secure functions first */
#నిర్వచితమైతే _WIN32
  (శూన్య)దత్తాంశం;
  {
    CURLcode ఫలితం = curlx_win32_random(entropy, నిడివి);
    ఒకవేళ(ఫలితం != CURLE_NOT_BUILT_లోపల)
      తిరిగి ఫలితం;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ARC4RANDOM
  (శూన్య)దత్తాంశం;
  r = (గుర్తులేని పూర్ణాంకం)arc4random();
  జ్ఞాపకం_నకలు(entropy, &r, నిడివి);
#లేకపోతే
  infof(దత్తాంశం, "WARNING: using weak random seed");
  {
    స్థిర గుర్తులేని పూర్ణాంకం randseed;
    స్థిర bool seeded = FALSE;
    గుర్తులేని పూర్ణాంకం rnd;
    ఒకవేళ(!seeded) {
      నిర్మాణం curltime now;
      curlx_pnow(&now);
      randseed += (గుర్తులేని పూర్ణాంకం)now.tv_usec + (గుర్తులేని పూర్ణాంకం)now.tv_sec;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      randseed = randseed * 1103515245 + 12345;
      seeded = TRUE;
    }

    /* Return an unsigned 32-bit pseudo-random number. */
    r = randseed = randseed * 1103515245 + 12345;
    rnd = (r << 16) | ((r >> 16) & 0xFFFF);
    జ్ఞాపకం_నకలు(entropy, &rnd, నిడివి);
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు

స్థిర CURLcode randit(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని పూర్ణాంకం *rnd,
                       bool env_override)
{
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(env_override) {
    అక్షరం *force_entropy = getenv("CURL_ENTROPY");
    ఒకవేళ(force_entropy) {
      స్థిర గుర్తులేని పూర్ణాంకం randseed;
      స్థిర bool seeded = FALSE;

      ఒకవేళ(!seeded) {
        గుర్తులేని పూర్ణాంకం విత్తనం = 0;
        size_t elen = అక్షరశ్రేణి_పొడవు(force_entropy);
        size_t clen = పరిమాణం(విత్తనం);
        size_t min = elen < clen ? elen : clen;
        జ్ఞాపకం_నకలు((అక్షరం *)&విత్తనం, force_entropy, min);
        randseed = ntohl(విత్తనం);
        seeded = TRUE;
      }
      లేకపోతే
        randseed++;
      *rnd = randseed;
      తిరిగి CURLE_OK;
    }
  }
#లేకపోతే
  (శూన్య)env_override;
#అయితే_ముగింపు

  /* data may be NULL! */
#నిర్వచితమైతే USE_SSL
  తిరిగి Curl_ssl_random(దత్తాంశం, (గుర్తులేని అక్షరం *)rnd, పరిమాణం(*rnd));
#లేకపోతే
  తిరిగి weak_random(దత్తాంశం, (గుర్తులేని అక్షరం *)rnd, పరిమాణం(*rnd));
#అయితే_ముగింపు
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

CURLcode Curl_rand_బైట్లు(నిర్మాణం Curl_easy *దత్తాంశం,
#నిర్వచితమైతే DEBUGBUILD
                         bool env_override,
#అయితే_ముగింపు
                         గుర్తులేని అక్షరం *rnd, size_t అంకె)
{
  CURLcode ఫలితం = CURLE_BAD_FUNCTION_వాదం;
#నిర్వచితం_కాకపోతే DEBUGBUILD
  స్థిరరాశి bool env_override = FALSE;
#అయితే_ముగింపు

  DEBUGASSERT(అంకె);

  అయితే(అంకె) {
    గుర్తులేని పూర్ణాంకం r;
    size_t ఎడమ = అంకె < పరిమాణం(గుర్తులేని పూర్ణాంకం) ? అంకె : పరిమాణం(గుర్తులేని పూర్ణాంకం);

    ఫలితం = randit(దత్తాంశం, &r, env_override);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    అయితే(ఎడమ) {
      *rnd++ = (గుర్తులేని అక్షరం)(r & 0xFF);
      r >>= 8;
      --అంకె;
      --ఎడమ;
    }
  }

  తిరిగి ఫలితం;
}

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */

CURLcode Curl_rand_hex(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని అక్షరం *rnd, size_t అంకె)
{
  CURLcode ఫలితం = CURLE_BAD_FUNCTION_వాదం;
  గుర్తులేని అక్షరం బఫరు[128];
  DEBUGASSERT(అంకె > 1);

  ఒకవేళ((అంకె / 2 >= పరిమాణం(బఫరు)) || !(అంకె & 1)) {
    /* make sure it fits in the local buffer and that it is an odd number! */
    DEBUGF(infof(దత్తాంశం, "invalid buffer size with Curl_rand_hex"));
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  అంకె--; /* save one for null-termination */

  ఫలితం = Curl_rand(దత్తాంశం, బఫరు, అంకె / 2);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  Curl_hexencode(బఫరు, అంకె / 2, rnd, అంకె + 1);
  తిరిగి ఫలితం;
}

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */

స్థిర స్థిరరాశి అక్షరం alnum[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

CURLcode Curl_rand_alnum(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని అక్షరం *rnd,
                         size_t అంకె)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి గుర్తులేని పూర్ణాంకం alnumspace = CURL_CSTRLEN(alnum);
  గుర్తులేని పూర్ణాంకం r;
  DEBUGASSERT(అంకె > 1);

  అంకె--; /* save one for null-termination */

  అయితే(అంకె) {
    చేయి {
      ఫలితం = randit(దత్తాంశం, &r, TRUE);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    } అయితే(r >= (UINT_MAX - UINT_MAX % alnumspace));

    *rnd++ = (గుర్తులేని అక్షరం)alnum[r % alnumspace];
    అంకె--;
  }
  *rnd = 0;

  తిరిగి ఫలితం;
}
