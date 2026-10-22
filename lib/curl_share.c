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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "curl_threads.h"
#অন্তর্ভাৱ "curl_share.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "vtls/vtls_scache.h"
#অন্তর্ভাৱ "hsts.h"
#অন্তর্ভাৱ "url.h"

স্থির শূন্য share_destroy(সংরচনা Curl_share *share)
{
  যদি(!share)
    ঘূৰাই;

  যদি(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  যদি(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = রিক্ত;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, পরিমাণ(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(শূন্য)
{
  সংরচনা Curl_share *share = curlx_calloc(1, পরিমাণ(সংরচনা Curl_share));
  যদি(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_গুনতি = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    যদি(!share->admin) {
      share_destroy(share);
      ঘূৰাই রিক্ত;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    যদি(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  ঘূৰাই share;
}

স্থির uint32_t share_ref_inc(সংরচনা Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_গুনতি;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#অন্যথা
  n = ++share->ref_গুনতি;
  share->has_been_shared = TRUE;
#endif
  ঘূৰাই n;
}

স্থির uint32_t share_ref_dec(সংরচনা Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_গুনতি);
  n = --share->ref_গুনতি;
  Curl_mutex_release(&share->lock);
#অন্যথা
  n = --share->ref_গুনতি;
#endif
  ঘূৰাই n;
}

স্থির bool share_has_been_shared(সংরচনা Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#অন্যথা
  was_shared = share->has_been_shared;
#endif
  ঘূৰাই was_shared;
}

স্থির bool share_lock_acquire(সংরচনা Curl_share *share,
                               সংরচনা Curl_easy *data)
{
  যদি(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    ঘূৰাই TRUE;
  }
  ঘূৰাই FALSE;
}

স্থির শূন্য share_lock_release(সংরচনা Curl_share *share,
                               সংরচনা Curl_easy *data,
                               bool locked)
{
  যদি(locked) {
    DEBUGASSERT(share->unlockfunc);
    যদি(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

স্থির bool share_in_use(সংরচনা Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_গুনতি > 1);
  Curl_mutex_release(&share->lock);
#অন্যথা
  bool locked = share_lock_acquire(share, রিক্ত);
  in_use = (share->ref_গুনতি > 1);
  share_lock_release(share, রিক্ত, locked);
#endif
  ঘূৰাই in_use;
}

স্থির শূন্য share_unlink(সংরচনা Curl_share **pshare,
                         সংরচনা Curl_easy *data,
                         bool locked)
{
  সংরচনা Curl_share *share = *pshare;
  uint32_t n;

  *pshare = রিক্ত;
  n = share_ref_dec(share);
  যদি(locked)
    share_lock_release(share, data, locked);
  যদি(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  পূর্ণাঙ্ক type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  শূন্য *ptr;
  CURLSHcode res = CURLSHE_OK;
  সংরচনা Curl_share *share = sh;

  যদি(!GOOD_SHARE_HANDLE(share))
    ঘূৰাই CURLSHE_INVALID;

  যদি(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    ঘূৰাই CURLSHE_IN_USE;
  }

  va_start(param, option);

  ৱিকল্প(option) {
  পক্ষ CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, পূর্ণাঙ্ক);

    ৱিকল্প(type) {
    পক্ষ CURL_LOCK_DATA_DNS:
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_COOKIE:
#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      যদি(!share->cookies) {
        share->cookies = Curl_cookie_init();
        যদি(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#অন্যথা /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      যদি(!share->hsts) {
        share->hsts = Curl_hsts_init();
        যদি(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#অন্যথা /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      যদি(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        যদি(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#অন্যথা
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      যদি(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ভঙ্গ;

    স্ৱভাৱ:
      res = CURLSHE_BAD_OPTION;
    }
    যদি(!res)
      share->specifier |= (unsigned পূর্ণাঙ্ক)(1 << type);
    ভঙ্গ;

  পক্ষ CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, পূর্ণাঙ্ক);
    ৱিকল্প(type) {
    পক্ষ CURL_LOCK_DATA_DNS:
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_COOKIE:
#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      যদি(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = রিক্ত;
      }
#অন্যথা /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      যদি(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#অন্যথা /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      যদি(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = রিক্ত;
      }
#অন্যথা
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ভঙ্গ;

    পক্ষ CURL_LOCK_DATA_CONNECT:
      ভঙ্গ;

    স্ৱভাৱ:
      res = CURLSHE_BAD_OPTION;
      ভঙ্গ;
    }
    যদি(!res)
      share->specifier &= ~(unsigned পূর্ণাঙ্ক)(1 << type);
    ভঙ্গ;

  পক্ষ CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    ভঙ্গ;

  পক্ষ CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    ভঙ্গ;

  পক্ষ CURLSHOPT_USERDATA:
    ptr = va_arg(param, শূন্য *);
    share->clientdata = ptr;
    ভঙ্গ;

  স্ৱভাৱ:
    res = CURLSHE_BAD_OPTION;
    ভঙ্গ;
  }

  va_end(param);

  ঘূৰাই res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  সংরচনা Curl_share *share = sh;
  bool locked;
  যদি(!GOOD_SHARE_HANDLE(share))
    ঘূৰাই CURLSHE_INVALID;

  যদি(share_in_use(share))
    ঘূৰাই CURLSHE_IN_USE;

  locked = share_lock_acquire(share, রিক্ত);
  share_unlink(&share, রিক্ত, locked);
  ঘূৰাই CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(সংরচনা Curl_share *share,
                                 সংরচনা Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  যদি(!share)
    ঘূৰাই CURLSHE_INVALID;

  যদি(share->specifier & (unsigned পূর্ণাঙ্ক)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  ঘূৰাই CURLSHE_OK;
}

CURLSHcode Curl_share_lock(সংরচনা Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  ঘূৰাই Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(সংরচনা Curl_share *share,
                                   সংরচনা Curl_easy *data,
                                   curl_lock_data type)
{
  যদি(!share)
    ঘূৰাই CURLSHE_INVALID;

  যদি(share->specifier & (unsigned পূর্ণাঙ্ক)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  ঘূৰাই CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(সংরচনা Curl_easy *data, curl_lock_data type)
{
  ঘূৰাই Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_share *share = data->share;

  যদি(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    যদি((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      যদি(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    যদি(share->cookies == data->cookies)
      data->cookies = রিক্ত;
#endif

#ifndef CURL_DISABLE_HSTS
    যদি(share->hsts == data->hsts)
      data->hsts = রিক্ত;
#endif
#ifdef USE_LIBPSL
    যদি(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : রিক্ত;
#endif

    share_unlink(&data->share, data, locked);
  }
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_share_easy_link(সংরচনা Curl_easy *data,
                              সংরচনা Curl_share *share)
{
  যদি(data->share) {
    DEBUGASSERT(0);
    ঘূৰাই CURLE_FAILED_INIT;
  }

  যদি(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    যদি((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    যদি(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    যদি(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    যদি(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  ঘূৰাই CURLE_OK;
}
