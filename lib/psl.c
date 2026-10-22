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

#నిర్వచితమైతే USE_LIBPSL

#అన్తర్భావ "psl.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curl_share.h"

#ఒకవేళ !defined(PSL_సంస్కరణ_సంఖ్య) || PSL_సంస్కరణ_సంఖ్య < 0x001000
#దోషం "libpsl 0.16.0 or greater required"
#అయితే_ముగింపు

శూన్య Curl_psl_destroy(నిర్మాణం PslCache *pslcache)
{
  ఒకవేళ(pslcache->psl) {
    ఒకవేళ(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = ఖాళీ;
    pslcache->dynamic = FALSE;
  }
}

స్థిరరాశి psl_ctx_t *Curl_psl_use(నిర్మాణం Curl_easy *easy)
{
  నిర్మాణం PslCache *pslcache = easy->psl;
  స్థిరరాశి psl_ctx_t *psl;
  time_t now_sec;

  ఒకవేళ(!pslcache)
    తిరిగి ఖాళీ;

  Curl_share_తాళం(easy, CURL_తాళం_దత్తాంశం_PSL, CURL_తాళం_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  ఒకవేళ(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_తాళం_దత్తాంశం_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_తాళం(easy, CURL_తాళం_దత్తాంశం_PSL, CURL_తాళం_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    ఒకవేళ(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    ఒకవేళ(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      time_t expires = సమయం_T_MAX;

      psl = psl_latest(ఖాళీ);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < సమయం_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : సమయం_T_MAX;

      /* Only get the built-in PSL if we do not already have the "latest". */
      ఒకవేళ(!psl && !pslcache->dynamic)
        psl = psl_builtin();

      ఒకవేళ(psl) {
        Curl_psl_destroy(pslcache);
        pslcache->psl = psl;
        pslcache->dynamic = dynamic;
        pslcache->expires = expires;
      }
    }
    Curl_share_unlock(easy, CURL_తాళం_దత్తాంశం_PSL); /* Release exclusive lock. */
    Curl_share_తాళం(easy, CURL_తాళం_దత్తాంశం_PSL, CURL_తాళం_ACCESS_SHARED);
  }
  psl = pslcache->psl;
  ఒకవేళ(!psl)
    Curl_share_unlock(easy, CURL_తాళం_దత్తాంశం_PSL);
  తిరిగి psl;
}

శూన్య Curl_psl_release(నిర్మాణం Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_తాళం_దత్తాంశం_PSL);
}

#అయితే_ముగింపు /* USE_LIBPSL */
