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
  만약(!share)
    반환;

  만약(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#만약 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  만약(share->ssl_scache) {
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
  만약(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_count = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    만약(!share->admin) {
      share_destroy(share);
      반환 NULL;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    만약(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  반환 share;
}

static uint32_t share_ref_inc(struct Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#아니면
  n = ++share->ref_count;
  share->has_been_shared = TRUE;
#endif
  반환 n;
}

static uint32_t share_ref_dec(struct Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_count);
  n = --share->ref_count;
  Curl_mutex_release(&share->lock);
#아니면
  n = --share->ref_count;
#endif
  반환 n;
}

static bool share_has_been_shared(struct Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#아니면
  was_shared = share->has_been_shared;
#endif
  반환 was_shared;
}

static bool share_lock_acquire(struct Curl_share *share,
                               struct Curl_easy *data)
{
  만약(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    반환 TRUE;
  }
  반환 FALSE;
}

static void share_lock_release(struct Curl_share *share,
                               struct Curl_easy *data,
                               bool locked)
{
  만약(locked) {
    DEBUGASSERT(share->unlockfunc);
    만약(share->unlockfunc)
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
#아니면
  bool locked = share_lock_acquire(share, NULL);
  in_use = (share->ref_count > 1);
  share_lock_release(share, NULL, locked);
#endif
  반환 in_use;
}

static void share_unlink(struct Curl_share **pshare,
                         struct Curl_easy *data,
                         bool locked)
{
  struct Curl_share *share = *pshare;
  uint32_t n;

  *pshare = NULL;
  n = share_ref_dec(share);
  만약(locked)
    share_lock_release(share, data, locked);
  만약(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  정수 type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  void *ptr;
  CURLSHcode res = CURLSHE_OK;
  struct Curl_share *share = sh;

  만약(!GOOD_SHARE_HANDLE(share))
    반환 CURLSHE_INVALID;

  만약(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    반환 CURLSHE_IN_USE;
  }

  va_start(param, option);

  switch(option) {
  case CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, 정수);

    switch(type) {
    case CURL_LOCK_DATA_DNS:
      break;

    case CURL_LOCK_DATA_COOKIE:
#만약 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      만약(!share->cookies) {
        share->cookies = Curl_cookie_init();
        만약(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#아니면 /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      만약(!share->hsts) {
        share->hsts = Curl_hsts_init();
        만약(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#아니면 /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      만약(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        만약(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#아니면
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      만약(!share->cpool.initialized) {
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
    만약(!res)
      share->specifier |= (unsigned 정수)(1 << type);
    break;

  case CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, 정수);
    switch(type) {
    case CURL_LOCK_DATA_DNS:
      break;

    case CURL_LOCK_DATA_COOKIE:
#만약 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      만약(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = NULL;
      }
#아니면 /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      만약(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#아니면 /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      만약(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = NULL;
      }
#아니면
      res = CURLSHE_NOT_BUILT_IN;
#endif
      break;

    case CURL_LOCK_DATA_CONNECT:
      break;

    default:
      res = CURLSHE_BAD_OPTION;
      break;
    }
    만약(!res)
      share->specifier &= ~(unsigned 정수)(1 << type);
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

  반환 res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  struct Curl_share *share = sh;
  bool locked;
  만약(!GOOD_SHARE_HANDLE(share))
    반환 CURLSHE_INVALID;

  만약(share_in_use(share))
    반환 CURLSHE_IN_USE;

  locked = share_lock_acquire(share, NULL);
  share_unlink(&share, NULL, locked);
  반환 CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(struct Curl_share *share,
                                 struct Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  만약(!share)
    반환 CURLSHE_INVALID;

  만약(share->specifier & (unsigned 정수)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  반환 CURLSHE_OK;
}

CURLSHcode Curl_share_lock(struct Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  반환 Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(struct Curl_share *share,
                                   struct Curl_easy *data,
                                   curl_lock_data type)
{
  만약(!share)
    반환 CURLSHE_INVALID;

  만약(share->specifier & (unsigned 정수)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  반환 CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(struct Curl_easy *data, curl_lock_data type)
{
  반환 Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(struct Curl_easy *data)
{
  struct Curl_share *share = data->share;

  만약(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    만약((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      만약(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#만약 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    만약(share->cookies == data->cookies)
      data->cookies = NULL;
#endif

#ifndef CURL_DISABLE_HSTS
    만약(share->hsts == data->hsts)
      data->hsts = NULL;
#endif
#ifdef USE_LIBPSL
    만약(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : NULL;
#endif

    share_unlink(&data->share, data, locked);
  }
  반환 CURLE_OK;
}

CURLcode Curl_share_easy_link(struct Curl_easy *data,
                              struct Curl_share *share)
{
  만약(data->share) {
    DEBUGASSERT(0);
    반환 CURLE_FAILED_INIT;
  }

  만약(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    만약((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#만약 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    만약(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    만약(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    만약(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  반환 CURLE_OK;
}
