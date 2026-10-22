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

#अगर_घोषित USE_LIBPSL

#समावेश "psl.h"
#समावेश "progress.h"
#समावेश "curl_share.h"

#अगर !defined(PSL_VERSION_संख्या) || PSL_VERSION_संख्या < 0x001000
#त्रुटि "libpsl 0.16.0 or greater required"
#पूर्ण_अगर

व्योम Curl_psl_destroy(काष्ठा PslCache *pslcache)
{
  अगर(pslcache->psl) {
    अगर(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = शून्य;
    pslcache->dynamic = FALSE;
  }
}

स्थिर psl_ctx_t *Curl_psl_use(काष्ठा Curl_easy *easy)
{
  काष्ठा PslCache *pslcache = easy->psl;
  स्थिर psl_ctx_t *psl;
  समय_प्रकार now_sec;

  अगर(!pslcache)
    वापस शून्य;

  Curl_share_lock(easy, CURL_LOCK_तथ्य_PSL, CURL_LOCK_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  अगर(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_LOCK_तथ्य_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_तथ्य_PSL, CURL_LOCK_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    अगर(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    अगर(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      समय_प्रकार expires = TIME_T_MAX;

      psl = psl_latest(शून्य);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < TIME_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : TIME_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      अगर(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      अगर(psl) {
        Curl_psl_destroy(pslcache);
        pslcache->psl = psl;
        pslcache->dynamic = dynamic;
        pslcache->expires = expires;
      }
    }
    Curl_share_unlock(easy, CURL_LOCK_तथ्य_PSL); /* Release exclusive lock. */
    Curl_share_lock(easy, CURL_LOCK_तथ्य_PSL, CURL_LOCK_ACCESS_SHARED);
  }
  psl = pslcache->psl;
  अगर(!psl)
    Curl_share_unlock(easy, CURL_LOCK_तथ्य_PSL);
  वापस psl;
}

व्योम Curl_psl_release(काष्ठा Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_LOCK_तथ्य_PSL);
}

#पूर्ण_अगर /* USE_LIBPSL */
