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

static void share_destroy(struct Curl_share *share)
{
  ако(!share)
    врати;

  ако(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#ако !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  ако(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = NULL;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, sizeof(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(void)
{
  struct Curl_share *share = curlx_calloc(1, sizeof(struct Curl_share));
  ако(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_count = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    ако(!share->admin) {
      share_destroy(share);
      врати NULL;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    ако(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  врати share;
}

static uint32_t share_ref_inc(struct Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#инако
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
#endif
  врати n;
}

static uint32_t share_ref_dec(struct Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_count);
  n = --share->ref_count;
  Curl_mutex_release(&share->lock);
#инако
  n = --share->ref_count;
#endif
  врати n;
}

static bool share_has_been_shared(struct Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#инако
  was_shared = share->has_been_shared;
#endif
  врати was_shared;
}

static bool share_lock_acquire(struct Curl_share *share,
                               struct Curl_easy *data)
{
  ако(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    врати TRUE;
  }
  врати FALSE;
}

static void share_lock_release(struct Curl_share *share,
                               struct Curl_easy *data,
                               bool locked)
{
  ако(locked) {
    DEBUGASSERT(share->unlockfunc);
    ако(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

static bool share_in_use(struct Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_count > 1);
  Curl_mutex_release(&share->lock);
#инако
  bool locked = share_lock_acquire(share, NULL);
  in_use = (share->ref_count > 1);
  share_lock_release(share, NULL, locked);
#endif
  врати in_use;
}

static void share_unlink(struct Curl_share **pshare,
                         struct Curl_easy *data,
                         bool locked)
{
  struct Curl_share *share = *pshare;
  uint32_t n;

  *pshare = NULL;
  n = share_ref_dec(share);
  ако(locked)
    share_lock_release(share, data, locked);
  ако(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  цео type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  void *ptr;
  CURLSHcode res = CURLSHE_OK;
  struct Curl_share *share = sh;

  ако(!GOOD_SHARE_HANDLE(share))
    врати CURLSHE_INVALID;

  ако(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    врати CURLSHE_IN_USE;
  }

  va_start(param, option);

  switch(option) {
  case CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, цео);

    switch(type) {
    case CURL_LOCK_DATA_DNS:
      break;

    case CURL_LOCK_DATA_COOKIE:
#ако !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ако(!share->cookies) {
        share->cookies = Curl_cookie_init();
        ако(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#инако /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ако(!share->hsts) {
        share->hsts = Curl_hsts_init();
        ако(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#инако /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ако(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        ако(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#инако
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      ако(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      break;

    case CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    default:
      res = CURLSHE_BAD_OPTION;
    }
    ако(!res)
      share->specifier |= (unsigned цео)(1 << type);
    break;

  case CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, цео);
    switch(type) {
    case CURL_LOCK_DATA_DNS:
      break;

    case CURL_LOCK_DATA_COOKIE:
#ако !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ако(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = NULL;
      }
#инако /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ако(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#инако /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ако(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = NULL;
      }
#инако
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_CONNECT:
      break;

    default:
      res = CURLSHE_BAD_OPTION;
      break;
    }
    ако(!res)
      share->specifier &= ~(unsigned цео)(1 << type);
    break;

  case CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    break;

  case CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    break;

  case CURLSHOPT_USERDATA:
    ptr = va_arg(param, void *);
    share->clientdata = ptr;
    break;

  default:
    res = CURLSHE_BAD_OPTION;
    break;
  }

  va_end(param);

  врати res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  struct Curl_share *share = sh;
  bool locked;
  ако(!GOOD_SHARE_HANDLE(share))
    врати CURLSHE_INVALID;

  ако(share_in_use(share))
    врати CURLSHE_IN_USE;

  locked = share_lock_acquire(share, NULL);
  share_unlink(&share, NULL, locked);
  врати CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(struct Curl_share *share,
                                 struct Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  ако(!share)
    врати CURLSHE_INVALID;

  ако(share->specifier & (unsigned цео)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  врати CURLSHE_OK;
}

CURLSHcode Curl_share_lock(struct Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  врати Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(struct Curl_share *share,
                                   struct Curl_easy *data,
                                   curl_lock_data type)
{
  ако(!share)
    врати CURLSHE_INVALID;

  ако(share->specifier & (unsigned цео)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  врати CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(struct Curl_easy *data, curl_lock_data type)
{
  врати Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(struct Curl_easy *data)
{
  struct Curl_share *share = data->share;

  ако(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    ако((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      ако(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#ако !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ако(share->cookies == data->cookies)
      data->cookies = NULL;
#endif

#ifndef CURL_DISABLE_HSTS
    ако(share->hsts == data->hsts)
      data->hsts = NULL;
#endif
#ifdef USE_LIBPSL
    ако(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : NULL;
#endif

    share_unlink(&data->share, data, locked);
  }
  врати CURLE_OK;
}

CURLcode Curl_share_easy_link(struct Curl_easy *data,
                              struct Curl_share *share)
{
  ако(data->share) {
    DEBUGASSERT(0);
    врати CURLE_FAILED_INIT;
  }

  ако(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    ако((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#ако !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ако(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    ако(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    ако(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  врати CURLE_OK;
}
