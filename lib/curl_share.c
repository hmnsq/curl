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

  إذا(share->specifier & (1 << CURL_تالو_معلومات_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SSL
  إذا(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = عدم;
  }
#جيڪڏهن_پورو

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#جيڪڏهن_وصف_ٿيل USE_MUTEX
  Curl_mutex_destroy(&share->تالو);
#جيڪڏهن_پورو
  curlx_memzero(share, حجم(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(فراغ)
{
  بنية Curl_share *share = curlx_calloc(1, حجم(بنية Curl_share));
  إذا(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_تالو_معلومات_SHARE);
#جيڪڏهن_وصف_ٿيل USE_MUTEX
    Curl_mutex_init(&share->تالو);
#جيڪڏهن_پورو
    share->ref_ڳڻپ = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    إذا(!share->admin) {
      share_destroy(share);
      أرجع عدم;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.اندروني = TRUE;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    إذا(getenv("CURL_DEBUG"))
      share->admin->سيٽ.verbose = TRUE;
#جيڪڏهن_پورو
  }
  أرجع share;
}

ساكن uint32_t share_ref_inc(بنية Curl_share *share)
{
  uint32_t n;
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  Curl_mutex_acquire(&share->تالو);
  n = ++share->ref_ڳڻپ;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->تالو);
#وإلا
  n = ++share->ref_ڳڻپ;
  share->has_been_shared = TRUE;
#جيڪڏهن_پورو
  أرجع n;
}

ساكن uint32_t share_ref_dec(بنية Curl_share *share)
{
  uint32_t n;
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  Curl_mutex_acquire(&share->تالو);
  DEBUGASSERT(share->ref_ڳڻپ);
  n = --share->ref_ڳڻپ;
  Curl_mutex_release(&share->تالو);
#وإلا
  n = --share->ref_ڳڻپ;
#جيڪڏهن_پورو
  أرجع n;
}

ساكن bool share_has_been_shared(بنية Curl_share *share)
{
  bool was_shared;
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  Curl_mutex_acquire(&share->تالو);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->تالو);
#وإلا
  was_shared = share->has_been_shared;
#جيڪڏهن_پورو
  أرجع was_shared;
}

ساكن bool share_تالو_acquire(بنية Curl_share *share,
                               بنية Curl_easy *معلومات)
{
  إذا(share->lockfunc && share->unlockfunc &&
     (معلومات || share_has_been_shared(share))) {
    share->lockfunc(معلومات, CURL_تالو_معلومات_SHARE, CURL_تالو_ACCESS_SINGLE,
                    share->clientdata);
    أرجع TRUE;
  }
  أرجع FALSE;
}

ساكن فراغ share_تالو_release(بنية Curl_share *share,
                               بنية Curl_easy *معلومات,
                               bool locked)
{
  إذا(locked) {
    DEBUGASSERT(share->unlockfunc);
    إذا(share->unlockfunc)
      share->unlockfunc(معلومات, CURL_تالو_معلومات_SHARE, share->clientdata);
  }
}

ساكن bool share_اندريون_use(بنية Curl_share *share)
{
  bool اندريون_use;
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  Curl_mutex_acquire(&share->تالو);
  اندريون_use = (share->ref_ڳڻپ > 1);
  Curl_mutex_release(&share->تالو);
#وإلا
  bool locked = share_تالو_acquire(share, عدم);
  اندريون_use = (share->ref_ڳڻپ > 1);
  share_تالو_release(share, عدم, locked);
#جيڪڏهن_پورو
  أرجع اندريون_use;
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
    share_تالو_release(share, معلومات, locked);
  إذا(!n)  /* last reference gone */
    share_destroy(share);
}

#وصف_رد curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  صحيح type;
  curl_تالو_function lockfunc;
  curl_unlock_function unlockfunc;
  فراغ *ptr;
  CURLSHcode res = CURLSHE_OK;
  بنية Curl_share *share = sh;

  إذا(!GOOD_SHARE_HANDLE(share))
    أرجع CURLSHE_INVALID;

  إذا(share_اندريون_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    أرجع CURLSHE_اندريون_USE;
  }

  va_start(param, option);

  بدل(option) {
  حالة CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, صحيح);

    بدل(type) {
    حالة CURL_تالو_معلومات_DNS:
      اكسر;

    حالة CURL_تالو_معلومات_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(!share->cookies) {
        share->cookies = Curl_cookie_init();
        إذا(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
      اكسر;

    حالة CURL_تالو_معلومات_HSTS:
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
      إذا(!share->hsts) {
        share->hsts = Curl_hsts_init();
        إذا(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
      اكسر;

    حالة CURL_تالو_معلومات_SSL_SESSION:
#جيڪڏهن_وصف_ٿيل USE_SSL
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
      res = CURLSHE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
      اكسر;

    حالة CURL_تالو_معلومات_CONNECT:
      /* It is safe to set this option several times on a share. */
      إذا(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      اكسر;

    حالة CURL_تالو_معلومات_PSL:
#جيڪڏهن_وصف_نه_ٿيل USE_LIBPSL
      res = CURLSHE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
    }
    إذا(!res)
      share->specifier |= (بنانشان صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, صحيح);
    بدل(type) {
    حالة CURL_تالو_معلومات_DNS:
      اكسر;

    حالة CURL_تالو_معلومات_COOKIE:
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      إذا(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = عدم;
      }
#وإلا /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
      اكسر;

    حالة CURL_تالو_معلومات_HSTS:
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
      إذا(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#وإلا /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
      اكسر;

    حالة CURL_تالو_معلومات_SSL_SESSION:
#جيڪڏهن_وصف_ٿيل USE_SSL
      إذا(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = عدم;
      }
#وإلا
      res = CURLSHE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
      اكسر;

    حالة CURL_تالو_معلومات_CONNECT:
      اكسر;

    افتراضي:
      res = CURLSHE_BAD_OPTION;
      اكسر;
    }
    إذا(!res)
      share->specifier &= ~(بنانشان صحيح)(1 << type);
    اكسر;

  حالة CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_تالو_function);
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

  إذا(share_اندريون_use(share))
    أرجع CURLSHE_اندريون_USE;

  locked = share_تالو_acquire(share, عدم);
  share_unlink(&share, عدم, locked);
  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_تالو_share(بنية Curl_share *share,
                                 بنية Curl_easy *معلومات,
                                 curl_تالو_معلومات type,
                                 curl_تالو_access accesstype)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بنانشان صحيح)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(معلومات, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_تالو(بنية Curl_easy *معلومات, curl_تالو_معلومات type,
                           curl_تالو_access accesstype)
{
  أرجع Curl_share_تالو_share(معلومات->share, معلومات, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(بنية Curl_share *share,
                                   بنية Curl_easy *معلومات,
                                   curl_تالو_معلومات type)
{
  إذا(!share)
    أرجع CURLSHE_INVALID;

  إذا(share->specifier & (بنانشان صحيح)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(معلومات, type, share->clientdata);

  أرجع CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(بنية Curl_easy *معلومات, curl_تالو_معلومات type)
{
  أرجع Curl_share_unlock_share(معلومات->share, معلومات, type);
}

CURLcode Curl_share_easy_unlink(بنية Curl_easy *معلومات)
{
  بنية Curl_share *share = معلومات->share;

  إذا(share) {
    bool locked = share_تالو_acquire(share, معلومات);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    إذا((share->specifier & (1 << CURL_تالو_معلومات_CONNECT))) {
      إذا(معلومات->conn)
        Curl_detach_connection(معلومات);
      معلومات->state.آخري_conn_id = -1;
      معلومات->state.آخري_cpid = UINT32_MAX;
    }

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    إذا(share->cookies == معلومات->cookies)
      معلومات->cookies = عدم;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
    إذا(share->hsts == معلومات->hsts)
      معلومات->hsts = عدم;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_LIBPSL
    إذا(&share->psl == معلومات->psl)
      معلومات->psl = معلومات->multi ? &معلومات->multi->psl : عدم;
#جيڪڏهن_پورو

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
    bool locked = share_تالو_acquire(share, معلومات);

    /* Forget identifiers from the previously used connection cache. */
    إذا((share->specifier & (1 << CURL_تالو_معلومات_CONNECT))) {
      معلومات->state.آخري_conn_id = -1;
      معلومات->state.آخري_cpid = UINT32_MAX;
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
#جيڪڏهن_پورو /* CURL_DISABLE_HTTP */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
    إذا(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&معلومات->hsts);
      معلومات->hsts = share->hsts;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_LIBPSL
    إذا(share->specifier & (1 << CURL_تالو_معلومات_PSL))
      معلومات->psl = &share->psl;
#جيڪڏهن_پورو

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_تالو_release(share, معلومات, locked);
  }
  أرجع CURLE_OK;
}
