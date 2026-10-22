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

#ضمن "urldata.h"
#ضمن "multiif.h"
#ضمن "curl_threads.h"
#ضمن "curl_share.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "hsts.h"
#ضمن "url.h"

ساكن فراغ share_destroy(بنية Curl_share *share)
{
  אם(!share)
    החזר;

  אם(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  אם(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = عدم;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, حجم(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(فراغ)
{
  بنية Curl_share *share = curlx_calloc(1, حجم(بنية Curl_share));
  אם(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_count = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    אם(!share->admin) {
      share_destroy(share);
      החזר عدم;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    אם(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  החזר share;
}

ساكن uint32_t share_ref_inc(بنية Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#אחרת
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
#endif
  החזר n;
}

ساكن uint32_t share_ref_dec(بنية Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_count);
  n = --share->ref_count;
  Curl_mutex_release(&share->lock);
#אחרת
  n = --share->ref_count;
#endif
  החזר n;
}

ساكن bool share_has_been_shared(بنية Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#אחרת
  was_shared = share->has_been_shared;
#endif
  החזר was_shared;
}

ساكن bool share_lock_acquire(بنية Curl_share *share,
                               بنية Curl_easy *data)
{
  אם(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    החזר TRUE;
  }
  החזר FALSE;
}

ساكن فراغ share_lock_release(بنية Curl_share *share,
                               بنية Curl_easy *data,
                               bool locked)
{
  אם(locked) {
    DEBUGASSERT(share->unlockfunc);
    אם(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

ساكن bool share_in_use(بنية Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_count > 1);
  Curl_mutex_release(&share->lock);
#אחרת
  bool locked = share_lock_acquire(share, عدم);
  in_use = (share->ref_count > 1);
  share_lock_release(share, عدم, locked);
#endif
  החזר in_use;
}

ساكن فراغ share_unlink(بنية Curl_share **pshare,
                         بنية Curl_easy *data,
                         bool locked)
{
  بنية Curl_share *share = *pshare;
  uint32_t n;

  *pshare = عدم;
  n = share_ref_dec(share);
  אם(locked)
    share_lock_release(share, data, locked);
  אם(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  صحيح type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  فراغ *ptr;
  CURLSHcode res = CURLSHE_OK;
  بنية Curl_share *share = sh;

  אם(!GOOD_SHARE_HANDLE(share))
    החזר CURLSHE_INVALID;

  אם(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    החזר CURLSHE_IN_USE;
  }

  va_start(param, option);

  بدل(option) {
  حالة CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, صحيح);

    بدل(type) {
    حالة CURL_LOCK_DATA_DNS:
      اكسر;

    حالة CURL_LOCK_DATA_COOKIE:
#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      אם(!share->cookies) {
        share->cookies = Curl_cookie_init();
        אם(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#אחרת /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      اكسر;

    حالة CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      אם(!share->hsts) {
        share->hsts = Curl_hsts_init();
        אם(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#אחרת /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      اكسر;

    حالة CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      אם(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        אם(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#אחרת
      res = CURLSHE_NOT_BUILT_IN;
#endif
      اكسر;

    حالة CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      אם(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      اكسر;

    حالة CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
    }
    אם(!res)
      share->specifier |= (unsigned صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, صحيح);
    بدل(type) {
    حالة CURL_LOCK_DATA_DNS:
      اكسر;

    حالة CURL_LOCK_DATA_COOKIE:
#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      אם(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = عدم;
      }
#אחרת /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      اكسر;

    حالة CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      אם(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#אחרת /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      اكسر;

    حالة CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      אם(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = عدم;
      }
#אחרת
      res = CURLSHE_NOT_BUILT_IN;
#endif
      اكسر;

    حالة CURL_LOCK_DATA_CONNECT:
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
      اكسر;
    }
    אם(!res)
      share->specifier &= ~(unsigned صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    اكسر;

  حالة CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    اكسر;

  حالة CURLSHOPT_USERDATA:
    ptr = va_arg(param, فراغ *);
    share->clientdata = ptr;
    اكسر;

  افتراضي:
    res = CURLSHE_BAD_OPTION;
    اكسر;
  }

  va_end(param);

  החזר res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  بنية Curl_share *share = sh;
  bool locked;
  אם(!GOOD_SHARE_HANDLE(share))
    החזר CURLSHE_INVALID;

  אם(share_in_use(share))
    החזר CURLSHE_IN_USE;

  locked = share_lock_acquire(share, عدم);
  share_unlink(&share, عدم, locked);
  החזר CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(بنية Curl_share *share,
                                 بنية Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  אם(!share)
    החזר CURLSHE_INVALID;

  אם(share->specifier & (unsigned صحيح)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  החזר CURLSHE_OK;
}

CURLSHcode Curl_share_lock(بنية Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  החזר Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(بنية Curl_share *share,
                                   بنية Curl_easy *data,
                                   curl_lock_data type)
{
  אם(!share)
    החזר CURLSHE_INVALID;

  אם(share->specifier & (unsigned صحيح)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  החזר CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(بنية Curl_easy *data, curl_lock_data type)
{
  החזר Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(بنية Curl_easy *data)
{
  بنية Curl_share *share = data->share;

  אם(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    אם((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      אם(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    אם(share->cookies == data->cookies)
      data->cookies = عدم;
#endif

#ifndef CURL_DISABLE_HSTS
    אם(share->hsts == data->hsts)
      data->hsts = عدم;
#endif
#ifdef USE_LIBPSL
    אם(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : عدم;
#endif

    share_unlink(&data->share, data, locked);
  }
  החזר CURLE_OK;
}

CURLcode Curl_share_easy_link(بنية Curl_easy *data,
                              بنية Curl_share *share)
{
  אם(data->share) {
    DEBUGASSERT(0);
    החזר CURLE_FAILED_INIT;
  }

  אם(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    אם((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    אם(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    אם(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    אם(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  החזר CURLE_OK;
}
