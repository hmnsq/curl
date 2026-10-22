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
  إذا(!share)
    أرجع;

  إذا(share->specifier & (1 << CURL_جندرا_انکڑے_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#جے_انت

#جے_تعریف_شدہ USE_SSL
  إذا(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = عدم;
  }
#جے_انت

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#جے_تعریف_شدہ USE_MUTEX
  Curl_mutex_destroy(&share->جندرا);
#جے_انت
  curlx_memzero(share, حجم(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(فراغ)
{
  بنية Curl_share *share = curlx_calloc(1, حجم(بنية Curl_share));
  إذا(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_جندرا_انکڑے_SHARE);
#جے_تعریف_شدہ USE_MUTEX
    Curl_mutex_init(&share->جندرا);
#جے_انت
    share->ref_گنتی = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    إذا(!share->admin) {
      share_destroy(share);
      أرجع عدم;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.اندرونی = TRUE;
#جے_تعریف_شدہ DEBUGBUILD
    إذا(getenv("CURL_DEBUG"))
      share->admin->سیٹ.verbose = TRUE;
#جے_انت
  }
  أرجع share;
}

ساكن uint32_t share_ref_inc(بنية Curl_share *share)
{
  uint32_t n;
#جے_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->جندرا);
  n = ++share->ref_گنتی;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->جندرا);
#وإلا
  n = ++share->ref_گنتی;
  share->has_been_shared = TRUE;
#جے_انت
  أرجع n;
}

ساكن uint32_t share_ref_dec(بنية Curl_share *share)
{
  uint32_t n;
#جے_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->جندرا);
  DEBUGASSERT(share->ref_گنتی);
  n = --share->ref_گنتی;
  Curl_mutex_release(&share->جندرا);
#وإلا
  n = --share->ref_گنتی;
#جے_انت
  أرجع n;
}

ساكن bool share_has_been_shared(بنية Curl_share *share)
{
  bool was_shared;
#جے_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->جندرا);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->جندرا);
#وإلا
  was_shared = share->has_been_shared;
#جے_انت
  أرجع was_shared;
}

ساكن bool share_جندرا_acquire(بنية Curl_share *share,
                               بنية Curl_easy *انکڑے)
{
  إذا(share->lockfunc && share->unlockfunc &&
     (انکڑے || share_has_been_shared(share))) {
    share->lockfunc(انکڑے, CURL_جندرا_انکڑے_SHARE, CURL_جندرا_ACCESS_SINGLE,
                    share->clientdata);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن فراغ share_جندرا_release(بنية Curl_share *share,
                               بنية Curl_easy *انکڑے,
                               bool locked)
{
  إذا(locked) {
    DEBUGASSERT(share->unlockfunc);
    إذا(share->unlockfunc)
      share->unlockfunc(انکڑے, CURL_جندرا_انکڑے_SHARE, share->clientdata);
  }
}

ساكن bool share_وچ_use(بنية Curl_share *share)
{
  bool وچ_use;
#جے_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->جندرا);
  وچ_use = (share->ref_گنتی > 1);
  Curl_mutex_release(&share->جندرا);
#وإلا
  bool locked = share_جندرا_acquire(share, عدم);
  وچ_use = (share->ref_گنتی > 1);
  share_جندرا_release(share, عدم, locked);
#جے_انت
  أرجع وچ_use;
}

ساكن فراغ share_unlink(بنية Curl_share **pshare,
                         بنية Curl_easy *انکڑے,
                         bool locked)
{
  بنية Curl_share *share = *pshare;
  uint32_t n;

  *pshare = عدم;
  n = share_ref_dec(share);
  إذا(locked)
    share_جندرا_release(share, انکڑے, locked);
  إذا(!n)  /* last reference gone */
    share_destroy(share);
}

#تعریف_رد curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  صحيح type;
  curl_جندرا_function lockfunc;
  curl_unlock_function unlockfunc;
  فراغ *ptr;
  CURLSHcode res = CURLSHE_OK;
  بنية Curl_share *share = sh;

  إذا(!GOOD_SHARE_HANDLE(share))
    أرجع CURLSHE_INVALID;

  إذا(share_وچ_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    أرجع CURLSHE_وچ_USE;
  }

  va_start(param, option);

  بدل(option) {
  حالة CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, صحيح);

    بدل(type) {
    حالة CURL_جندرا_انکڑے_DNS:
      اكسر;

    حالة CURL_جندرا_انکڑے_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(!share->cookies) {
        share->cookies = Curl_cookie_init();
        إذا(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_وچ;
#جے_انت
      اكسر;

    حالة CURL_جندرا_انکڑے_HSTS:
#جے_بنا_تعریف CURL_DISABLE_HSTS
      إذا(!share->hsts) {
        share->hsts = Curl_hsts_init();
        إذا(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_وچ;
#جے_انت
      اكسر;

    حالة CURL_جندرا_انکڑے_SSL_SESSION:
#جے_تعریف_شدہ USE_SSL
      إذا(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        إذا(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#وإلا
      res = CURLSHE_NOT_BUILT_وچ;
#جے_انت
      اكسر;

    حالة CURL_جندرا_انکڑے_CONNECT:
      /* It is safe to set this option several times on a share. */
      إذا(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      اكسر;

    حالة CURL_جندرا_انکڑے_PSL:
#جے_بنا_تعریف USE_LIBPSL
      res = CURLSHE_NOT_BUILT_وچ;
#جے_انت
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
    }
    إذا(!res)
      share->specifier |= (بنانشانی صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, صحيح);
    بدل(type) {
    حالة CURL_جندرا_انکڑے_DNS:
      اكسر;

    حالة CURL_جندرا_انکڑے_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = عدم;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_وچ;
#جے_انت
      اكسر;

    حالة CURL_جندرا_انکڑے_HSTS:
#جے_بنا_تعریف CURL_DISABLE_HSTS
      إذا(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_وچ;
#جے_انت
      اكسر;

    حالة CURL_جندرا_انکڑے_SSL_SESSION:
#جے_تعریف_شدہ USE_SSL
      إذا(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = عدم;
      }
#وإلا
      res = CURLSHE_NOT_BUILT_وچ;
#جے_انت
      اكسر;

    حالة CURL_جندرا_انکڑے_CONNECT:
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
      اكسر;
    }
    إذا(!res)
      share->specifier &= ~(بنانشانی صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_جندرا_function);
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

  أرجع res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  بنية Curl_share *share = sh;
  bool locked;
  إذا(!GOOD_SHARE_HANDLE(share))
    أرجع CURLSHE_INVALID;

  إذا(share_وچ_use(share))
    أرجع CURLSHE_وچ_USE;

  locked = share_جندرا_acquire(share, عدم);
  share_unlink(&share, عدم, locked);
  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_جندرا_share(بنية Curl_share *share,
                                 بنية Curl_easy *انکڑے,
                                 curl_جندرا_انکڑے type,
                                 curl_جندرا_access accesstype)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بنانشانی صحيح)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(انکڑے, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_جندرا(بنية Curl_easy *انکڑے, curl_جندرا_انکڑے type,
                           curl_جندرا_access accesstype)
{
  أرجع Curl_share_جندرا_share(انکڑے->share, انکڑے, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(بنية Curl_share *share,
                                   بنية Curl_easy *انکڑے,
                                   curl_جندرا_انکڑے type)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بنانشانی صحيح)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(انکڑے, type, share->clientdata);

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(بنية Curl_easy *انکڑے, curl_جندرا_انکڑے type)
{
  أرجع Curl_share_unlock_share(انکڑے->share, انکڑے, type);
}

CURLcode Curl_share_easy_unlink(بنية Curl_easy *انکڑے)
{
  بنية Curl_share *share = انکڑے->share;

  إذا(share) {
    bool locked = share_جندرا_acquire(share, انکڑے);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    إذا((share->specifier & (1 << CURL_جندرا_انکڑے_CONNECT))) {
      إذا(انکڑے->conn)
        Curl_detach_connection(انکڑے);
      انکڑے->state.آخری_conn_id = -1;
      انکڑے->state.آخری_cpid = UINT32_MAX;
    }

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    إذا(share->cookies == انکڑے->cookies)
      انکڑے->cookies = عدم;
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_HSTS
    إذا(share->hsts == انکڑے->hsts)
      انکڑے->hsts = عدم;
#جے_انت
#جے_تعریف_شدہ USE_LIBPSL
    إذا(&share->psl == انکڑے->psl)
      انکڑے->psl = انکڑے->multi ? &انکڑے->multi->psl : عدم;
#جے_انت

    share_unlink(&انکڑے->share, انکڑے, locked);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_share_easy_link(بنية Curl_easy *انکڑے,
                              بنية Curl_share *share)
{
  إذا(انکڑے->share) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  إذا(share) {
    bool locked = share_جندرا_acquire(share, انکڑے);

    /* Forget identifiers from the previously used connection cache. */
    إذا((share->specifier & (1 << CURL_جندرا_انکڑے_CONNECT))) {
      انکڑے->state.آخری_conn_id = -1;
      انکڑے->state.آخری_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    انکڑے->share = share;

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    إذا(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(انکڑے->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      انکڑے->cookies = share->cookies;
    }
#جے_انت /* CURL_DISABLE_HTTP */
#جے_بنا_تعریف CURL_DISABLE_HSTS
    إذا(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&انکڑے->hsts);
      انکڑے->hsts = share->hsts;
    }
#جے_انت
#جے_تعریف_شدہ USE_LIBPSL
    إذا(share->specifier & (1 << CURL_جندرا_انکڑے_PSL))
      انکڑے->psl = &share->psl;
#جے_انت

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_جندرا_release(share, انکڑے, locked);
  }
  أرجع CURLE_OK;
}
