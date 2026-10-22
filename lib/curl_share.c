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

ساکن خالی share_destroy(ساخت Curl_share *share)
{
  اگر(!share)
    واپس;

  اگر(share->specifier & (1 << CURL_تالا_ڈیٹا_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#اگر_ختم

#اگر_تعریف_شدہ USE_SSL
  اگر(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = تہی;
  }
#اگر_ختم

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#اگر_تعریف_شدہ USE_MUTEX
  Curl_mutex_destroy(&share->تالا);
#اگر_ختم
  curlx_memzero(share, حجم(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(خالی)
{
  ساخت Curl_share *share = curlx_calloc(1, حجم(ساخت Curl_share));
  اگر(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_تالا_ڈیٹا_SHARE);
#اگر_تعریف_شدہ USE_MUTEX
    Curl_mutex_init(&share->تالا);
#اگر_ختم
    share->ref_گنتی = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    اگر(!share->admin) {
      share_destroy(share);
      واپس تہی;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.اندرونی_2 = TRUE;
#اگر_تعریف_شدہ DEBUGBUILD
    اگر(getenv("CURL_DEBUG"))
      share->admin->سیٹ.verbose = TRUE;
#اگر_ختم
  }
  واپس share;
}

ساکن uint32_t share_ref_inc(ساخت Curl_share *share)
{
  uint32_t n;
#اگر_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->تالا);
  n = ++share->ref_گنتی;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->تالا);
#ورنہ
  n = ++share->ref_گنتی;
  share->has_been_shared = TRUE;
#اگر_ختم
  واپس n;
}

ساکن uint32_t share_ref_dec(ساخت Curl_share *share)
{
  uint32_t n;
#اگر_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->تالا);
  DEBUGASSERT(share->ref_گنتی);
  n = --share->ref_گنتی;
  Curl_mutex_release(&share->تالا);
#ورنہ
  n = --share->ref_گنتی;
#اگر_ختم
  واپس n;
}

ساکن bool share_has_been_shared(ساخت Curl_share *share)
{
  bool was_shared;
#اگر_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->تالا);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->تالا);
#ورنہ
  was_shared = share->has_been_shared;
#اگر_ختم
  واپس was_shared;
}

ساکن bool share_تالا_acquire(ساخت Curl_share *share,
                               ساخت Curl_easy *ڈیٹا)
{
  اگر(share->lockfunc && share->unlockfunc &&
     (ڈیٹا || share_has_been_shared(share))) {
    share->lockfunc(ڈیٹا, CURL_تالا_ڈیٹا_SHARE, CURL_تالا_ACCESS_SINGLE,
                    share->clientdata);
    واپس TRUE;
  }
  واپس FALSE;
}

ساکن خالی share_تالا_release(ساخت Curl_share *share,
                               ساخت Curl_easy *ڈیٹا,
                               bool locked)
{
  اگر(locked) {
    DEBUGASSERT(share->unlockfunc);
    اگر(share->unlockfunc)
      share->unlockfunc(ڈیٹا, CURL_تالا_ڈیٹا_SHARE, share->clientdata);
  }
}

ساکن bool share_اندر_use(ساخت Curl_share *share)
{
  bool اندر_use;
#اگر_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&share->تالا);
  اندر_use = (share->ref_گنتی > 1);
  Curl_mutex_release(&share->تالا);
#ورنہ
  bool locked = share_تالا_acquire(share, تہی);
  اندر_use = (share->ref_گنتی > 1);
  share_تالا_release(share, تہی, locked);
#اگر_ختم
  واپس اندر_use;
}

ساکن خالی share_unlink(ساخت Curl_share **pshare,
                         ساخت Curl_easy *ڈیٹا,
                         bool locked)
{
  ساخت Curl_share *share = *pshare;
  uint32_t n;

  *pshare = تہی;
  n = share_ref_dec(share);
  اگر(locked)
    share_تالا_release(share, ڈیٹا, locked);
  اگر(!n)  /* last reference gone */
    share_destroy(share);
}

#تعریف_منسوخ curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  صحیح type;
  curl_تالا_function lockfunc;
  curl_unlock_function unlockfunc;
  خالی *ptr;
  CURLSHcode res = CURLSHE_OK;
  ساخت Curl_share *share = sh;

  اگر(!GOOD_SHARE_HANDLE(share))
    واپس CURLSHE_INVALID;

  اگر(share_اندر_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    واپس CURLSHE_اندر_USE;
  }

  va_start(param, option);

  انتخاب(option) {
  حالت CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, صحیح);

    انتخاب(type) {
    حالت CURL_تالا_ڈیٹا_DNS:
      توڑو;

    حالت CURL_تالا_ڈیٹا_COOKIE:
#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      اگر(!share->cookies) {
        share->cookies = Curl_cookie_init();
        اگر(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#ورنہ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_اندر;
#اگر_ختم
      توڑو;

    حالت CURL_تالا_ڈیٹا_HSTS:
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
      اگر(!share->hsts) {
        share->hsts = Curl_hsts_init();
        اگر(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#ورنہ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_اندر;
#اگر_ختم
      توڑو;

    حالت CURL_تالا_ڈیٹا_SSL_SESSION:
#اگر_تعریف_شدہ USE_SSL
      اگر(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        اگر(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#ورنہ
      res = CURLSHE_NOT_BUILT_اندر;
#اگر_ختم
      توڑو;

    حالت CURL_تالا_ڈیٹا_CONNECT:
      /* It is safe to set this option several times on a share. */
      اگر(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      توڑو;

    حالت CURL_تالا_ڈیٹا_PSL:
#اگر_غیر_تعریف_شدہ USE_LIBPSL
      res = CURLSHE_NOT_BUILT_اندر;
#اگر_ختم
      توڑو;

    طےشدہ:
      res = CURLSHE_BAD_OPTION;
    }
    اگر(!res)
      share->specifier |= (غیرعلامتی صحیح)(1 << type);
    توڑو;

  حالت CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, صحیح);
    انتخاب(type) {
    حالت CURL_تالا_ڈیٹا_DNS:
      توڑو;

    حالت CURL_تالا_ڈیٹا_COOKIE:
#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      اگر(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = تہی;
      }
#ورنہ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_اندر;
#اگر_ختم
      توڑو;

    حالت CURL_تالا_ڈیٹا_HSTS:
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
      اگر(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#ورنہ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_اندر;
#اگر_ختم
      توڑو;

    حالت CURL_تالا_ڈیٹا_SSL_SESSION:
#اگر_تعریف_شدہ USE_SSL
      اگر(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = تہی;
      }
#ورنہ
      res = CURLSHE_NOT_BUILT_اندر;
#اگر_ختم
      توڑو;

    حالت CURL_تالا_ڈیٹا_CONNECT:
      توڑو;

    طےشدہ:
      res = CURLSHE_BAD_OPTION;
      توڑو;
    }
    اگر(!res)
      share->specifier &= ~(غیرعلامتی صحیح)(1 << type);
    توڑو;

  حالت CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_تالا_function);
    share->lockfunc = lockfunc;
    توڑو;

  حالت CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    توڑو;

  حالت CURLSHOPT_USERDATA:
    ptr = va_arg(param, خالی *);
    share->clientdata = ptr;
    توڑو;

  طےشدہ:
    res = CURLSHE_BAD_OPTION;
    توڑو;
  }

  va_end(param);

  واپس res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  ساخت Curl_share *share = sh;
  bool locked;
  اگر(!GOOD_SHARE_HANDLE(share))
    واپس CURLSHE_INVALID;

  اگر(share_اندر_use(share))
    واپس CURLSHE_اندر_USE;

  locked = share_تالا_acquire(share, تہی);
  share_unlink(&share, تہی, locked);
  واپس CURLSHE_OK;
}

CURLSHcode Curl_share_تالا_share(ساخت Curl_share *share,
                                 ساخت Curl_easy *ڈیٹا,
                                 curl_تالا_ڈیٹا type,
                                 curl_تالا_access accesstype)
{
  اگر(!share)
    واپس CURLSHE_INVALID;

  اگر(share->specifier & (غیرعلامتی صحیح)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(ڈیٹا, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  واپس CURLSHE_OK;
}

CURLSHcode Curl_share_تالا(ساخت Curl_easy *ڈیٹا, curl_تالا_ڈیٹا type,
                           curl_تالا_access accesstype)
{
  واپس Curl_share_تالا_share(ڈیٹا->share, ڈیٹا, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(ساخت Curl_share *share,
                                   ساخت Curl_easy *ڈیٹا,
                                   curl_تالا_ڈیٹا type)
{
  اگر(!share)
    واپس CURLSHE_INVALID;

  اگر(share->specifier & (غیرعلامتی صحیح)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(ڈیٹا, type, share->clientdata);

  واپس CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(ساخت Curl_easy *ڈیٹا, curl_تالا_ڈیٹا type)
{
  واپس Curl_share_unlock_share(ڈیٹا->share, ڈیٹا, type);
}

CURLcode Curl_share_easy_unlink(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_share *share = ڈیٹا->share;

  اگر(share) {
    bool locked = share_تالا_acquire(share, ڈیٹا);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    اگر((share->specifier & (1 << CURL_تالا_ڈیٹا_CONNECT))) {
      اگر(ڈیٹا->conn)
        Curl_detach_connection(ڈیٹا);
      ڈیٹا->state.آخری_conn_id = -1;
      ڈیٹا->state.آخری_cpid = UINT32_MAX;
    }

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    اگر(share->cookies == ڈیٹا->cookies)
      ڈیٹا->cookies = تہی;
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
    اگر(share->hsts == ڈیٹا->hsts)
      ڈیٹا->hsts = تہی;
#اگر_ختم
#اگر_تعریف_شدہ USE_LIBPSL
    اگر(&share->psl == ڈیٹا->psl)
      ڈیٹا->psl = ڈیٹا->multi ? &ڈیٹا->multi->psl : تہی;
#اگر_ختم

    share_unlink(&ڈیٹا->share, ڈیٹا, locked);
  }
  واپس CURLE_OK;
}

CURLcode Curl_share_easy_link(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_share *share)
{
  اگر(ڈیٹا->share) {
    DEBUGASSERT(0);
    واپس CURLE_FAILED_INIT;
  }

  اگر(share) {
    bool locked = share_تالا_acquire(share, ڈیٹا);

    /* Forget identifiers from the previously used connection cache. */
    اگر((share->specifier & (1 << CURL_تالا_ڈیٹا_CONNECT))) {
      ڈیٹا->state.آخری_conn_id = -1;
      ڈیٹا->state.آخری_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    ڈیٹا->share = share;

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    اگر(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(ڈیٹا->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      ڈیٹا->cookies = share->cookies;
    }
#اگر_ختم /* CURL_DISABLE_HTTP */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
    اگر(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&ڈیٹا->hsts);
      ڈیٹا->hsts = share->hsts;
    }
#اگر_ختم
#اگر_تعریف_شدہ USE_LIBPSL
    اگر(share->specifier & (1 << CURL_تالا_ڈیٹا_PSL))
      ڈیٹا->psl = &share->psl;
#اگر_ختم

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_تالا_release(share, ڈیٹا, locked);
  }
  واپس CURLE_OK;
}
