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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_LIBPSL

#অন্তর্ভাব "psl.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curl_share.h"

#যদি !defined(PSL_VERSION_সংখ্যা) || PSL_VERSION_সংখ্যা < 0x001000
#error "libpsl 0.16.0 or greater required"
#endif

শূন্য Curl_psl_destroy(কাঠামো PslCache *pslcache)
{
  যদি(pslcache->psl) {
    যদি(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = ফাঁকা;
    pslcache->dynamic = FALSE;
  }
}

ধ্রুবক psl_ctx_t *Curl_psl_use(কাঠামো Curl_easy *easy)
{
  কাঠামো PslCache *pslcache = easy->psl;
  ধ্রুবক psl_ctx_t *psl;
  time_t now_sec;

  যদি(!pslcache)
    ফেরত ফাঁকা;

  Curl_share_lock(easy, CURL_LOCK_তথ্য_PSL, CURL_LOCK_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  যদি(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_LOCK_তথ্য_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_তথ্য_PSL, CURL_LOCK_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    যদি(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    যদি(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      time_t expires = TIME_T_MAX;

      psl = psl_latest(ফাঁকা);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < TIME_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : TIME_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      যদি(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      যদি(psl) {
        Curl_psl_destroy(pslcache);
        pslcache->psl = psl;
        pslcache->dynamic = dynamic;
        pslcache->expires = expires;
      }
    }
    Curl_share_unlock(easy, CURL_LOCK_তথ্য_PSL); /* Release exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_তথ্য_PSL, CURL_LOCK_ACCESS_SHARED);
  }
  psl = pslcache->psl;
  যদি(!psl)
    Curl_share_unlock(easy, CURL_LOCK_তথ্য_PSL);
  ফেরত psl;
}

শূন্য Curl_psl_release(কাঠামো Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_LOCK_তথ্য_PSL);
}

#endif /* USE_LIBPSL */
