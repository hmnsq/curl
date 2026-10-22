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

  إذا(share->specifier & (1 << CURL_قلف_معلومات_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#د_که_پای

#که_تعریف_شوی USE_SSL
  إذا(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = عدم;
  }
#د_که_پای

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#که_تعریف_شوی USE_MUTEX
  Curl_mutex_destroy(&share->قلف);
#د_که_پای
  curlx_memzero(share, حجم(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(فراغ)
{
  بنية Curl_share *share = curlx_calloc(1, حجم(بنية Curl_share));
  إذا(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_قلف_معلومات_SHARE);
#که_تعریف_شوی USE_MUTEX
    Curl_mutex_init(&share->قلف);
#د_که_پای
    share->ref_شمېر = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    إذا(!share->admin) {
      share_destroy(share);
      أرجع عدم;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.دننی = TRUE;
#که_تعریف_شوی DEBUGBUILD
    إذا(getenv("CURL_DEBUG"))
      share->admin->سېټ.verbose = TRUE;
#د_که_پای
  }
  أرجع share;
}

ساكن uint32_t share_ref_inc(بنية Curl_share *share)
{
  uint32_t n;
#که_تعریف_شوی USE_MUTEX
  Curl_mutex_acquire(&share->قلف);
  n = ++share->ref_شمېر;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->قلف);
#وإلا
  n = ++share->ref_شمېر;
  share->has_been_shared = TRUE;
#د_که_پای
  أرجع n;
}

ساكن uint32_t share_ref_dec(بنية Curl_share *share)
{
  uint32_t n;
#که_تعریف_شوی USE_MUTEX
  Curl_mutex_acquire(&share->قلف);
  DEBUGASSERT(share->ref_شمېر);
  n = --share->ref_شمېر;
  Curl_mutex_release(&share->قلف);
#وإلا
  n = --share->ref_شمېر;
#د_که_پای
  أرجع n;
}

ساكن bool share_has_been_shared(بنية Curl_share *share)
{
  bool was_shared;
#که_تعریف_شوی USE_MUTEX
  Curl_mutex_acquire(&share->قلف);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->قلف);
#وإلا
  was_shared = share->has_been_shared;
#د_که_پای
  أرجع was_shared;
}

ساكن bool share_قلف_acquire(بنية Curl_share *share,
                               بنية Curl_easy *معلومات)
{
  إذا(share->lockfunc && share->unlockfunc &&
     (معلومات || share_has_been_shared(share))) {
    share->lockfunc(معلومات, CURL_قلف_معلومات_SHARE, CURL_قلف_ACCESS_SINGLE,
                    share->clientdata);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن فراغ share_قلف_release(بنية Curl_share *share,
                               بنية Curl_easy *معلومات,
                               bool locked)
{
  إذا(locked) {
    DEBUGASSERT(share->unlockfunc);
    إذا(share->unlockfunc)
      share->unlockfunc(معلومات, CURL_قلف_معلومات_SHARE, share->clientdata);
  }
}

ساكن bool share_دننه_use(بنية Curl_share *share)
{
  bool دننه_use;
#که_تعریف_شوی USE_MUTEX
  Curl_mutex_acquire(&share->قلف);
  دننه_use = (share->ref_شمېر > 1);
  Curl_mutex_release(&share->قلف);
#وإلا
  bool locked = share_قلف_acquire(share, عدم);
  دننه_use = (share->ref_شمېر > 1);
  share_قلف_release(share, عدم, locked);
#د_که_پای
  أرجع دننه_use;
}

ساكن فراغ share_unlink(بنية Curl_share **pshare,
                         بنية Curl_easy *معلومات,
                         bool locked)
{
  بنية Curl_share *share = *pshare;
  uint32_t n;

  *pshare = عدم;
  n = share_ref_dec(share);
  إذا(locked)
    share_قلف_release(share, معلومات, locked);
  إذا(!n)  /* last reference gone */
    share_destroy(share);
}

#تعریف_لغوه curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  صحيح type;
  curl_قلف_function lockfunc;
  curl_unlock_function unlockfunc;
  فراغ *ptr;
  CURLSHcode res = CURLSHE_OK;
  بنية Curl_share *share = sh;

  إذا(!GOOD_SHARE_HANDLE(share))
    أرجع CURLSHE_INVALID;

  إذا(share_دننه_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    أرجع CURLSHE_دننه_USE;
  }

  va_start(param, option);

  بدل(option) {
  حالة CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, صحيح);

    بدل(type) {
    حالة CURL_قلف_معلومات_DNS:
      اكسر;

    حالة CURL_قلف_معلومات_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(!share->cookies) {
        share->cookies = Curl_cookie_init();
        إذا(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_دننه;
#د_که_پای
      اكسر;

    حالة CURL_قلف_معلومات_HSTS:
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
      إذا(!share->hsts) {
        share->hsts = Curl_hsts_init();
        إذا(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_دننه;
#د_که_پای
      اكسر;

    حالة CURL_قلف_معلومات_SSL_SESSION:
#که_تعریف_شوی USE_SSL
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
      res = CURLSHE_NOT_BUILT_دننه;
#د_که_پای
      اكسر;

    حالة CURL_قلف_معلومات_CONNECT:
      /* It is safe to set this option several times on a share. */
      إذا(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      اكسر;

    حالة CURL_قلف_معلومات_PSL:
#که_نه_تعریف_شوی USE_LIBPSL
      res = CURLSHE_NOT_BUILT_دننه;
#د_که_پای
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
    }
    إذا(!res)
      share->specifier |= (بینښه صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, صحيح);
    بدل(type) {
    حالة CURL_قلف_معلومات_DNS:
      اكسر;

    حالة CURL_قلف_معلومات_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = عدم;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_دننه;
#د_که_پای
      اكسر;

    حالة CURL_قلف_معلومات_HSTS:
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
      إذا(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_دننه;
#د_که_پای
      اكسر;

    حالة CURL_قلف_معلومات_SSL_SESSION:
#که_تعریف_شوی USE_SSL
      إذا(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = عدم;
      }
#وإلا
      res = CURLSHE_NOT_BUILT_دننه;
#د_که_پای
      اكسر;

    حالة CURL_قلف_معلومات_CONNECT:
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
      اكسر;
    }
    إذا(!res)
      share->specifier &= ~(بینښه صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_قلف_function);
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

  إذا(share_دننه_use(share))
    أرجع CURLSHE_دننه_USE;

  locked = share_قلف_acquire(share, عدم);
  share_unlink(&share, عدم, locked);
  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_قلف_share(بنية Curl_share *share,
                                 بنية Curl_easy *معلومات,
                                 curl_قلف_معلومات type,
                                 curl_قلف_access accesstype)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بینښه صحيح)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(معلومات, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_قلف(بنية Curl_easy *معلومات, curl_قلف_معلومات type,
                           curl_قلف_access accesstype)
{
  أرجع Curl_share_قلف_share(معلومات->share, معلومات, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(بنية Curl_share *share,
                                   بنية Curl_easy *معلومات,
                                   curl_قلف_معلومات type)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بینښه صحيح)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(معلومات, type, share->clientdata);

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(بنية Curl_easy *معلومات, curl_قلف_معلومات type)
{
  أرجع Curl_share_unlock_share(معلومات->share, معلومات, type);
}

CURLcode Curl_share_easy_unlink(بنية Curl_easy *معلومات)
{
  بنية Curl_share *share = معلومات->share;

  إذا(share) {
    bool locked = share_قلف_acquire(share, معلومات);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    إذا((share->specifier & (1 << CURL_قلف_معلومات_CONNECT))) {
      إذا(معلومات->conn)
        Curl_detach_connection(معلومات);
      معلومات->state.وروستی_conn_id = -1;
      معلومات->state.وروستی_cpid = UINT32_MAX;
    }

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    إذا(share->cookies == معلومات->cookies)
      معلومات->cookies = عدم;
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_HSTS
    إذا(share->hsts == معلومات->hsts)
      معلومات->hsts = عدم;
#د_که_پای
#که_تعریف_شوی USE_LIBPSL
    إذا(&share->psl == معلومات->psl)
      معلومات->psl = معلومات->multi ? &معلومات->multi->psl : عدم;
#د_که_پای

    share_unlink(&معلومات->share, معلومات, locked);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_share_easy_link(بنية Curl_easy *معلومات,
                              بنية Curl_share *share)
{
  إذا(معلومات->share) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  إذا(share) {
    bool locked = share_قلف_acquire(share, معلومات);

    /* Forget identifiers from the previously used connection cache. */
    إذا((share->specifier & (1 << CURL_قلف_معلومات_CONNECT))) {
      معلومات->state.وروستی_conn_id = -1;
      معلومات->state.وروستی_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    معلومات->share = share;

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    إذا(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(معلومات->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      معلومات->cookies = share->cookies;
    }
#د_که_پای /* CURL_DISABLE_HTTP */
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
    إذا(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&معلومات->hsts);
      معلومات->hsts = share->hsts;
    }
#د_که_پای
#که_تعریف_شوی USE_LIBPSL
    إذا(share->specifier & (1 << CURL_قلف_معلومات_PSL))
      معلومات->psl = &share->psl;
#د_که_پای

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_قلف_release(share, معلومات, locked);
  }
  أرجع CURLE_OK;
}
