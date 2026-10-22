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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "hsts.h"
#অন্তর্ভাব "url.h"

স্থির শূন্য share_destroy(কাঠামো Curl_share *share)
{
  যদি(!share)
    ফেরত;

  যদি(share->specifier & (1 << CURL_LOCK_তথ্য_CONNECT)) {
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
    share->ssl_scache = ফাঁকা;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, আকার(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(শূন্য)
{
  কাঠামো Curl_share *share = curlx_calloc(1, আকার(কাঠামো Curl_share));
  যদি(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_তথ্য_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_গুনতি = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    যদি(!share->admin) {
      share_destroy(share);
      ফেরত ফাঁকা;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    যদি(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  ফেরত share;
}

স্থির uint32_t share_ref_inc(কাঠামো Curl_share *share)
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
  ফেরত n;
}

স্থির uint32_t share_ref_dec(কাঠামো Curl_share *share)
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
  ফেরত n;
}

স্থির bool share_has_been_shared(কাঠামো Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#অন্যথা
  was_shared = share->has_been_shared;
#endif
  ফেরত was_shared;
}

স্থির bool share_lock_acquire(কাঠামো Curl_share *share,
                               কাঠামো Curl_easy *তথ্য)
{
  যদি(share->lockfunc && share->unlockfunc &&
     (তথ্য || share_has_been_shared(share))) {
    share->lockfunc(তথ্য, CURL_LOCK_তথ্য_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

স্থির শূন্য share_lock_release(কাঠামো Curl_share *share,
                               কাঠামো Curl_easy *তথ্য,
                               bool locked)
{
  যদি(locked) {
    DEBUGASSERT(share->unlockfunc);
    যদি(share->unlockfunc)
      share->unlockfunc(তথ্য, CURL_LOCK_তথ্য_SHARE, share->clientdata);
  }
}

স্থির bool share_in_use(কাঠামো Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_গুনতি > 1);
  Curl_mutex_release(&share->lock);
#অন্যথা
  bool locked = share_lock_acquire(share, ফাঁকা);
  in_use = (share->ref_গুনতি > 1);
  share_lock_release(share, ফাঁকা, locked);
#endif
  ফেরত in_use;
}

স্থির শূন্য share_unlink(কাঠামো Curl_share **pshare,
                         কাঠামো Curl_easy *তথ্য,
                         bool locked)
{
  কাঠামো Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ফাঁকা;
  n = share_ref_dec(share);
  যদি(locked)
    share_lock_release(share, তথ্য, locked);
  যদি(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  পূর্ণসংখ্যা type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  শূন্য *ptr;
  CURLSHcode res = CURLSHE_OK;
  কাঠামো Curl_share *share = sh;

  যদি(!GOOD_SHARE_HANDLE(share))
    ফেরত CURLSHE_INVALID;

  যদি(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    ফেরত CURLSHE_IN_USE;
  }

  va_start(param, option);

  নির্বাচন(option) {
  ক্ষেত্র CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, পূর্ণসংখ্যা);

    নির্বাচন(type) {
    ক্ষেত্র CURL_LOCK_তথ্য_DNS:
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_COOKIE:
#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      যদি(!share->cookies) {
        share->cookies = Curl_cookie_init();
        যদি(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#অন্যথা /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_HSTS:
#ifndef CURL_DISABLE_HSTS
      যদি(!share->hsts) {
        share->hsts = Curl_hsts_init();
        যদি(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#অন্যথা /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_SSL_SESSION:
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
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_CONNECT:
      /* It is safe to set this option several times on a share. */
      যদি(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      বিরতি;

    পূর্বনির্ধারিত:
      res = CURLSHE_BAD_OPTION;
    }
    যদি(!res)
      share->specifier |= (unsigned পূর্ণসংখ্যা)(1 << type);
    বিরতি;

  ক্ষেত্র CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, পূর্ণসংখ্যা);
    নির্বাচন(type) {
    ক্ষেত্র CURL_LOCK_তথ্য_DNS:
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_COOKIE:
#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      যদি(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ফাঁকা;
      }
#অন্যথা /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_HSTS:
#ifndef CURL_DISABLE_HSTS
      যদি(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#অন্যথা /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_SSL_SESSION:
#ifdef USE_SSL
      যদি(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ফাঁকা;
      }
#অন্যথা
      res = CURLSHE_NOT_BUILT_IN;
#endif
      বিরতি;

    ক্ষেত্র CURL_LOCK_তথ্য_CONNECT:
      বিরতি;

    পূর্বনির্ধারিত:
      res = CURLSHE_BAD_OPTION;
      বিরতি;
    }
    যদি(!res)
      share->specifier &= ~(unsigned পূর্ণসংখ্যা)(1 << type);
    বিরতি;

  ক্ষেত্র CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    বিরতি;

  ক্ষেত্র CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    বিরতি;

  ক্ষেত্র CURLSHOPT_USERDATA:
    ptr = va_arg(param, শূন্য *);
    share->clientdata = ptr;
    বিরতি;

  পূর্বনির্ধারিত:
    res = CURLSHE_BAD_OPTION;
    বিরতি;
  }

  va_end(param);

  ফেরত res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  কাঠামো Curl_share *share = sh;
  bool locked;
  যদি(!GOOD_SHARE_HANDLE(share))
    ফেরত CURLSHE_INVALID;

  যদি(share_in_use(share))
    ফেরত CURLSHE_IN_USE;

  locked = share_lock_acquire(share, ফাঁকা);
  share_unlink(&share, ফাঁকা, locked);
  ফেরত CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(কাঠামো Curl_share *share,
                                 কাঠামো Curl_easy *তথ্য,
                                 curl_lock_তথ্য type,
                                 curl_lock_access accesstype)
{
  যদি(!share)
    ফেরত CURLSHE_INVALID;

  যদি(share->specifier & (unsigned পূর্ণসংখ্যা)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(তথ্য, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  ফেরত CURLSHE_OK;
}

CURLSHcode Curl_share_lock(কাঠামো Curl_easy *তথ্য, curl_lock_তথ্য type,
                           curl_lock_access accesstype)
{
  ফেরত Curl_share_lock_share(তথ্য->share, তথ্য, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(কাঠামো Curl_share *share,
                                   কাঠামো Curl_easy *তথ্য,
                                   curl_lock_তথ্য type)
{
  যদি(!share)
    ফেরত CURLSHE_INVALID;

  যদি(share->specifier & (unsigned পূর্ণসংখ্যা)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(তথ্য, type, share->clientdata);

  ফেরত CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(কাঠামো Curl_easy *তথ্য, curl_lock_তথ্য type)
{
  ফেরত Curl_share_unlock_share(তথ্য->share, তথ্য, type);
}

CURLcode Curl_share_easy_unlink(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_share *share = তথ্য->share;

  যদি(share) {
    bool locked = share_lock_acquire(share, তথ্য);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    যদি((share->specifier & (1 << CURL_LOCK_তথ্য_CONNECT))) {
      যদি(তথ্য->conn)
        Curl_detach_connection(তথ্য);
      তথ্য->state.last_conn_id = -1;
      তথ্য->state.last_cpid = UINT32_MAX;
    }

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    যদি(share->cookies == তথ্য->cookies)
      তথ্য->cookies = ফাঁকা;
#endif

#ifndef CURL_DISABLE_HSTS
    যদি(share->hsts == তথ্য->hsts)
      তথ্য->hsts = ফাঁকা;
#endif
#ifdef USE_LIBPSL
    যদি(&share->psl == তথ্য->psl)
      তথ্য->psl = তথ্য->multi ? &তথ্য->multi->psl : ফাঁকা;
#endif

    share_unlink(&তথ্য->share, তথ্য, locked);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_share_easy_link(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_share *share)
{
  যদি(তথ্য->share) {
    DEBUGASSERT(0);
    ফেরত CURLE_FAILED_INIT;
  }

  যদি(share) {
    bool locked = share_lock_acquire(share, তথ্য);

    /* Forget identifiers from the previously used connection cache. */
    যদি((share->specifier & (1 << CURL_LOCK_তথ্য_CONNECT))) {
      তথ্য->state.last_conn_id = -1;
      তথ্য->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    তথ্য->share = share;

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    যদি(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(তথ্য->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      তথ্য->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    যদি(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&তথ্য->hsts);
      তথ্য->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    যদি(share->specifier & (1 << CURL_LOCK_তথ্য_PSL))
      তথ্য->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, তথ্য, locked);
  }
  ফেরত CURLE_OK;
}
