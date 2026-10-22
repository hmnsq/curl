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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "url.h"

स्थिर शून्य share_destroy(संरचना Curl_share *share)
{
  जइ(!share)
    प्रत्यागम;

  जइ(share->specifier & (1 << CURL_LOCK_DATA_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#endif

#ifndef CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#endif

#ifdef USE_SSL
  जइ(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = रिक्त;
  }
#endif

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ifdef USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#endif
  curlx_memzero(share, परिमाण(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(शून्य)
{
  संरचना Curl_share *share = curlx_calloc(1, परिमाण(संरचना Curl_share));
  जइ(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_DATA_SHARE);
#ifdef USE_MUTEX
    Curl_mutex_init(&share->lock);
#endif
    share->ref_गणनम् = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    जइ(!share->admin) {
      share_destroy(share);
      प्रत्यागम रिक्त;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#ifdef DEBUGBUILD
    जइ(getenv("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#endif
  }
  प्रत्यागम share;
}

स्थिर uint32_t share_ref_inc(संरचना Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_गणनम्;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#अण्णहा
  n = ++share->ref_गणनम्;
  share->has_been_shared = TRUE;
#endif
  प्रत्यागम n;
}

स्थिर uint32_t share_ref_dec(संरचना Curl_share *share)
{
  uint32_t n;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_गणनम्);
  n = --share->ref_गणनम्;
  Curl_mutex_release(&share->lock);
#अण्णहा
  n = --share->ref_गणनम्;
#endif
  प्रत्यागम n;
}

स्थिर bool share_has_been_shared(संरचना Curl_share *share)
{
  bool was_shared;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#अण्णहा
  was_shared = share->has_been_shared;
#endif
  प्रत्यागम was_shared;
}

स्थिर bool share_lock_acquire(संरचना Curl_share *share,
                               संरचना Curl_easy *data)
{
  जइ(share->lockfunc && share->unlockfunc &&
     (data || share_has_been_shared(share))) {
    share->lockfunc(data, CURL_LOCK_DATA_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर शून्य share_lock_release(संरचना Curl_share *share,
                               संरचना Curl_easy *data,
                               bool locked)
{
  जइ(locked) {
    DEBUGASSERT(share->unlockfunc);
    जइ(share->unlockfunc)
      share->unlockfunc(data, CURL_LOCK_DATA_SHARE, share->clientdata);
  }
}

स्थिर bool share_in_use(संरचना Curl_share *share)
{
  bool in_use;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_गणनम् > 1);
  Curl_mutex_release(&share->lock);
#अण्णहा
  bool locked = share_lock_acquire(share, रिक्त);
  in_use = (share->ref_गणनम् > 1);
  share_lock_release(share, रिक्त, locked);
#endif
  प्रत्यागम in_use;
}

स्थिर शून्य share_unlink(संरचना Curl_share **pshare,
                         संरचना Curl_easy *data,
                         bool locked)
{
  संरचना Curl_share *share = *pshare;
  uint32_t n;

  *pshare = रिक्त;
  n = share_ref_dec(share);
  जइ(locked)
    share_lock_release(share, data, locked);
  जइ(!n)  /* last reference gone */
    share_destroy(share);
}

#undef curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  पूर्णाङ्क type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  शून्य *ptr;
  CURLSHcode res = CURLSHE_OK;
  संरचना Curl_share *share = sh;

  जइ(!GOOD_SHARE_HANDLE(share))
    प्रत्यागम CURLSHE_INVALID;

  जइ(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    प्रत्यागम CURLSHE_IN_USE;
  }

  va_start(param, option);

  विकल्प(option) {
  पक्ष CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, पूर्णाङ्क);

    विकल्प(type) {
    पक्ष CURL_LOCK_DATA_DNS:
      भङ्ग;

    पक्ष CURL_LOCK_DATA_COOKIE:
#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      जइ(!share->cookies) {
        share->cookies = Curl_cookie_init();
        जइ(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#अण्णहा /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      भङ्ग;

    पक्ष CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      जइ(!share->hsts) {
        share->hsts = Curl_hsts_init();
        जइ(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#अण्णहा /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      भङ्ग;

    पक्ष CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      जइ(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        जइ(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#अण्णहा
      res = CURLSHE_NOT_BUILT_IN;
#endif
      भङ्ग;

    पक्ष CURL_LOCK_DATA_CONNECT:
      /* It is safe to set this option several times on a share. */
      जइ(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      भङ्ग;

    पक्ष CURL_LOCK_DATA_PSL:
#ifndef USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#endif
      भङ्ग;

    स्वभाव:
      res = CURLSHE_BAD_OPTION;
    }
    जइ(!res)
      share->specifier |= (unsigned पूर्णाङ्क)(1 << type);
    भङ्ग;

  पक्ष CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, पूर्णाङ्क);
    विकल्प(type) {
    पक्ष CURL_LOCK_DATA_DNS:
      भङ्ग;

    पक्ष CURL_LOCK_DATA_COOKIE:
#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      जइ(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = रिक्त;
      }
#अण्णहा /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      भङ्ग;

    पक्ष CURL_LOCK_DATA_HSTS:
#ifndef CURL_DISABLE_HSTS
      जइ(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#अण्णहा /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#endif
      भङ्ग;

    पक्ष CURL_LOCK_DATA_SSL_SESSION:
#ifdef USE_SSL
      जइ(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = रिक्त;
      }
#अण्णहा
      res = CURLSHE_NOT_BUILT_IN;
#endif
      भङ्ग;

    पक्ष CURL_LOCK_DATA_CONNECT:
      भङ्ग;

    स्वभाव:
      res = CURLSHE_BAD_OPTION;
      भङ्ग;
    }
    जइ(!res)
      share->specifier &= ~(unsigned पूर्णाङ्क)(1 << type);
    भङ्ग;

  पक्ष CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_lock_function);
    share->lockfunc = lockfunc;
    भङ्ग;

  पक्ष CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    भङ्ग;

  पक्ष CURLSHOPT_USERDATA:
    ptr = va_arg(param, शून्य *);
    share->clientdata = ptr;
    भङ्ग;

  स्वभाव:
    res = CURLSHE_BAD_OPTION;
    भङ्ग;
  }

  va_end(param);

  प्रत्यागम res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  संरचना Curl_share *share = sh;
  bool locked;
  जइ(!GOOD_SHARE_HANDLE(share))
    प्रत्यागम CURLSHE_INVALID;

  जइ(share_in_use(share))
    प्रत्यागम CURLSHE_IN_USE;

  locked = share_lock_acquire(share, रिक्त);
  share_unlink(&share, रिक्त, locked);
  प्रत्यागम CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(संरचना Curl_share *share,
                                 संरचना Curl_easy *data,
                                 curl_lock_data type,
                                 curl_lock_access accesstype)
{
  जइ(!share)
    प्रत्यागम CURLSHE_INVALID;

  जइ(share->specifier & (unsigned पूर्णाङ्क)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(data, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  प्रत्यागम CURLSHE_OK;
}

CURLSHcode Curl_share_lock(संरचना Curl_easy *data, curl_lock_data type,
                           curl_lock_access accesstype)
{
  प्रत्यागम Curl_share_lock_share(data->share, data, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(संरचना Curl_share *share,
                                   संरचना Curl_easy *data,
                                   curl_lock_data type)
{
  जइ(!share)
    प्रत्यागम CURLSHE_INVALID;

  जइ(share->specifier & (unsigned पूर्णाङ्क)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(data, type, share->clientdata);

  प्रत्यागम CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(संरचना Curl_easy *data, curl_lock_data type)
{
  प्रत्यागम Curl_share_unlock_share(data->share, data, type);
}

CURLcode Curl_share_easy_unlink(संरचना Curl_easy *data)
{
  संरचना Curl_share *share = data->share;

  जइ(share) {
    bool locked = share_lock_acquire(share, data);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    जइ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      जइ(data->conn)
        Curl_detach_connection(data);
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    जइ(share->cookies == data->cookies)
      data->cookies = रिक्त;
#endif

#ifndef CURL_DISABLE_HSTS
    जइ(share->hsts == data->hsts)
      data->hsts = रिक्त;
#endif
#ifdef USE_LIBPSL
    जइ(&share->psl == data->psl)
      data->psl = data->multi ? &data->multi->psl : रिक्त;
#endif

    share_unlink(&data->share, data, locked);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_share_easy_link(संरचना Curl_easy *data,
                              संरचना Curl_share *share)
{
  जइ(data->share) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  जइ(share) {
    bool locked = share_lock_acquire(share, data);

    /* Forget identifiers from the previously used connection cache. */
    जइ((share->specifier & (1 << CURL_LOCK_DATA_CONNECT))) {
      data->state.last_conn_id = -1;
      data->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    data->share = share;

#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    जइ(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(data->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      data->cookies = share->cookies;
    }
#endif /* CURL_DISABLE_HTTP */
#ifndef CURL_DISABLE_HSTS
    जइ(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&data->hsts);
      data->hsts = share->hsts;
    }
#endif
#ifdef USE_LIBPSL
    जइ(share->specifier & (1 << CURL_LOCK_DATA_PSL))
      data->psl = &share->psl;
#endif

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, data, locked);
  }
  प्रत्यागम CURLE_OK;
}
