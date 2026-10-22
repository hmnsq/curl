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

#اگر_تعریف_شدہ USE_LIBPSL

#شامل "psl.h"
#شامل "progress.h"
#شامل "curl_share.h"

#اگر !defined(PSL_نسخہ_نمبر) || PSL_نسخہ_نمبر < 0x001000
#غلطی "libpsl 0.16.0 or greater required"
#اگر_ختم

خالی Curl_psl_destroy(ساخت PslCache *pslcache)
{
  اگر(pslcache->psl) {
    اگر(pslcache->dynamic)
      psl_free((psl_ctx_t *)CURL_UNCONST(pslcache->psl));
    pslcache->psl = تہی;
    pslcache->dynamic = FALSE;
  }
}

ثابت psl_ctx_t *Curl_psl_use(ساخت Curl_easy *easy)
{
  ساخت PslCache *pslcache = easy->psl;
  ثابت psl_ctx_t *psl;
  time_t now_sec;

  اگر(!pslcache)
    واپس تہی;

  Curl_share_تالا(easy, CURL_تالا_ڈیٹا_PSL, CURL_تالا_ACCESS_SHARED);
  now_sec = Curl_pgrs_now(easy)->tv_sec;
  اگر(!pslcache->psl || pslcache->expires <= now_sec) {
    /* Release the shared lock so another thread can refresh the cache and
       avoid deadlock. */
    Curl_share_unlock(easy, CURL_تالا_ڈیٹا_PSL);

    /* Update cache: this needs an exclusive lock. */
    Curl_share_تالا(easy, CURL_تالا_ڈیٹا_PSL, CURL_تالا_ACCESS_SINGLE);

    /* Recheck in case another thread did the job. */
    اگر(pslcache->expires <= now_sec) {
      now_sec = Curl_pgrs_now(easy)->tv_sec;
    }
    اگر(!pslcache->psl || pslcache->expires <= now_sec) {
      bool dynamic = FALSE;
      time_t expires = وقت_T_MAX;

      psl = psl_latest(تہی);
      dynamic = !!psl;
      /* Take care of possible time computation overflow. */
      expires = (now_sec < وقت_T_MAX - PSL_TTL) ?
                (now_sec + PSL_TTL) : وقت_T_MAX;

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
    Curl_share_unlock(easy, CURL_تالا_ڈیٹا_PSL); /* Release exclusive lock. */
    Curl_share_تالا(easy, CURL_تالا_ڈیٹا_PSL, CURL_تالا_ACCESS_SHARED);
  }
  psl = pslcache->psl;
  اگر(!psl)
    Curl_share_unlock(easy, CURL_تالا_ڈیٹا_PSL);
  واپس psl;
}

خالی Curl_psl_release(ساخت Curl_easy *easy)
{
  Curl_share_unlock(easy, CURL_تالا_ڈیٹا_PSL);
}

#اگر_ختم /* USE_LIBPSL */
