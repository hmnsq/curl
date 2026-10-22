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

  إذا(share->specifier & (1 << CURL_قفل_داده_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#پایان_اگر

#اگر_تعریف_شده USE_SSL
  إذا(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = عدم;
  }
#پایان_اگر

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#اگر_تعریف_شده USE_MUTEX
  Curl_mutex_destroy(&share->قفل);
#پایان_اگر
  curlx_memzero(share, حجم(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(فراغ)
{
  بنية Curl_share *share = curlx_calloc(1, حجم(بنية Curl_share));
  إذا(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_قفل_داده_SHARE);
#اگر_تعریف_شده USE_MUTEX
    Curl_mutex_init(&share->قفل);
#پایان_اگر
    share->ref_شمار = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    إذا(!share->admin) {
      share_destroy(share);
      أرجع عدم;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.داخلی = TRUE;
#اگر_تعریف_شده DEBUGBUILD
    إذا(getenv("CURL_DEBUG"))
      share->admin->مجموعه.verbose = TRUE;
#پایان_اگر
  }
  أرجع share;
}

ساكن uint32_t share_ref_inc(بنية Curl_share *share)
{
  uint32_t n;
#اگر_تعریف_شده USE_MUTEX
  Curl_mutex_acquire(&share->قفل);
  n = ++share->ref_شمار;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->قفل);
#وإلا
  n = ++share->ref_شمار;
  share->has_been_shared = TRUE;
#پایان_اگر
  أرجع n;
}

ساكن uint32_t share_ref_dec(بنية Curl_share *share)
{
  uint32_t n;
#اگر_تعریف_شده USE_MUTEX
  Curl_mutex_acquire(&share->قفل);
  DEBUGASSERT(share->ref_شمار);
  n = --share->ref_شمار;
  Curl_mutex_release(&share->قفل);
#وإلا
  n = --share->ref_شمار;
#پایان_اگر
  أرجع n;
}

ساكن bool share_has_been_shared(بنية Curl_share *share)
{
  bool was_shared;
#اگر_تعریف_شده USE_MUTEX
  Curl_mutex_acquire(&share->قفل);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->قفل);
#وإلا
  was_shared = share->has_been_shared;
#پایان_اگر
  أرجع was_shared;
}

ساكن bool share_قفل_acquire(بنية Curl_share *share,
                               بنية Curl_easy *داده)
{
  إذا(share->lockfunc && share->unlockfunc &&
     (داده || share_has_been_shared(share))) {
    share->lockfunc(داده, CURL_قفل_داده_SHARE, CURL_قفل_ACCESS_SINGLE,
                    share->clientdata);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن فراغ share_قفل_release(بنية Curl_share *share,
                               بنية Curl_easy *داده,
                               bool locked)
{
  إذا(locked) {
    DEBUGASSERT(share->unlockfunc);
    إذا(share->unlockfunc)
      share->unlockfunc(داده, CURL_قفل_داده_SHARE, share->clientdata);
  }
}

ساكن bool share_درون_use(بنية Curl_share *share)
{
  bool درون_use;
#اگر_تعریف_شده USE_MUTEX
  Curl_mutex_acquire(&share->قفل);
  درون_use = (share->ref_شمار > 1);
  Curl_mutex_release(&share->قفل);
#وإلا
  bool locked = share_قفل_acquire(share, عدم);
  درون_use = (share->ref_شمار > 1);
  share_قفل_release(share, عدم, locked);
#پایان_اگر
  أرجع درون_use;
}

ساكن فراغ share_unlink(بنية Curl_share **pshare,
                         بنية Curl_easy *داده,
                         bool locked)
{
  بنية Curl_share *share = *pshare;
  uint32_t n;

  *pshare = عدم;
  n = share_ref_dec(share);
  إذا(locked)
    share_قفل_release(share, داده, locked);
  إذا(!n)  /* last reference gone */
    share_destroy(share);
}

#لغو_تعریف curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  صحيح type;
  curl_قفل_function lockfunc;
  curl_unlock_function unlockfunc;
  فراغ *ptr;
  CURLSHcode res = CURLSHE_OK;
  بنية Curl_share *share = sh;

  إذا(!GOOD_SHARE_HANDLE(share))
    أرجع CURLSHE_INVALID;

  إذا(share_درون_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    أرجع CURLSHE_درون_USE;
  }

  va_start(param, option);

  بدل(option) {
  حالة CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, صحيح);

    بدل(type) {
    حالة CURL_قفل_داده_DNS:
      اكسر;

    حالة CURL_قفل_داده_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(!share->cookies) {
        share->cookies = Curl_cookie_init();
        إذا(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_درون;
#پایان_اگر
      اكسر;

    حالة CURL_قفل_داده_HSTS:
#اگر_تعریف_نشده CURL_DISABLE_HSTS
      إذا(!share->hsts) {
        share->hsts = Curl_hsts_init();
        إذا(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_درون;
#پایان_اگر
      اكسر;

    حالة CURL_قفل_داده_SSL_SESSION:
#اگر_تعریف_شده USE_SSL
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
      res = CURLSHE_NOT_BUILT_درون;
#پایان_اگر
      اكسر;

    حالة CURL_قفل_داده_CONNECT:
      /* It is safe to set this option several times on a share. */
      إذا(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      اكسر;

    حالة CURL_قفل_داده_PSL:
#اگر_تعریف_نشده USE_LIBPSL
      res = CURLSHE_NOT_BUILT_درون;
#پایان_اگر
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
    }
    إذا(!res)
      share->specifier |= (بیعلامت صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, صحيح);
    بدل(type) {
    حالة CURL_قفل_داده_DNS:
      اكسر;

    حالة CURL_قفل_داده_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = عدم;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_درون;
#پایان_اگر
      اكسر;

    حالة CURL_قفل_داده_HSTS:
#اگر_تعریف_نشده CURL_DISABLE_HSTS
      إذا(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_درون;
#پایان_اگر
      اكسر;

    حالة CURL_قفل_داده_SSL_SESSION:
#اگر_تعریف_شده USE_SSL
      إذا(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = عدم;
      }
#وإلا
      res = CURLSHE_NOT_BUILT_درون;
#پایان_اگر
      اكسر;

    حالة CURL_قفل_داده_CONNECT:
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
      اكسر;
    }
    إذا(!res)
      share->specifier &= ~(بیعلامت صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_قفل_function);
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

  إذا(share_درون_use(share))
    أرجع CURLSHE_درون_USE;

  locked = share_قفل_acquire(share, عدم);
  share_unlink(&share, عدم, locked);
  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_قفل_share(بنية Curl_share *share,
                                 بنية Curl_easy *داده,
                                 curl_قفل_داده type,
                                 curl_قفل_access accesstype)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بیعلامت صحيح)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(داده, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_قفل(بنية Curl_easy *داده, curl_قفل_داده type,
                           curl_قفل_access accesstype)
{
  أرجع Curl_share_قفل_share(داده->share, داده, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(بنية Curl_share *share,
                                   بنية Curl_easy *داده,
                                   curl_قفل_داده type)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بیعلامت صحيح)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(داده, type, share->clientdata);

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(بنية Curl_easy *داده, curl_قفل_داده type)
{
  أرجع Curl_share_unlock_share(داده->share, داده, type);
}

CURLcode Curl_share_easy_unlink(بنية Curl_easy *داده)
{
  بنية Curl_share *share = داده->share;

  إذا(share) {
    bool locked = share_قفل_acquire(share, داده);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    إذا((share->specifier & (1 << CURL_قفل_داده_CONNECT))) {
      إذا(داده->conn)
        Curl_detach_connection(داده);
      داده->state.آخرین_conn_id = -1;
      داده->state.آخرین_cpid = UINT32_MAX;
    }

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    إذا(share->cookies == داده->cookies)
      داده->cookies = عدم;
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_HSTS
    إذا(share->hsts == داده->hsts)
      داده->hsts = عدم;
#پایان_اگر
#اگر_تعریف_شده USE_LIBPSL
    إذا(&share->psl == داده->psl)
      داده->psl = داده->multi ? &داده->multi->psl : عدم;
#پایان_اگر

    share_unlink(&داده->share, داده, locked);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_share_easy_link(بنية Curl_easy *داده,
                              بنية Curl_share *share)
{
  إذا(داده->share) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  إذا(share) {
    bool locked = share_قفل_acquire(share, داده);

    /* Forget identifiers from the previously used connection cache. */
    إذا((share->specifier & (1 << CURL_قفل_داده_CONNECT))) {
      داده->state.آخرین_conn_id = -1;
      داده->state.آخرین_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    داده->share = share;

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    إذا(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(داده->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      داده->cookies = share->cookies;
    }
#پایان_اگر /* CURL_DISABLE_HTTP */
#اگر_تعریف_نشده CURL_DISABLE_HSTS
    إذا(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&داده->hsts);
      داده->hsts = share->hsts;
    }
#پایان_اگر
#اگر_تعریف_شده USE_LIBPSL
    إذا(share->specifier & (1 << CURL_قفل_داده_PSL))
      داده->psl = &share->psl;
#پایان_اگر

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_قفل_release(share, داده, locked);
  }
  أرجع CURLE_OK;
}
