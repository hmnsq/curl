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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL

#ಅನ್ತರ್ಭಾವ "psl.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"

#ಆದರೆ !defined(PSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ) || PSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x001000
#ದೋಷ "libpsl 0.16.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_psl_destroy(ರಚನೆ PslCache *pslcache)
{
  ಆದರೆ(pslcache->psl) {
    ಆದರೆ(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = ಖಾಲಿ;
    pslcache->dynamic = FALSE;
  }
}

ಸ್ಥಿರಾಂಕ psl_ctx_t *Curl_psl_use(ರಚನೆ Curl_easy *easy)
{
  ರಚನೆ PslCache *pslcache = easy->psl;
  ಸ್ಥಿರಾಂಕ psl_ctx_t *psl;
  time_t now_sec;

  ಆದರೆ(!pslcache)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  Curl_share_ಬೀಗ(easy, CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL, CURL_ಬೀಗ_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  ಆದರೆ(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_ಬೀಗ(easy, CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL, CURL_ಬೀಗ_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    ಆದರೆ(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    ಆದರೆ(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      time_t expires = ಸಮಯ_T_MAX;

      psl = psl_latest(ಖಾಲಿ);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < ಸಮಯ_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : ಸಮಯ_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      ಆದರೆ(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      ಆದರೆ(psl) {
        Curl_psl_destroy(pslcache);
        pslcache->psl = psl;
        pslcache->dynamic = dynamic;
        pslcache->expires = expires;
      }
    }
    Curl_share_unlock(easy, CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL); /* Release exclusive lock. */
    Curl_share_ಬೀಗ(easy, CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL, CURL_ಬೀಗ_ACCESS_SHARED);
  }
  psl = pslcache->psl;
  ಆದರೆ(!psl)
    Curl_share_unlock(easy, CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL);
  ಹಿಂತಿರುಗಿ psl;
}

ಶೂನ್ಯ Curl_psl_release(ರಚನೆ Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBPSL */
