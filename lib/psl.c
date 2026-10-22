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

#ifdef USE_LIBPSL

#अन्तर्भाव "psl.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curl_share.h"

#यदि !defined(PSL_VERSION_संख्या) || PSL_VERSION_संख्या < 0x001000
#error "libpsl 0.16.0 or greater required"
#endif

शून्य Curl_psl_destroy(संरचना PslCache *pslcache)
{
  यदि(pslcache->psl) {
    यदि(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = रिक्त;
    pslcache->dynamic = FALSE;
  }
}

ध्रुव psl_ctx_t *Curl_psl_use(संरचना Curl_easy *easy)
{
  संरचना PslCache *pslcache = easy->psl;
  ध्रुव psl_ctx_t *psl;
  समय_प्रकार now_sec;

  यदि(!pslcache)
    प्रत्यागम रिक्त;

  Curl_share_lock(easy, CURL_LOCK_DATA_PSL, CURL_LOCK_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  यदि(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_DATA_PSL, CURL_LOCK_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    यदि(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    यदि(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      समय_प्रकार expires = TIME_T_MAX;

      psl = psl_latest(रिक्त);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < TIME_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : TIME_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      यदि(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      यदि(psl) {
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
  यदि(!psl)
    Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);
  प्रत्यागम psl;
}

शून्य Curl_psl_release(संरचना Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_LOCK_DATA_PSL);
}

#endif /* USE_LIBPSL */
