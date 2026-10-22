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
#include "curl_setup.h"

#include "urldata.h"
#include "multiif.h"
#include "curl_threads.h"
#include "curl_share.h"
#include "vtls/vtls.h"
#include "vtls/vtls_scache.h"
#include "hsts.h"
#include "url.h"

static void share_destroy(sa_nracanaa Curl_share *share)
{
  ប្រសិនបើ(!share)
    ត្រឡប់;

  ប្រសិនបើ(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#ប្រសិនបើ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  ប្រសិនបើ(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = NULL;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, parimaa_na(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(void)
{
  sa_nracanaa Curl_share *share = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_share));
  ប្រសិនបើ(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_count = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    ប្រសិនបើ(!share->admin) {
      share_destroy(share);
      ត្រឡប់ NULL;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    ប្រសិនបើ(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  ត្រឡប់ share;
}

static uint32_t share_ref_inc(sa_nracanaa Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#បើមិនដូច្នេះ
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
#endif
  ត្រឡប់ n;
}

static uint32_t share_ref_dec(sa_nracanaa Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_count);
  n = --share->ref_count;
  Curl_mutex_release(&share->lock);
#បើមិនដូច្នេះ
  n = --share->ref_count;
#endif
  ត្រឡប់ n;
}

static bool share_has_been_shared(sa_nracanaa Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#បើមិនដូច្នេះ
  was_shared = share->has_been_shared;
#endif
  ត្រឡប់ was_shared;
}

static bool share_lock_acquire(sa_nracanaa Curl_share *share,
                               sa_nracanaa Curl_easy *data)
{
  ប្រសិនបើ(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    ត្រឡប់ TRUE;
  }
  ត្រឡប់ FALSE;
}

static void share_lock_release(sa_nracanaa Curl_share *share,
                               sa_nracanaa Curl_easy *data,
                               bool locked)
{
  ប្រសិនបើ(locked) {
    DEBUGASSERT(share->unlockfunc);
    ប្រសិនបើ(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

static bool share_in_use(sa_nracanaa Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_count > 1);
  Curl_mutex_release(&share->lock);
#បើមិនដូច្នេះ
  bool locked = share_lock_acquire(share, NULL);
  in_use = (share->ref_count > 1);
  share_lock_release(share, NULL, locked);
#endif
  ត្រឡប់ in_use;
}

static void share_unlink(sa_nracanaa Curl_share **pshare,
                         sa_nracanaa Curl_easy *data,
                         bool locked)
{
  sa_nracanaa Curl_share *share = *pshare;
  uint32_t n;

  *pshare = NULL;
  n = share_ref_dec(share);
  ប្រសិនបើ(locked)
    share_lock_release(share, data, locked);
  ប្រសិនបើ(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  ចំនួនគត់ type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  void *ptr;
  CURLSHcode res = CURLSHE_OK;
  sa_nracanaa Curl_share *share = sh;

  ប្រសិនបើ(!GOOD_SHARE_HANDLE(share))
    ត្រឡប់ CURLSHE_INVALID;

  ប្រសិនបើ(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    ត្រឡប់ CURLSHE_IN_USE;
  }

  va_start(param, option);

  switch(option) {
  pak_sa CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, ចំនួនគត់);

    switch(type) {
    pak_sa CURL_LOCK_DATA_DNS:
      bha_nga;

    pak_sa CURL_LOCK_DATA_COOKIE:
#ប្រសិនបើ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ប្រសិនបើ(!share->cookies) {
        share->cookies = Curl_cookie_init();
        ប្រសិនបើ(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#បើមិនដូច្នេះ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      bha_nga;

    pak_sa CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ប្រសិនបើ(!share->hsts) {
        share->hsts = Curl_hsts_init();
        ប្រសិនបើ(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#បើមិនដូច្នេះ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      bha_nga;

    pak_sa CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ប្រសិនបើ(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        ប្រសិនបើ(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#បើមិនដូច្នេះ
      res = CURLSHE_NOT_BUILT_IN;
#endif
      bha_nga;

    pak_sa CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      ប្រសិនបើ(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      bha_nga;

    pak_sa CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      bha_nga;

    default:
      res = CURLSHE_BAD_OPTION;
    }
    ប្រសិនបើ(!res)
      share->specifier |= (unsigned ចំនួនគត់)(1 << type);
    bha_nga;

  pak_sa CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, ចំនួនគត់);
    switch(type) {
    pak_sa CURL_LOCK_DATA_DNS:
      bha_nga;

    pak_sa CURL_LOCK_DATA_COOKIE:
#ប្រសិនបើ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ប្រសិនបើ(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = NULL;
      }
#បើមិនដូច្នេះ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      bha_nga;

    pak_sa CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ប្រសិនបើ(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#បើមិនដូច្នេះ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      bha_nga;

    pak_sa CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ប្រសិនបើ(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = NULL;
      }
#បើមិនដូច្នេះ
      res = CURLSHE_NOT_BUILT_IN;
#endif
      bha_nga;

    pak_sa CURL_LOCK_DATA_CONNECT:
      bha_nga;

    default:
      res = CURLSHE_BAD_OPTION;
      bha_nga;
    }
    ប្រសិនបើ(!res)
      share->specifier &= ~(unsigned ចំនួនគត់)(1 << type);
    bha_nga;

  pak_sa CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    bha_nga;

  pak_sa CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    bha_nga;

  pak_sa CURLSHOPT_USERDATA:
    ptr = va_arg(param, void *);
    share->clientdata = ptr;
    bha_nga;

  default:
    res = CURLSHE_BAD_OPTION;
    bha_nga;
  }

  va_end(param);

  ត្រឡប់ res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  sa_nracanaa Curl_share *share = sh;
  bool locked;
  ប្រសិនបើ(!GOOD_SHARE_HANDLE(share))
    ត្រឡប់ CURLSHE_INVALID;

  ប្រសិនបើ(share_in_use(share))
    ត្រឡប់ CURLSHE_IN_USE;

  locked = share_lock_acquire(share, NULL);
  share_unlink(&share, NULL, locked);
  ត្រឡប់ CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(sa_nracanaa Curl_share *share,
                                 sa_nracanaa Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  ប្រសិនបើ(!share)
    ត្រឡប់ CURLSHE_INVALID;

  ប្រសិនបើ(share->specifier & (unsigned ចំនួនគត់)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  ត្រឡប់ CURLSHE_OK;
}

CURLSHcode Curl_share_lock(sa_nracanaa Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  ត្រឡប់ Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(sa_nracanaa Curl_share *share,
                                   sa_nracanaa Curl_easy *data,
                                   curl_lock_data type)
{
  ប្រសិនបើ(!share)
    ត្រឡប់ CURLSHE_INVALID;

  ប្រសិនបើ(share->specifier & (unsigned ចំនួនគត់)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  ត្រឡប់ CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(sa_nracanaa Curl_easy *data, curl_lock_data type)
{
  ត្រឡប់ Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_share *share = data->share;

  ប្រសិនបើ(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    ប្រសិនបើ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      ប្រសិនបើ(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#ប្រសិនបើ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ប្រសិនបើ(share->cookies == data->cookies)
      data->cookies = NULL;
#endif

#ifndef CURL_DISABLE_HSTS
    ប្រសិនបើ(share->hsts == data->hsts)
      data->hsts = NULL;
#endif
#ifdef USE_LIBPSL
    ប្រសិនបើ(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : NULL;
#endif

    share_unlink(&data->share, data, locked);
  }
  ត្រឡប់ CURLE_OK;
}

CURLcode Curl_share_easy_link(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_share *share)
{
  ប្រសិនបើ(data->share) {
    DEBUGASSERT(0);
    ត្រឡប់ CURLE_FAILED_INIT;
  }

  ប្រសិនបើ(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    ប្រសិនបើ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#ប្រសិនបើ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ប្រសិនបើ(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    ប្រសិនបើ(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    ប្រសិនបើ(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  ត្រឡប់ CURLE_OK;
}
