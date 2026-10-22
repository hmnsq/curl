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

#شامل "urldata.h"
#شامل "multiif.h"
#شامل "curl_threads.h"
#شامل "curl_share.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_scache.h"
#شامل "hsts.h"
#شامل "url.h"

ایستا تهی share_destroy(ساختار Curl_share *share)
{
  اگر(!share)
    بازگشت;

  اگر(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  اگر(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = پوچ;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, اندازه(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(تهی)
{
  ساختار Curl_share *share = curlx_calloc(1, اندازه(ساختار Curl_share));
  اگر(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_شمار = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    اگر(!share->admin) {
      share_destroy(share);
      بازگشت پوچ;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    اگر(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  بازگشت share;
}

ایستا uint32_t share_ref_inc(ساختار Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_شمار;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#وگرنه
  n = ++share->ref_شمار;
  share->has_been_shared = TRUE;
#endif
  بازگشت n;
}

ایستا uint32_t share_ref_dec(ساختار Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_شمار);
  n = --share->ref_شمار;
  Curl_mutex_release(&share->lock);
#وگرنه
  n = --share->ref_شمار;
#endif
  بازگشت n;
}

ایستا bool share_has_been_shared(ساختار Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#وگرنه
  was_shared = share->has_been_shared;
#endif
  بازگشت was_shared;
}

ایستا bool share_lock_acquire(ساختار Curl_share *share,
                               ساختار Curl_easy *data)
{
  اگر(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

ایستا تهی share_lock_release(ساختار Curl_share *share,
                               ساختار Curl_easy *data,
                               bool locked)
{
  اگر(locked) {
    DEBUGASSERT(share->unlockfunc);
    اگر(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

ایستا bool share_in_use(ساختار Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_شمار > 1);
  Curl_mutex_release(&share->lock);
#وگرنه
  bool locked = share_lock_acquire(share, پوچ);
  in_use = (share->ref_شمار > 1);
  share_lock_release(share, پوچ, locked);
#endif
  بازگشت in_use;
}

ایستا تهی share_unlink(ساختار Curl_share **pshare,
                         ساختار Curl_easy *data,
                         bool locked)
{
  ساختار Curl_share *share = *pshare;
  uint32_t n;

  *pshare = پوچ;
  n = share_ref_dec(share);
  اگر(locked)
    share_lock_release(share, data, locked);
  اگر(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  صحیح type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  تهی *ptr;
  CURLSHcode res = CURLSHE_OK;
  ساختار Curl_share *share = sh;

  اگر(!GOOD_SHARE_HANDLE(share))
    بازگشت CURLSHE_INVALID;

  اگر(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    بازگشت CURLSHE_IN_USE;
  }

  va_start(param, option);

  انتخاب(option) {
  حالت CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, صحیح);

    انتخاب(type) {
    حالت CURL_LOCK_DATA_DNS:
      بشکن;

    حالت CURL_LOCK_DATA_COOKIE:
#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      اگر(!share->cookies) {
        share->cookies = Curl_cookie_init();
        اگر(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#وگرنه /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      بشکن;

    حالت CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      اگر(!share->hsts) {
        share->hsts = Curl_hsts_init();
        اگر(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#وگرنه /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      بشکن;

    حالت CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      اگر(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        اگر(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#وگرنه
      res = CURLSHE_NOT_BUILT_IN;
#endif
      بشکن;

    حالت CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      اگر(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      بشکن;

    حالت CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      بشکن;

    پیشفرض:
      res = CURLSHE_BAD_OPTION;
    }
    اگر(!res)
      share->specifier |= (unsigned صحیح)(1 << type);
    بشکن;

  حالت CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, صحیح);
    انتخاب(type) {
    حالت CURL_LOCK_DATA_DNS:
      بشکن;

    حالت CURL_LOCK_DATA_COOKIE:
#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      اگر(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = پوچ;
      }
#وگرنه /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      بشکن;

    حالت CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      اگر(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#وگرنه /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      بشکن;

    حالت CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      اگر(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = پوچ;
      }
#وگرنه
      res = CURLSHE_NOT_BUILT_IN;
#endif
      بشکن;

    حالت CURL_LOCK_DATA_CONNECT:
      بشکن;

    پیشفرض:
      res = CURLSHE_BAD_OPTION;
      بشکن;
    }
    اگر(!res)
      share->specifier &= ~(unsigned صحیح)(1 << type);
    بشکن;

  حالت CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    بشکن;

  حالت CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    بشکن;

  حالت CURLSHOPT_USERDATA:
    ptr = va_arg(param, تهی *);
    share->clientdata = ptr;
    بشکن;

  پیشفرض:
    res = CURLSHE_BAD_OPTION;
    بشکن;
  }

  va_end(param);

  بازگشت res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  ساختار Curl_share *share = sh;
  bool locked;
  اگر(!GOOD_SHARE_HANDLE(share))
    بازگشت CURLSHE_INVALID;

  اگر(share_in_use(share))
    بازگشت CURLSHE_IN_USE;

  locked = share_lock_acquire(share, پوچ);
  share_unlink(&share, پوچ, locked);
  بازگشت CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(ساختار Curl_share *share,
                                 ساختار Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  اگر(!share)
    بازگشت CURLSHE_INVALID;

  اگر(share->specifier & (unsigned صحیح)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  بازگشت CURLSHE_OK;
}

CURLSHcode Curl_share_lock(ساختار Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  بازگشت Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(ساختار Curl_share *share,
                                   ساختار Curl_easy *data,
                                   curl_lock_data type)
{
  اگر(!share)
    بازگشت CURLSHE_INVALID;

  اگر(share->specifier & (unsigned صحیح)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  بازگشت CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(ساختار Curl_easy *data, curl_lock_data type)
{
  بازگشت Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(ساختار Curl_easy *data)
{
  ساختار Curl_share *share = data->share;

  اگر(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    اگر((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      اگر(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    اگر(share->cookies == data->cookies)
      data->cookies = پوچ;
#endif

#ifndef CURL_DISABLE_HSTS
    اگر(share->hsts == data->hsts)
      data->hsts = پوچ;
#endif
#ifdef USE_LIBPSL
    اگر(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : پوچ;
#endif

    share_unlink(&data->share, data, locked);
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_share_easy_link(ساختار Curl_easy *data,
                              ساختار Curl_share *share)
{
  اگر(data->share) {
    DEBUGASSERT(0);
    بازگشت CURLE_FAILED_INIT;
  }

  اگر(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    اگر((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    اگر(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    اگر(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    اگر(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  بازگشت CURLE_OK;
}
