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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curl_threads.h"
#અન્તર્ભાવ "curl_share.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "hsts.h"
#અન્તર્ભાવ "url.h"

સ્થિર શૂન્ય share_destroy(સંરચના Curl_share *share)
{
  જો(!share)
    પાછા;

  જો(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  જો(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = ખાલી;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, કદ(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(શૂન્ય)
{
  સંરચના Curl_share *share = curlx_calloc(1, કદ(સંરચના Curl_share));
  જો(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_ગણતરી = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    જો(!share->admin) {
      share_destroy(share);
      પાછા ખાલી;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    જો(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  પાછા share;
}

સ્થિર uint32_t share_ref_inc(સંરચના Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_ગણતરી;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#નહીંતર
  n = ++share->ref_ગણતરી;
  share->has_been_shared = TRUE;
#endif
  પાછા n;
}

સ્થિર uint32_t share_ref_dec(સંરચના Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_ગણતરી);
  n = --share->ref_ગણતરી;
  Curl_mutex_release(&share->lock);
#નહીંતર
  n = --share->ref_ગણતરી;
#endif
  પાછા n;
}

સ્થિર bool share_has_been_shared(સંરચના Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#નહીંતર
  was_shared = share->has_been_shared;
#endif
  પાછા was_shared;
}

સ્થિર bool share_lock_acquire(સંરચના Curl_share *share,
                               સંરચના Curl_easy *data)
{
  જો(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    પાછા TRUE;
  }
  પાછા FALSE;
}

સ્થિર શૂન્ય share_lock_release(સંરચના Curl_share *share,
                               સંરચના Curl_easy *data,
                               bool locked)
{
  જો(locked) {
    DEBUGASSERT(share->unlockfunc);
    જો(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

સ્થિર bool share_in_use(સંરચના Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_ગણતરી > 1);
  Curl_mutex_release(&share->lock);
#નહીંતર
  bool locked = share_lock_acquire(share, ખાલી);
  in_use = (share->ref_ગણતરી > 1);
  share_lock_release(share, ખાલી, locked);
#endif
  પાછા in_use;
}

સ્થિર શૂન્ય share_unlink(સંરચના Curl_share **pshare,
                         સંરચના Curl_easy *data,
                         bool locked)
{
  સંરચના Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ખાલી;
  n = share_ref_dec(share);
  જો(locked)
    share_lock_release(share, data, locked);
  જો(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  પૂર્ણાંક type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  શૂન્ય *ptr;
  CURLSHcode res = CURLSHE_OK;
  સંરચના Curl_share *share = sh;

  જો(!GOOD_SHARE_HANDLE(share))
    પાછા CURLSHE_INVALID;

  જો(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    પાછા CURLSHE_IN_USE;
  }

  va_start(param, option);

  પસંદગી(option) {
  સ્થિતિ CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, પૂર્ણાંક);

    પસંદગી(type) {
    સ્થિતિ CURL_LOCK_DATA_DNS:
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_COOKIE:
#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      જો(!share->cookies) {
        share->cookies = Curl_cookie_init();
        જો(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#નહીંતર /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      જો(!share->hsts) {
        share->hsts = Curl_hsts_init();
        જો(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#નહીંતર /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      જો(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        જો(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#નહીંતર
      res = CURLSHE_NOT_BUILT_IN;
#endif
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      જો(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      તોડો;

    મૂળભૂત:
      res = CURLSHE_BAD_OPTION;
    }
    જો(!res)
      share->specifier |= (unsigned પૂર્ણાંક)(1 << type);
    તોડો;

  સ્થિતિ CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, પૂર્ણાંક);
    પસંદગી(type) {
    સ્થિતિ CURL_LOCK_DATA_DNS:
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_COOKIE:
#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      જો(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ખાલી;
      }
#નહીંતર /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      જો(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#નહીંતર /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      જો(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ખાલી;
      }
#નહીંતર
      res = CURLSHE_NOT_BUILT_IN;
#endif
      તોડો;

    સ્થિતિ CURL_LOCK_DATA_CONNECT:
      તોડો;

    મૂળભૂત:
      res = CURLSHE_BAD_OPTION;
      તોડો;
    }
    જો(!res)
      share->specifier &= ~(unsigned પૂર્ણાંક)(1 << type);
    તોડો;

  સ્થિતિ CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    તોડો;

  સ્થિતિ CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    તોડો;

  સ્થિતિ CURLSHOPT_USERDATA:
    ptr = va_arg(param, શૂન્ય *);
    share->clientdata = ptr;
    તોડો;

  મૂળભૂત:
    res = CURLSHE_BAD_OPTION;
    તોડો;
  }

  va_end(param);

  પાછા res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  સંરચના Curl_share *share = sh;
  bool locked;
  જો(!GOOD_SHARE_HANDLE(share))
    પાછા CURLSHE_INVALID;

  જો(share_in_use(share))
    પાછા CURLSHE_IN_USE;

  locked = share_lock_acquire(share, ખાલી);
  share_unlink(&share, ખાલી, locked);
  પાછા CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(સંરચના Curl_share *share,
                                 સંરચના Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  જો(!share)
    પાછા CURLSHE_INVALID;

  જો(share->specifier & (unsigned પૂર્ણાંક)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  પાછા CURLSHE_OK;
}

CURLSHcode Curl_share_lock(સંરચના Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  પાછા Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(સંરચના Curl_share *share,
                                   સંરચના Curl_easy *data,
                                   curl_lock_data type)
{
  જો(!share)
    પાછા CURLSHE_INVALID;

  જો(share->specifier & (unsigned પૂર્ણાંક)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  પાછા CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(સંરચના Curl_easy *data, curl_lock_data type)
{
  પાછા Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(સંરચના Curl_easy *data)
{
  સંરચના Curl_share *share = data->share;

  જો(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    જો((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      જો(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    જો(share->cookies == data->cookies)
      data->cookies = ખાલી;
#endif

#ifndef CURL_DISABLE_HSTS
    જો(share->hsts == data->hsts)
      data->hsts = ખાલી;
#endif
#ifdef USE_LIBPSL
    જો(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : ખાલી;
#endif

    share_unlink(&data->share, data, locked);
  }
  પાછા CURLE_OK;
}

CURLcode Curl_share_easy_link(સંરચના Curl_easy *data,
                              સંરચના Curl_share *share)
{
  જો(data->share) {
    DEBUGASSERT(0);
    પાછા CURLE_FAILED_INIT;
  }

  જો(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    જો((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    જો(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    જો(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    જો(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  પાછા CURLE_OK;
}
