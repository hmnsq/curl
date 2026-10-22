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

#ifdef USE_LIBPSL

#ضمن "psl.h"
#ضمن "progress.h"
#ضمن "curl_share.h"

#إذا !defined(PSL_VERSION_نمبر) || PSL_VERSION_نمبر < 0x001000
#error "libpsl 0.16.0 or greater required"
#endif

فراغ Curl_psl_destroy(بنية PslCache *pslcache)
{
  إذا(pslcache->psl) {
    إذا(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = عدم;
    pslcache->dynamic = FALSE;
  }
}

ثابت psl_ctx_t *Curl_psl_use(بنية Curl_easy *easy)
{
  بنية PslCache *pslcache = easy->psl;
  ثابت psl_ctx_t *psl;
  time_t now_sec;

  إذا(!pslcache)
    أرجع عدم;

  Curl_share_lock(easy, CURL_LOCK_DATA_PSL, CURL_LOCK_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  إذا(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_DATA_PSL, CURL_LOCK_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    إذا(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    إذا(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      time_t expires = TIME_T_MAX;

      psl = psl_latest(عدم);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < TIME_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : TIME_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      إذا(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      إذا(psl) {
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
  إذا(!psl)
    Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);
  أرجع psl;
}

فراغ Curl_psl_release(بنية Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);
}

#endif /* USE_LIBPSL */
