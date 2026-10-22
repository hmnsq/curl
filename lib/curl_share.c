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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "hsts.h"
#அந்தர்பாவ "url.h"

நிலையான வெறுமை share_destroy(கட்டமைப்பு Curl_share *share)
{
  எனில்(!share)
    திரும்பு;

  எனில்(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  எனில்(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = வெற்று;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, அளவு(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(வெறுமை)
{
  கட்டமைப்பு Curl_share *share = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_share));
  எனில்(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_மொத்தம் = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    எனில்(!share->admin) {
      share_destroy(share);
      திரும்பு வெற்று;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    எனில்(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  திரும்பு share;
}

நிலையான uint32_t share_ref_inc(கட்டமைப்பு Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_மொத்தம்;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#இல்லையெனில்
  n = ++share->ref_மொத்தம்;
  share->has_been_shared = TRUE;
#endif
  திரும்பு n;
}

நிலையான uint32_t share_ref_dec(கட்டமைப்பு Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_மொத்தம்);
  n = --share->ref_மொத்தம்;
  Curl_mutex_release(&share->lock);
#இல்லையெனில்
  n = --share->ref_மொத்தம்;
#endif
  திரும்பு n;
}

நிலையான bool share_has_been_shared(கட்டமைப்பு Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#இல்லையெனில்
  was_shared = share->has_been_shared;
#endif
  திரும்பு was_shared;
}

நிலையான bool share_lock_acquire(கட்டமைப்பு Curl_share *share,
                               கட்டமைப்பு Curl_easy *data)
{
  எனில்(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

நிலையான வெறுமை share_lock_release(கட்டமைப்பு Curl_share *share,
                               கட்டமைப்பு Curl_easy *data,
                               bool locked)
{
  எனில்(locked) {
    DEBUGASSERT(share->unlockfunc);
    எனில்(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

நிலையான bool share_in_use(கட்டமைப்பு Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_மொத்தம் > 1);
  Curl_mutex_release(&share->lock);
#இல்லையெனில்
  bool locked = share_lock_acquire(share, வெற்று);
  in_use = (share->ref_மொத்தம் > 1);
  share_lock_release(share, வெற்று, locked);
#endif
  திரும்பு in_use;
}

நிலையான வெறுமை share_unlink(கட்டமைப்பு Curl_share **pshare,
                         கட்டமைப்பு Curl_easy *data,
                         bool locked)
{
  கட்டமைப்பு Curl_share *share = *pshare;
  uint32_t n;

  *pshare = வெற்று;
  n = share_ref_dec(share);
  எனில்(locked)
    share_lock_release(share, data, locked);
  எனில்(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  முழு type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  வெறுமை *ptr;
  CURLSHcode res = CURLSHE_OK;
  கட்டமைப்பு Curl_share *share = sh;

  எனில்(!GOOD_SHARE_HANDLE(share))
    திரும்பு CURLSHE_INVALID;

  எனில்(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    திரும்பு CURLSHE_IN_USE;
  }

  va_start(param, option);

  தேர்வு(option) {
  நிலை CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, முழு);

    தேர்வு(type) {
    நிலை CURL_LOCK_DATA_DNS:
      நிறுத்து;

    நிலை CURL_LOCK_DATA_COOKIE:
#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      எனில்(!share->cookies) {
        share->cookies = Curl_cookie_init();
        எனில்(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#இல்லையெனில் /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      நிறுத்து;

    நிலை CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      எனில்(!share->hsts) {
        share->hsts = Curl_hsts_init();
        எனில்(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#இல்லையெனில் /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      நிறுத்து;

    நிலை CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      எனில்(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        எனில்(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#இல்லையெனில்
      res = CURLSHE_NOT_BUILT_IN;
#endif
      நிறுத்து;

    நிலை CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      எனில்(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      நிறுத்து;

    நிலை CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      நிறுத்து;

    இயல்பு:
      res = CURLSHE_BAD_OPTION;
    }
    எனில்(!res)
      share->specifier |= (unsigned முழு)(1 << type);
    நிறுத்து;

  நிலை CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, முழு);
    தேர்வு(type) {
    நிலை CURL_LOCK_DATA_DNS:
      நிறுத்து;

    நிலை CURL_LOCK_DATA_COOKIE:
#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      எனில்(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = வெற்று;
      }
#இல்லையெனில் /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      நிறுத்து;

    நிலை CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      எனில்(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#இல்லையெனில் /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      நிறுத்து;

    நிலை CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      எனில்(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = வெற்று;
      }
#இல்லையெனில்
      res = CURLSHE_NOT_BUILT_IN;
#endif
      நிறுத்து;

    நிலை CURL_LOCK_DATA_CONNECT:
      நிறுத்து;

    இயல்பு:
      res = CURLSHE_BAD_OPTION;
      நிறுத்து;
    }
    எனில்(!res)
      share->specifier &= ~(unsigned முழு)(1 << type);
    நிறுத்து;

  நிலை CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    நிறுத்து;

  நிலை CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    நிறுத்து;

  நிலை CURLSHOPT_USERDATA:
    ptr = va_arg(param, வெறுமை *);
    share->clientdata = ptr;
    நிறுத்து;

  இயல்பு:
    res = CURLSHE_BAD_OPTION;
    நிறுத்து;
  }

  va_end(param);

  திரும்பு res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  கட்டமைப்பு Curl_share *share = sh;
  bool locked;
  எனில்(!GOOD_SHARE_HANDLE(share))
    திரும்பு CURLSHE_INVALID;

  எனில்(share_in_use(share))
    திரும்பு CURLSHE_IN_USE;

  locked = share_lock_acquire(share, வெற்று);
  share_unlink(&share, வெற்று, locked);
  திரும்பு CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(கட்டமைப்பு Curl_share *share,
                                 கட்டமைப்பு Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  எனில்(!share)
    திரும்பு CURLSHE_INVALID;

  எனில்(share->specifier & (unsigned முழு)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  திரும்பு CURLSHE_OK;
}

CURLSHcode Curl_share_lock(கட்டமைப்பு Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  திரும்பு Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(கட்டமைப்பு Curl_share *share,
                                   கட்டமைப்பு Curl_easy *data,
                                   curl_lock_data type)
{
  எனில்(!share)
    திரும்பு CURLSHE_INVALID;

  எனில்(share->specifier & (unsigned முழு)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  திரும்பு CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(கட்டமைப்பு Curl_easy *data, curl_lock_data type)
{
  திரும்பு Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_share *share = data->share;

  எனில்(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    எனில்((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      எனில்(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    எனில்(share->cookies == data->cookies)
      data->cookies = வெற்று;
#endif

#ifndef CURL_DISABLE_HSTS
    எனில்(share->hsts == data->hsts)
      data->hsts = வெற்று;
#endif
#ifdef USE_LIBPSL
    எனில்(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : வெற்று;
#endif

    share_unlink(&data->share, data, locked);
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_share_easy_link(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_share *share)
{
  எனில்(data->share) {
    DEBUGASSERT(0);
    திரும்பு CURLE_FAILED_INIT;
  }

  எனில்(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    எனில்((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    எனில்(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    எனில்(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    எனில்(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  திரும்பு CURLE_OK;
}
