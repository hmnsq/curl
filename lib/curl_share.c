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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "curl_threads.h"
#അന്തര്ഭാവ "curl_share.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "hsts.h"
#അന്തര്ഭാവ "url.h"

സ്ഥിതം ശൂന്യം share_destroy(ഘടന Curl_share *share)
{
  എങ്കിൽ(!share)
    തിരികെ;

  എങ്കിൽ(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  എങ്കിൽ(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = ശൂന്യ;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, വലുപ്പം(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(ശൂന്യം)
{
  ഘടന Curl_share *share = curlx_calloc(1, വലുപ്പം(ഘടന Curl_share));
  എങ്കിൽ(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_എണ്ണം = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    എങ്കിൽ(!share->admin) {
      share_destroy(share);
      തിരികെ ശൂന്യ;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    എങ്കിൽ(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  തിരികെ share;
}

സ്ഥിതം uint32_t share_ref_inc(ഘടന Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_എണ്ണം;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#അല്ലെങ്കിൽ
  n = ++share->ref_എണ്ണം;
  share->has_been_shared = TRUE;
#endif
  തിരികെ n;
}

സ്ഥിതം uint32_t share_ref_dec(ഘടന Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_എണ്ണം);
  n = --share->ref_എണ്ണം;
  Curl_mutex_release(&share->lock);
#അല്ലെങ്കിൽ
  n = --share->ref_എണ്ണം;
#endif
  തിരികെ n;
}

സ്ഥിതം bool share_has_been_shared(ഘടന Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#അല്ലെങ്കിൽ
  was_shared = share->has_been_shared;
#endif
  തിരികെ was_shared;
}

സ്ഥിതം bool share_lock_acquire(ഘടന Curl_share *share,
                               ഘടന Curl_easy *data)
{
  എങ്കിൽ(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം ശൂന്യം share_lock_release(ഘടന Curl_share *share,
                               ഘടന Curl_easy *data,
                               bool locked)
{
  എങ്കിൽ(locked) {
    DEBUGASSERT(share->unlockfunc);
    എങ്കിൽ(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

സ്ഥിതം bool share_in_use(ഘടന Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_എണ്ണം > 1);
  Curl_mutex_release(&share->lock);
#അല്ലെങ്കിൽ
  bool locked = share_lock_acquire(share, ശൂന്യ);
  in_use = (share->ref_എണ്ണം > 1);
  share_lock_release(share, ശൂന്യ, locked);
#endif
  തിരികെ in_use;
}

സ്ഥിതം ശൂന്യം share_unlink(ഘടന Curl_share **pshare,
                         ഘടന Curl_easy *data,
                         bool locked)
{
  ഘടന Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ശൂന്യ;
  n = share_ref_dec(share);
  എങ്കിൽ(locked)
    share_lock_release(share, data, locked);
  എങ്കിൽ(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  പൂർണ്ണസംഖ്യ type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  ശൂന്യം *ptr;
  CURLSHcode res = CURLSHE_OK;
  ഘടന Curl_share *share = sh;

  എങ്കിൽ(!GOOD_SHARE_HANDLE(share))
    തിരികെ CURLSHE_INVALID;

  എങ്കിൽ(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    തിരികെ CURLSHE_IN_USE;
  }

  va_start(param, option);

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, പൂർണ്ണസംഖ്യ);

    തിരഞ്ഞെടുപ്പ്(type) {
    അവസ്ഥ CURL_LOCK_DATA_DNS:
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_COOKIE:
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      എങ്കിൽ(!share->cookies) {
        share->cookies = Curl_cookie_init();
        എങ്കിൽ(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#അല്ലെങ്കിൽ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      എങ്കിൽ(!share->hsts) {
        share->hsts = Curl_hsts_init();
        എങ്കിൽ(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#അല്ലെങ്കിൽ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      എങ്കിൽ(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        എങ്കിൽ(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#അല്ലെങ്കിൽ
      res = CURLSHE_NOT_BUILT_IN;
#endif
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      എങ്കിൽ(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      res = CURLSHE_BAD_OPTION;
    }
    എങ്കിൽ(!res)
      share->specifier |= (unsigned പൂർണ്ണസംഖ്യ)(1 << type);
    നിർത്തുക;

  അവസ്ഥ CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, പൂർണ്ണസംഖ്യ);
    തിരഞ്ഞെടുപ്പ്(type) {
    അവസ്ഥ CURL_LOCK_DATA_DNS:
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_COOKIE:
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      എങ്കിൽ(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ശൂന്യ;
      }
#അല്ലെങ്കിൽ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      എങ്കിൽ(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#അല്ലെങ്കിൽ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      എങ്കിൽ(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ശൂന്യ;
      }
#അല്ലെങ്കിൽ
      res = CURLSHE_NOT_BUILT_IN;
#endif
      നിർത്തുക;

    അവസ്ഥ CURL_LOCK_DATA_CONNECT:
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      res = CURLSHE_BAD_OPTION;
      നിർത്തുക;
    }
    എങ്കിൽ(!res)
      share->specifier &= ~(unsigned പൂർണ്ണസംഖ്യ)(1 << type);
    നിർത്തുക;

  അവസ്ഥ CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    നിർത്തുക;

  അവസ്ഥ CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    നിർത്തുക;

  അവസ്ഥ CURLSHOPT_USERDATA:
    ptr = va_arg(param, ശൂന്യം *);
    share->clientdata = ptr;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    res = CURLSHE_BAD_OPTION;
    നിർത്തുക;
  }

  va_end(param);

  തിരികെ res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  ഘടന Curl_share *share = sh;
  bool locked;
  എങ്കിൽ(!GOOD_SHARE_HANDLE(share))
    തിരികെ CURLSHE_INVALID;

  എങ്കിൽ(share_in_use(share))
    തിരികെ CURLSHE_IN_USE;

  locked = share_lock_acquire(share, ശൂന്യ);
  share_unlink(&share, ശൂന്യ, locked);
  തിരികെ CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(ഘടന Curl_share *share,
                                 ഘടന Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  എങ്കിൽ(!share)
    തിരികെ CURLSHE_INVALID;

  എങ്കിൽ(share->specifier & (unsigned പൂർണ്ണസംഖ്യ)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  തിരികെ CURLSHE_OK;
}

CURLSHcode Curl_share_lock(ഘടന Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  തിരികെ Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(ഘടന Curl_share *share,
                                   ഘടന Curl_easy *data,
                                   curl_lock_data type)
{
  എങ്കിൽ(!share)
    തിരികെ CURLSHE_INVALID;

  എങ്കിൽ(share->specifier & (unsigned പൂർണ്ണസംഖ്യ)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  തിരികെ CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(ഘടന Curl_easy *data, curl_lock_data type)
{
  തിരികെ Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(ഘടന Curl_easy *data)
{
  ഘടന Curl_share *share = data->share;

  എങ്കിൽ(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    എങ്കിൽ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      എങ്കിൽ(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    എങ്കിൽ(share->cookies == data->cookies)
      data->cookies = ശൂന്യ;
#endif

#ifndef CURL_DISABLE_HSTS
    എങ്കിൽ(share->hsts == data->hsts)
      data->hsts = ശൂന്യ;
#endif
#ifdef USE_LIBPSL
    എങ്കിൽ(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : ശൂന്യ;
#endif

    share_unlink(&data->share, data, locked);
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_share_easy_link(ഘടന Curl_easy *data,
                              ഘടന Curl_share *share)
{
  എങ്കിൽ(data->share) {
    DEBUGASSERT(0);
    തിരികെ CURLE_FAILED_INIT;
  }

  എങ്കിൽ(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    എങ്കിൽ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    എങ്കിൽ(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    എങ്കിൽ(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    എങ്കിൽ(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  തിരികെ CURLE_OK;
}
