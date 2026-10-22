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

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hsts.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"

ਸਥਿਰ ਖਾਲੀ share_destroy(ਬਣਤਰ Curl_share *share)
{
  ਜੇ(!share)
    ਵਾਪਸ;

  ਜੇ(share->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਜੇ(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = ਨਿਰਾ;
  }
#ਜੇ_ਅੰਤ

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  Curl_mutex_destroy(&share->ਤਾਲਾ);
#ਜੇ_ਅੰਤ
  curlx_memzero(share, ਆਕਾਰ(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(ਖਾਲੀ)
{
  ਬਣਤਰ Curl_share *share = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_share));
  ਜੇ(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_SHARE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
    Curl_mutex_init(&share->ਤਾਲਾ);
#ਜੇ_ਅੰਤ
    share->ref_ਗਿਣਤੀ = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    ਜੇ(!share->admin) {
      share_destroy(share);
      ਵਾਪਸ ਨਿਰਾ;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.ਅੰਦਰੂਨੀ = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਜੇ(getenv("CURL_DEBUG"))
      share->admin->ਸੈੱਟ.verbose = TRUE;
#ਜੇ_ਅੰਤ
  }
  ਵਾਪਸ share;
}

ਸਥਿਰ uint32_t share_ref_inc(ਬਣਤਰ Curl_share *share)
{
  uint32_t n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  Curl_mutex_acquire(&share->ਤਾਲਾ);
  n = ++share->ref_ਗਿਣਤੀ;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->ਤਾਲਾ);
#ਨਹੀਂਤਾਂ
  n = ++share->ref_ਗਿਣਤੀ;
  share->has_been_shared = TRUE;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ n;
}

ਸਥਿਰ uint32_t share_ref_dec(ਬਣਤਰ Curl_share *share)
{
  uint32_t n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  Curl_mutex_acquire(&share->ਤਾਲਾ);
  DEBUGASSERT(share->ref_ਗਿਣਤੀ);
  n = --share->ref_ਗਿਣਤੀ;
  Curl_mutex_release(&share->ਤਾਲਾ);
#ਨਹੀਂਤਾਂ
  n = --share->ref_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ n;
}

ਸਥਿਰ bool share_has_been_shared(ਬਣਤਰ Curl_share *share)
{
  bool was_shared;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  Curl_mutex_acquire(&share->ਤਾਲਾ);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->ਤਾਲਾ);
#ਨਹੀਂਤਾਂ
  was_shared = share->has_been_shared;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ was_shared;
}

ਸਥਿਰ bool share_ਤਾਲਾ_acquire(ਬਣਤਰ Curl_share *share,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(share->lockfunc && share->unlockfunc &&
     (ਅੰਕੜੇ || share_has_been_shared(share))) {
    share->lockfunc(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_SHARE, CURL_ਤਾਲਾ_ACCESS_SINGLE,
                    share->clientdata);
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਖਾਲੀ share_ਤਾਲਾ_release(ਬਣਤਰ Curl_share *share,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool locked)
{
  ਜੇ(locked) {
    DEBUGASSERT(share->unlockfunc);
    ਜੇ(share->unlockfunc)
      share->unlockfunc(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_SHARE, share->clientdata);
  }
}

ਸਥਿਰ bool share_ਵਿੱਚ_use(ਬਣਤਰ Curl_share *share)
{
  bool ਵਿੱਚ_use;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  Curl_mutex_acquire(&share->ਤਾਲਾ);
  ਵਿੱਚ_use = (share->ref_ਗਿਣਤੀ > 1);
  Curl_mutex_release(&share->ਤਾਲਾ);
#ਨਹੀਂਤਾਂ
  bool locked = share_ਤਾਲਾ_acquire(share, ਨਿਰਾ);
  ਵਿੱਚ_use = (share->ref_ਗਿਣਤੀ > 1);
  share_ਤਾਲਾ_release(share, ਨਿਰਾ, locked);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਵਿੱਚ_use;
}

ਸਥਿਰ ਖਾਲੀ share_unlink(ਬਣਤਰ Curl_share **pshare,
                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         bool locked)
{
  ਬਣਤਰ Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ਨਿਰਾ;
  n = share_ref_dec(share);
  ਜੇ(locked)
    share_ਤਾਲਾ_release(share, ਅੰਕੜੇ, locked);
  ਜੇ(!n)  /* last reference gone */
    share_destroy(share);
}

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  ਪੂਰਨ type;
  curl_ਤਾਲਾ_function lockfunc;
  curl_unlock_function unlockfunc;
  ਖਾਲੀ *ptr;
  CURLSHcode res = CURLSHE_OK;
  ਬਣਤਰ Curl_share *share = sh;

  ਜੇ(!GOOD_SHARE_HANDLE(share))
    ਵਾਪਸ CURLSHE_INVALID;

  ਜੇ(share_ਵਿੱਚ_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    ਵਾਪਸ CURLSHE_ਵਿੱਚ_USE;
  }

  va_start(param, option);

  ਚੋਣ(option) {
  ਹਾਲਤ CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, ਪੂਰਨ);

    ਚੋਣ(type) {
    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_DNS:
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE:
#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ਜੇ(!share->cookies) {
        share->cookies = Curl_cookie_init();
        ਜੇ(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#ਨਹੀਂਤਾਂ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_HSTS:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
      ਜੇ(!share->hsts) {
        share->hsts = Curl_hsts_init();
        ਜੇ(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#ਨਹੀਂਤਾਂ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_SSL_SESSION:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
      ਜੇ(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        ਜੇ(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#ਨਹੀਂਤਾਂ
      res = CURLSHE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT:
      /* It is safe to set this option several times on a share. */
      ਜੇ(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
      res = CURLSHE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;

    ਮੂਲ:
      res = CURLSHE_BAD_OPTION;
    }
    ਜੇ(!res)
      share->specifier |= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(1 << type);
    ਰੋਕੋ;

  ਹਾਲਤ CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, ਪੂਰਨ);
    ਚੋਣ(type) {
    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_DNS:
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_COOKIE:
#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ਜੇ(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ਨਿਰਾ;
      }
#ਨਹੀਂਤਾਂ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_HSTS:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
      ਜੇ(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#ਨਹੀਂਤਾਂ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_SSL_SESSION:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
      ਜੇ(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ਨਿਰਾ;
      }
#ਨਹੀਂਤਾਂ
      res = CURLSHE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;

    ਹਾਲਤ CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT:
      ਰੋਕੋ;

    ਮੂਲ:
      res = CURLSHE_BAD_OPTION;
      ਰੋਕੋ;
    }
    ਜੇ(!res)
      share->specifier &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(1 << type);
    ਰੋਕੋ;

  ਹਾਲਤ CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_ਤਾਲਾ_function);
    share->lockfunc = lockfunc;
    ਰੋਕੋ;

  ਹਾਲਤ CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    ਰੋਕੋ;

  ਹਾਲਤ CURLSHOPT_USERDATA:
    ptr = va_arg(param, ਖਾਲੀ *);
    share->clientdata = ptr;
    ਰੋਕੋ;

  ਮੂਲ:
    res = CURLSHE_BAD_OPTION;
    ਰੋਕੋ;
  }

  va_end(param);

  ਵਾਪਸ res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  ਬਣਤਰ Curl_share *share = sh;
  bool locked;
  ਜੇ(!GOOD_SHARE_HANDLE(share))
    ਵਾਪਸ CURLSHE_INVALID;

  ਜੇ(share_ਵਿੱਚ_use(share))
    ਵਾਪਸ CURLSHE_ਵਿੱਚ_USE;

  locked = share_ਤਾਲਾ_acquire(share, ਨਿਰਾ);
  share_unlink(&share, ਨਿਰਾ, locked);
  ਵਾਪਸ CURLSHE_OK;
}

CURLSHcode Curl_share_ਤਾਲਾ_share(ਬਣਤਰ Curl_share *share,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 curl_ਤਾਲਾ_ਅੰਕੜੇ type,
                                 curl_ਤਾਲਾ_access accesstype)
{
  ਜੇ(!share)
    ਵਾਪਸ CURLSHE_INVALID;

  ਜੇ(share->specifier & (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(ਅੰਕੜੇ, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  ਵਾਪਸ CURLSHE_OK;
}

CURLSHcode Curl_share_ਤਾਲਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_ਤਾਲਾ_ਅੰਕੜੇ type,
                           curl_ਤਾਲਾ_access accesstype)
{
  ਵਾਪਸ Curl_share_ਤਾਲਾ_share(ਅੰਕੜੇ->share, ਅੰਕੜੇ, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(ਬਣਤਰ Curl_share *share,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   curl_ਤਾਲਾ_ਅੰਕੜੇ type)
{
  ਜੇ(!share)
    ਵਾਪਸ CURLSHE_INVALID;

  ਜੇ(share->specifier & (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(ਅੰਕੜੇ, type, share->clientdata);

  ਵਾਪਸ CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_ਤਾਲਾ_ਅੰਕੜੇ type)
{
  ਵਾਪਸ Curl_share_unlock_share(ਅੰਕੜੇ->share, ਅੰਕੜੇ, type);
}

CURLcode Curl_share_easy_unlink(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_share *share = ਅੰਕੜੇ->share;

  ਜੇ(share) {
    bool locked = share_ਤਾਲਾ_acquire(share, ਅੰਕੜੇ);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    ਜੇ((share->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT))) {
      ਜੇ(ਅੰਕੜੇ->conn)
        Curl_detach_connection(ਅੰਕੜੇ);
      ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
      ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
    }

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ਜੇ(share->cookies == ਅੰਕੜੇ->cookies)
      ਅੰਕੜੇ->cookies = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
    ਜੇ(share->hsts == ਅੰਕੜੇ->hsts)
      ਅੰਕੜੇ->hsts = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
    ਜੇ(&share->psl == ਅੰਕੜੇ->psl)
      ਅੰਕੜੇ->psl = ਅੰਕੜੇ->multi ? &ਅੰਕੜੇ->multi->psl : ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    share_unlink(&ਅੰਕੜੇ->share, ਅੰਕੜੇ, locked);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_share_easy_link(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_share *share)
{
  ਜੇ(ਅੰਕੜੇ->share) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਜੇ(share) {
    bool locked = share_ਤਾਲਾ_acquire(share, ਅੰਕੜੇ);

    /* Forget identifiers from the previously used connection cache. */
    ਜੇ((share->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_CONNECT))) {
      ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
      ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    ਅੰਕੜੇ->share = share;

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ਜੇ(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(ਅੰਕੜੇ->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      ਅੰਕੜੇ->cookies = share->cookies;
    }
#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
    ਜੇ(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&ਅੰਕੜੇ->hsts);
      ਅੰਕੜੇ->hsts = share->hsts;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
    ਜੇ(share->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL))
      ਅੰਕੜੇ->psl = &share->psl;
#ਜੇ_ਅੰਤ

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_ਤਾਲਾ_release(share, ਅੰਕੜੇ, locked);
  }
  ਵਾਪਸ CURLE_OK;
}
