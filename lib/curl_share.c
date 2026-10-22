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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "hsts.h"
#ଅନ୍ତର୍ଭାଵ "url.h"

ସ୍ଥିର ଶୂନ୍ୟ share_destroy(ସଂରଚନା Curl_share *share)
{
  ଯଦି(!share)
    ଫେରନ୍ତୁ;

  ଯଦି(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  ଯଦି(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = ଖାଲି;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, ଆକାର(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(ଶୂନ୍ୟ)
{
  ସଂରଚନା Curl_share *share = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_share));
  ଯଦି(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_ଗଣନା = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    ଯଦି(!share->admin) {
      share_destroy(share);
      ଫେରନ୍ତୁ ଖାଲି;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    ଯଦି(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  ଫେରନ୍ତୁ share;
}

ସ୍ଥିର uint32_t share_ref_inc(ସଂରଚନା Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_ଗଣନା;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#ଅନ୍ୟଥା
  n = ++share->ref_ଗଣନା;
  share->has_been_shared = TRUE;
#endif
  ଫେରନ୍ତୁ n;
}

ସ୍ଥିର uint32_t share_ref_dec(ସଂରଚନା Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_ଗଣନା);
  n = --share->ref_ଗଣନା;
  Curl_mutex_release(&share->lock);
#ଅନ୍ୟଥା
  n = --share->ref_ଗଣନା;
#endif
  ଫେରନ୍ତୁ n;
}

ସ୍ଥିର bool share_has_been_shared(ସଂରଚନା Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#ଅନ୍ୟଥା
  was_shared = share->has_been_shared;
#endif
  ଫେରନ୍ତୁ was_shared;
}

ସ୍ଥିର bool share_lock_acquire(ସଂରଚନା Curl_share *share,
                               ସଂରଚନା Curl_easy *data)
{
  ଯଦି(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ share_lock_release(ସଂରଚନା Curl_share *share,
                               ସଂରଚନା Curl_easy *data,
                               bool locked)
{
  ଯଦି(locked) {
    DEBUGASSERT(share->unlockfunc);
    ଯଦି(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

ସ୍ଥିର bool share_in_use(ସଂରଚନା Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_ଗଣନା > 1);
  Curl_mutex_release(&share->lock);
#ଅନ୍ୟଥା
  bool locked = share_lock_acquire(share, ଖାଲି);
  in_use = (share->ref_ଗଣନା > 1);
  share_lock_release(share, ଖାଲି, locked);
#endif
  ଫେରନ୍ତୁ in_use;
}

ସ୍ଥିର ଶୂନ୍ୟ share_unlink(ସଂରଚନା Curl_share **pshare,
                         ସଂରଚନା Curl_easy *data,
                         bool locked)
{
  ସଂରଚନା Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ଖାଲି;
  n = share_ref_dec(share);
  ଯଦି(locked)
    share_lock_release(share, data, locked);
  ଯଦି(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  ଶୂନ୍ୟ *ptr;
  CURLSHcode res = CURLSHE_OK;
  ସଂରଚନା Curl_share *share = sh;

  ଯଦି(!GOOD_SHARE_HANDLE(share))
    ଫେରନ୍ତୁ CURLSHE_INVALID;

  ଯଦି(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    ଫେରନ୍ତୁ CURLSHE_IN_USE;
  }

  va_start(param, option);

  ବାଛ(option) {
  ସ୍ଥିତି CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

    ବାଛ(type) {
    ସ୍ଥିତି CURL_LOCK_DATA_DNS:
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_COOKIE:
#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ଯଦି(!share->cookies) {
        share->cookies = Curl_cookie_init();
        ଯଦି(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#ଅନ୍ୟଥା /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ଯଦି(!share->hsts) {
        share->hsts = Curl_hsts_init();
        ଯଦି(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#ଅନ୍ୟଥା /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ଯଦି(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        ଯଦି(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#ଅନ୍ୟଥା
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      ଯଦି(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      res = CURLSHE_BAD_OPTION;
    }
    ଯଦି(!res)
      share->specifier |= (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(1 << type);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
    ବାଛ(type) {
    ସ୍ଥିତି CURL_LOCK_DATA_DNS:
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_COOKIE:
#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ଯଦି(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ଖାଲି;
      }
#ଅନ୍ୟଥା /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      ଯଦି(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#ଅନ୍ୟଥା /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      ଯଦି(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ଖାଲି;
      }
#ଅନ୍ୟଥା
      res = CURLSHE_NOT_BUILT_IN;
#endif
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_LOCK_DATA_CONNECT:
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      res = CURLSHE_BAD_OPTION;
      ଭାଙ୍ଗ;
    }
    ଯଦି(!res)
      share->specifier &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(1 << type);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURLSHOPT_USERDATA:
    ptr = va_arg(param, ଶୂନ୍ୟ *);
    share->clientdata = ptr;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    res = CURLSHE_BAD_OPTION;
    ଭାଙ୍ଗ;
  }

  va_end(param);

  ଫେରନ୍ତୁ res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  ସଂରଚନା Curl_share *share = sh;
  bool locked;
  ଯଦି(!GOOD_SHARE_HANDLE(share))
    ଫେରନ୍ତୁ CURLSHE_INVALID;

  ଯଦି(share_in_use(share))
    ଫେରନ୍ତୁ CURLSHE_IN_USE;

  locked = share_lock_acquire(share, ଖାଲି);
  share_unlink(&share, ଖାଲି, locked);
  ଫେରନ୍ତୁ CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(ସଂରଚନା Curl_share *share,
                                 ସଂରଚନା Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  ଯଦି(!share)
    ଫେରନ୍ତୁ CURLSHE_INVALID;

  ଯଦି(share->specifier & (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  ଫେରନ୍ତୁ CURLSHE_OK;
}

CURLSHcode Curl_share_lock(ସଂରଚନା Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  ଫେରନ୍ତୁ Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(ସଂରଚନା Curl_share *share,
                                   ସଂରଚନା Curl_easy *data,
                                   curl_lock_data type)
{
  ଯଦି(!share)
    ଫେରନ୍ତୁ CURLSHE_INVALID;

  ଯଦି(share->specifier & (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  ଫେରନ୍ତୁ CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(ସଂରଚନା Curl_easy *data, curl_lock_data type)
{
  ଫେରନ୍ତୁ Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_share *share = data->share;

  ଯଦି(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    ଯଦି((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      ଯଦି(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ଯଦି(share->cookies == data->cookies)
      data->cookies = ଖାଲି;
#endif

#ifndef CURL_DISABLE_HSTS
    ଯଦି(share->hsts == data->hsts)
      data->hsts = ଖାଲି;
#endif
#ifdef USE_LIBPSL
    ଯଦି(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : ଖାଲି;
#endif

    share_unlink(&data->share, data, locked);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_share_easy_link(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_share *share)
{
  ଯଦି(data->share) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ଯଦି(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    ଯଦି((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ଯଦି(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    ଯଦି(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    ଯଦି(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}
