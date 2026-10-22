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

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "hsts.h"
#అన్తర్భావ "url.h"

స్థిర శూన్య share_destroy(నిర్మాణం Curl_share *share)
{
  ఒకవేళ(!share)
    తిరిగి;

  ఒకవేళ(share->specifier & (1 << CURL_తాళం_దత్తాంశం_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SSL
  ఒకవేళ(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = ఖాళీ;
  }
#అయితే_ముగింపు

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#నిర్వచితమైతే USE_MUTEX
  Curl_mutex_destroy(&share->తాళం);
#అయితే_ముగింపు
  curlx_memzero(share, పరిమాణం(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(శూన్య)
{
  నిర్మాణం Curl_share *share = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_share));
  ఒకవేళ(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_తాళం_దత్తాంశం_SHARE);
#నిర్వచితమైతే USE_MUTEX
    Curl_mutex_init(&share->తాళం);
#అయితే_ముగింపు
    share->ref_లెక్క = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    ఒకవేళ(!share->admin) {
      share_destroy(share);
      తిరిగి ఖాళీ;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.అంతర్గత = TRUE;
#నిర్వచితమైతే DEBUGBUILD
    ఒకవేళ(getenv("CURL_DEBUG"))
      share->admin->సమితి.verbose = TRUE;
#అయితే_ముగింపు
  }
  తిరిగి share;
}

స్థిర uint32_t share_ref_inc(నిర్మాణం Curl_share *share)
{
  uint32_t n;
#నిర్వచితమైతే USE_MUTEX
  Curl_mutex_acquire(&share->తాళం);
  n = ++share->ref_లెక్క;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->తాళం);
#లేకపోతే
  n = ++share->ref_లెక్క;
  share->has_been_shared = TRUE;
#అయితే_ముగింపు
  తిరిగి n;
}

స్థిర uint32_t share_ref_dec(నిర్మాణం Curl_share *share)
{
  uint32_t n;
#నిర్వచితమైతే USE_MUTEX
  Curl_mutex_acquire(&share->తాళం);
  DEBUGASSERT(share->ref_లెక్క);
  n = --share->ref_లెక్క;
  Curl_mutex_release(&share->తాళం);
#లేకపోతే
  n = --share->ref_లెక్క;
#అయితే_ముగింపు
  తిరిగి n;
}

స్థిర bool share_has_been_shared(నిర్మాణం Curl_share *share)
{
  bool was_shared;
#నిర్వచితమైతే USE_MUTEX
  Curl_mutex_acquire(&share->తాళం);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->తాళం);
#లేకపోతే
  was_shared = share->has_been_shared;
#అయితే_ముగింపు
  తిరిగి was_shared;
}

స్థిర bool share_తాళం_acquire(నిర్మాణం Curl_share *share,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(share->lockfunc && share->unlockfunc &&
     (దత్తాంశం || share_has_been_shared(share))) {
    share->lockfunc(దత్తాంశం, CURL_తాళం_దత్తాంశం_SHARE, CURL_తాళం_ACCESS_SINGLE,
                    share->clientdata);
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

స్థిర శూన్య share_తాళం_release(నిర్మాణం Curl_share *share,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               bool locked)
{
  ఒకవేళ(locked) {
    DEBUGASSERT(share->unlockfunc);
    ఒకవేళ(share->unlockfunc)
      share->unlockfunc(దత్తాంశం, CURL_తాళం_దత్తాంశం_SHARE, share->clientdata);
  }
}

స్థిర bool share_లోపల_use(నిర్మాణం Curl_share *share)
{
  bool లోపల_use;
#నిర్వచితమైతే USE_MUTEX
  Curl_mutex_acquire(&share->తాళం);
  లోపల_use = (share->ref_లెక్క > 1);
  Curl_mutex_release(&share->తాళం);
#లేకపోతే
  bool locked = share_తాళం_acquire(share, ఖాళీ);
  లోపల_use = (share->ref_లెక్క > 1);
  share_తాళం_release(share, ఖాళీ, locked);
#అయితే_ముగింపు
  తిరిగి లోపల_use;
}

స్థిర శూన్య share_unlink(నిర్మాణం Curl_share **pshare,
                         నిర్మాణం Curl_easy *దత్తాంశం,
                         bool locked)
{
  నిర్మాణం Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ఖాళీ;
  n = share_ref_dec(share);
  ఒకవేళ(locked)
    share_తాళం_release(share, దత్తాంశం, locked);
  ఒకవేళ(!n)  /* last reference gone */
    share_destroy(share);
}

#నిర్వచనం_రద్దు curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  పూర్ణాంకం type;
  curl_తాళం_function lockfunc;
  curl_unlock_function unlockfunc;
  శూన్య *ptr;
  CURLSHcode res = CURLSHE_OK;
  నిర్మాణం Curl_share *share = sh;

  ఒకవేళ(!GOOD_SHARE_HANDLE(share))
    తిరిగి CURLSHE_INVALID;

  ఒకవేళ(share_లోపల_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    తిరిగి CURLSHE_లోపల_USE;
  }

  va_start(param, option);

  ఎంపిక(option) {
  స్థితి CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, పూర్ణాంకం);

    ఎంపిక(type) {
    స్థితి CURL_తాళం_దత్తాంశం_DNS:
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_COOKIE:
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ఒకవేళ(!share->cookies) {
        share->cookies = Curl_cookie_init();
        ఒకవేళ(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#లేకపోతే /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_HSTS:
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
      ఒకవేళ(!share->hsts) {
        share->hsts = Curl_hsts_init();
        ఒకవేళ(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#లేకపోతే /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_SSL_SESSION:
#నిర్వచితమైతే USE_SSL
      ఒకవేళ(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        ఒకవేళ(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#లేకపోతే
      res = CURLSHE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_CONNECT:
      /* It is safe to set this option several times on a share. */
      ఒకవేళ(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_PSL:
#నిర్వచితం_కాకపోతే USE_LIBPSL
      res = CURLSHE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
      ఆపు;

    అప్రమేయ:
      res = CURLSHE_BAD_OPTION;
    }
    ఒకవేళ(!res)
      share->specifier |= (గుర్తులేని పూర్ణాంకం)(1 << type);
    ఆపు;

  స్థితి CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, పూర్ణాంకం);
    ఎంపిక(type) {
    స్థితి CURL_తాళం_దత్తాంశం_DNS:
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_COOKIE:
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ఒకవేళ(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ఖాళీ;
      }
#లేకపోతే /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_HSTS:
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
      ఒకవేళ(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#లేకపోతే /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_SSL_SESSION:
#నిర్వచితమైతే USE_SSL
      ఒకవేళ(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ఖాళీ;
      }
#లేకపోతే
      res = CURLSHE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
      ఆపు;

    స్థితి CURL_తాళం_దత్తాంశం_CONNECT:
      ఆపు;

    అప్రమేయ:
      res = CURLSHE_BAD_OPTION;
      ఆపు;
    }
    ఒకవేళ(!res)
      share->specifier &= ~(గుర్తులేని పూర్ణాంకం)(1 << type);
    ఆపు;

  స్థితి CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_తాళం_function);
    share->lockfunc = lockfunc;
    ఆపు;

  స్థితి CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    ఆపు;

  స్థితి CURLSHOPT_USERDATA:
    ptr = va_arg(param, శూన్య *);
    share->clientdata = ptr;
    ఆపు;

  అప్రమేయ:
    res = CURLSHE_BAD_OPTION;
    ఆపు;
  }

  va_end(param);

  తిరిగి res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  నిర్మాణం Curl_share *share = sh;
  bool locked;
  ఒకవేళ(!GOOD_SHARE_HANDLE(share))
    తిరిగి CURLSHE_INVALID;

  ఒకవేళ(share_లోపల_use(share))
    తిరిగి CURLSHE_లోపల_USE;

  locked = share_తాళం_acquire(share, ఖాళీ);
  share_unlink(&share, ఖాళీ, locked);
  తిరిగి CURLSHE_OK;
}

CURLSHcode Curl_share_తాళం_share(నిర్మాణం Curl_share *share,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 curl_తాళం_దత్తాంశం type,
                                 curl_తాళం_access accesstype)
{
  ఒకవేళ(!share)
    తిరిగి CURLSHE_INVALID;

  ఒకవేళ(share->specifier & (గుర్తులేని పూర్ణాంకం)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(దత్తాంశం, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  తిరిగి CURLSHE_OK;
}

CURLSHcode Curl_share_తాళం(నిర్మాణం Curl_easy *దత్తాంశం, curl_తాళం_దత్తాంశం type,
                           curl_తాళం_access accesstype)
{
  తిరిగి Curl_share_తాళం_share(దత్తాంశం->share, దత్తాంశం, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(నిర్మాణం Curl_share *share,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   curl_తాళం_దత్తాంశం type)
{
  ఒకవేళ(!share)
    తిరిగి CURLSHE_INVALID;

  ఒకవేళ(share->specifier & (గుర్తులేని పూర్ణాంకం)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(దత్తాంశం, type, share->clientdata);

  తిరిగి CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(నిర్మాణం Curl_easy *దత్తాంశం, curl_తాళం_దత్తాంశం type)
{
  తిరిగి Curl_share_unlock_share(దత్తాంశం->share, దత్తాంశం, type);
}

CURLcode Curl_share_easy_unlink(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_share *share = దత్తాంశం->share;

  ఒకవేళ(share) {
    bool locked = share_తాళం_acquire(share, దత్తాంశం);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    ఒకవేళ((share->specifier & (1 << CURL_తాళం_దత్తాంశం_CONNECT))) {
      ఒకవేళ(దత్తాంశం->conn)
        Curl_detach_connection(దత్తాంశం);
      దత్తాంశం->state.చివరి_conn_id = -1;
      దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
    }

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ఒకవేళ(share->cookies == దత్తాంశం->cookies)
      దత్తాంశం->cookies = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
    ఒకవేళ(share->hsts == దత్తాంశం->hsts)
      దత్తాంశం->hsts = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBPSL
    ఒకవేళ(&share->psl == దత్తాంశం->psl)
      దత్తాంశం->psl = దత్తాంశం->multi ? &దత్తాంశం->multi->psl : ఖాళీ;
#అయితే_ముగింపు

    share_unlink(&దత్తాంశం->share, దత్తాంశం, locked);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_share_easy_link(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_share *share)
{
  ఒకవేళ(దత్తాంశం->share) {
    DEBUGASSERT(0);
    తిరిగి CURLE_FAILED_INIT;
  }

  ఒకవేళ(share) {
    bool locked = share_తాళం_acquire(share, దత్తాంశం);

    /* Forget identifiers from the previously used connection cache. */
    ఒకవేళ((share->specifier & (1 << CURL_తాళం_దత్తాంశం_CONNECT))) {
      దత్తాంశం->state.చివరి_conn_id = -1;
      దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    దత్తాంశం->share = share;

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ఒకవేళ(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(దత్తాంశం->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      దత్తాంశం->cookies = share->cookies;
    }
#అయితే_ముగింపు /* CURL_DISABLE_HTTP */
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
    ఒకవేళ(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&దత్తాంశం->hsts);
      దత్తాంశం->hsts = share->hsts;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBPSL
    ఒకవేళ(share->specifier & (1 << CURL_తాళం_దత్తాంశం_PSL))
      దత్తాంశం->psl = &share->psl;
#అయితే_ముగింపు

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_తాళం_release(share, దత్తాంశం, locked);
  }
  తిరిగి CURLE_OK;
}
