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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "url.h"

ಸ್ಥಿರ ಶೂನ್ಯ share_destroy(ಸಂರಚನಾ Curl_share *share)
{
  ಯದಿ(!share)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  ಯದಿ(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = ರಿಕ್ತ;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, ಪರಿಮಾಣ(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ Curl_share *share = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_share));
  ಯದಿ(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_ಎಣಿಕೆ = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    ಯದಿ(!share->admin) {
      share_destroy(share);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    ಯದಿ(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  ಪ್ರತ್ಯಾಗಮ share;
}

ಸ್ಥಿರ uint32_t share_ref_inc(ಸಂರಚನಾ Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_ಎಣಿಕೆ;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#ಅನ್ಯಥಾ
  n = ++share->ref_ಎಣಿಕೆ;
  share->has_been_shared = TRUE;
#endif
  ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ uint32_t share_ref_dec(ಸಂರಚನಾ Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_ಎಣಿಕೆ);
  n = --share->ref_ಎಣಿಕೆ;
  Curl_mutex_release(&share->lock);
#ಅನ್ಯಥಾ
  n = --share->ref_ಎಣಿಕೆ;
#endif
  ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ bool share_has_been_shared(ಸಂರಚನಾ Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#ಅನ್ಯಥಾ
  was_shared = share->has_been_shared;
#endif
  ಪ್ರತ್ಯಾಗಮ was_shared;
}

ಸ್ಥಿರ bool share_lock_acquire(ಸಂರಚನಾ Curl_share *share,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ share_lock_release(ಸಂರಚನಾ Curl_share *share,
                               ಸಂರಚನಾ Curl_easy *data,
                               bool locked)
{
  ಯದಿ(locked) {
    DEBUGASSERT(share->unlockfunc);
    ಯದಿ(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

ಸ್ಥಿರ bool share_in_use(ಸಂರಚನಾ Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_ಎಣಿಕೆ > 1);
  Curl_mutex_release(&share->lock);
#ಅನ್ಯಥಾ
  bool locked = share_lock_acquire(share, ರಿಕ್ತ);
  in_use = (share->ref_ಎಣಿಕೆ > 1);
  share_lock_release(share, ರಿಕ್ತ, locked);
#endif
  ಪ್ರತ್ಯಾಗಮ in_use;
}

ಸ್ಥಿರ ಶೂನ್ಯ share_unlink(ಸಂರಚನಾ Curl_share **pshare,
                         ಸಂರಚನಾ Curl_easy *data,
                         bool locked)
{
  ಸಂರಚನಾ Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ರಿಕ್ತ;
  n = share_ref_dec(share);
  ಯದಿ(locked)
    share_lock_release(share, data, locked);
  ಯದಿ(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  ಪೂರ್ಣಾಙ್ಕ type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  ಶೂನ್ಯ *ptr;
  CURLSHcode res = CURLSHE_OK;
  ಸಂರಚನಾ Curl_share *share = sh;

  ಯದಿ(!GOOD_SHARE_HANDLE(share))
    ಪ್ರತ್ಯಾಗಮ CURLSHE_INVALID;

  ಯದಿ(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    ಪ್ರತ್ಯಾಗಮ CURLSHE_IN_USE;
  }

  va_start(param, option);

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, ಪೂರ್ಣಾಙ್ಕ);

    ವಿಕಲ್ಪ(type) {
    ಪಕ್ಷ CURL_LOCK_DATA_DNS:
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_COOKIE:
#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ಯದಿ(!share->cookies) {
        share->cookies = Curl_cookie_init();
        ಯದಿ(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#ಅನ್ಯಥಾ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ಯದಿ(!share->hsts) {
        share->hsts = Curl_hsts_init();
        ಯದಿ(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#ಅನ್ಯಥಾ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ಯದಿ(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        ಯದಿ(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#ಅನ್ಯಥಾ
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      ಯದಿ(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      res = CURLSHE_BAD_OPTION;
    }
    ಯದಿ(!res)
      share->specifier |= (unsigned ಪೂರ್ಣಾಙ್ಕ)(1 << type);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, ಪೂರ್ಣಾಙ್ಕ);
    ವಿಕಲ್ಪ(type) {
    ಪಕ್ಷ CURL_LOCK_DATA_DNS:
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_COOKIE:
#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ಯದಿ(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ರಿಕ್ತ;
      }
#ಅನ್ಯಥಾ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ಯದಿ(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#ಅನ್ಯಥಾ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ಯದಿ(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ರಿಕ್ತ;
      }
#ಅನ್ಯಥಾ
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_LOCK_DATA_CONNECT:
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      res = CURLSHE_BAD_OPTION;
      ಭಙ್ಗ;
    }
    ಯದಿ(!res)
      share->specifier &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)(1 << type);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    ಭಙ್ಗ;

  ಪಕ್ಷ CURLSHOPT_USERDATA:
    ptr = va_arg(param, ಶೂನ್ಯ *);
    share->clientdata = ptr;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    res = CURLSHE_BAD_OPTION;
    ಭಙ್ಗ;
  }

  va_end(param);

  ಪ್ರತ್ಯಾಗಮ res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  ಸಂರಚನಾ Curl_share *share = sh;
  bool locked;
  ಯದಿ(!GOOD_SHARE_HANDLE(share))
    ಪ್ರತ್ಯಾಗಮ CURLSHE_INVALID;

  ಯದಿ(share_in_use(share))
    ಪ್ರತ್ಯಾಗಮ CURLSHE_IN_USE;

  locked = share_lock_acquire(share, ರಿಕ್ತ);
  share_unlink(&share, ರಿಕ್ತ, locked);
  ಪ್ರತ್ಯಾಗಮ CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(ಸಂರಚನಾ Curl_share *share,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  ಯದಿ(!share)
    ಪ್ರತ್ಯಾಗಮ CURLSHE_INVALID;

  ಯದಿ(share->specifier & (unsigned ಪೂರ್ಣಾಙ್ಕ)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  ಪ್ರತ್ಯಾಗಮ CURLSHE_OK;
}

CURLSHcode Curl_share_lock(ಸಂರಚನಾ Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  ಪ್ರತ್ಯಾಗಮ Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(ಸಂರಚನಾ Curl_share *share,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   curl_lock_data type)
{
  ಯದಿ(!share)
    ಪ್ರತ್ಯಾಗಮ CURLSHE_INVALID;

  ಯದಿ(share->specifier & (unsigned ಪೂರ್ಣಾಙ್ಕ)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  ಪ್ರತ್ಯಾಗಮ CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(ಸಂರಚನಾ Curl_easy *data, curl_lock_data type)
{
  ಪ್ರತ್ಯಾಗಮ Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_share *share = data->share;

  ಯದಿ(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    ಯದಿ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      ಯದಿ(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ಯದಿ(share->cookies == data->cookies)
      data->cookies = ರಿಕ್ತ;
#endif

#ifndef CURL_DISABLE_HSTS
    ಯದಿ(share->hsts == data->hsts)
      data->hsts = ರಿಕ್ತ;
#endif
#ifdef USE_LIBPSL
    ಯದಿ(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : ರಿಕ್ತ;
#endif

    share_unlink(&data->share, data, locked);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_share_easy_link(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_share *share)
{
  ಯದಿ(data->share) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ಯದಿ(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    ಯದಿ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ಯದಿ(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    ಯದಿ(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    ಯದಿ(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
