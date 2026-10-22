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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL

#ਅਨ੍ਤਰ੍ਭਾਵ "psl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"

#ਜੇ !defined(PSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ) || PSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x001000
#ਗਲਤੀ "libpsl 0.16.0 or greater required"
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_psl_destroy(ਬਣਤਰ PslCache *pslcache)
{
  ਜੇ(pslcache->psl) {
    ਜੇ(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = ਨਿਰਾ;
    pslcache->dynamic = FALSE;
  }
}

ਅਚਲ psl_ctx_t *Curl_psl_use(ਬਣਤਰ Curl_easy *easy)
{
  ਬਣਤਰ PslCache *pslcache = easy->psl;
  ਅਚਲ psl_ctx_t *psl;
  time_t now_sec;

  ਜੇ(!pslcache)
    ਵਾਪਸ ਨਿਰਾ;

  Curl_share_ਤਾਲਾ(easy, CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL, CURL_ਤਾਲਾ_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  ਜੇ(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_ਤਾਲਾ(easy, CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL, CURL_ਤਾਲਾ_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    ਜੇ(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    ਜੇ(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      time_t expires = ਸਮਾਂ_T_MAX;

      psl = psl_latest(ਨਿਰਾ);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < ਸਮਾਂ_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : ਸਮਾਂ_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      ਜੇ(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      ਜੇ(psl) {
        Curl_psl_destroy(pslcache);
        pslcache->psl = psl;
        pslcache->dynamic = dynamic;
        pslcache->expires = expires;
      }
    }
    Curl_share_unlock(easy, CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL); /* Release exclusive lock. */
    Curl_share_ਤਾਲਾ(easy, CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL, CURL_ਤਾਲਾ_ACCESS_SHARED);
  }
  psl = pslcache->psl;
  ਜੇ(!psl)
    Curl_share_unlock(easy, CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL);
  ਵਾਪਸ psl;
}

ਖਾਲੀ Curl_psl_release(ਬਣਤਰ Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL);
}

#ਜੇ_ਅੰਤ /* USE_LIBPSL */
