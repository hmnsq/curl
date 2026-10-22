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

#ifdef USE_LIBPSL

#شامل "psl.h"
#شامل "progress.h"
#شامل "curl_share.h"

#اگر !defined(PSL_VERSION_عدد) || PSL_VERSION_عدد < 0x001000
#error "libpsl 0.16.0 or greater required"
#endif

تهی Curl_psl_destroy(ساختار PslCache *pslcache)
{
  اگر(pslcache->psl) {
    اگر(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = پوچ;
    pslcache->dynamic = FALSE;
  }
}

ثابت psl_ctx_t *Curl_psl_use(ساختار Curl_easy *easy)
{
  ساختار PslCache *pslcache = easy->psl;
  ثابت psl_ctx_t *psl;
  time_t now_sec;

  اگر(!pslcache)
    بازگشت پوچ;

  Curl_share_lock(easy, CURL_LOCK_DATA_PSL, CURL_LOCK_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  اگر(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_DATA_PSL, CURL_LOCK_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    اگر(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    اگر(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      time_t expires = TIME_T_MAX;

      psl = psl_latest(پوچ);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < TIME_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : TIME_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      اگر(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      اگر(psl) {
        Curl_psl_destroy(pslcache);
        pslcache->psl = psl;
        pslcache->dynamic = dynamic;
        pslcache->expires = expires;
      }
    }
    Curl_share_unlock(easy, CURL_LOCK_DATA_PSL); /* Release exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_DATA_PSL, CURL_LOCK_ACCESS_SHARED);
  }
  psl = pslcache->psl;
  اگر(!psl)
    Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);
  بازگشت psl;
}

تهی Curl_psl_release(ساختار Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);
}

#endif /* USE_LIBPSL */
