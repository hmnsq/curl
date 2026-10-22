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
  यदि(!share)
    प्रत्यागम;

  यदि(share->specifier & (1 << CURL_तालकम्_दत्त_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#यदि_अन्तः

#यदि_परिभाषितम् USE_SSL
  यदि(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = रिक्त;
  }
#यदि_अन्तः

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#यदि_परिभाषितम् USE_MUTEX
  Curl_mutex_destroy(&share->तालकम्);
#यदि_अन्तः
  curlx_memzero(share, परिमाण(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(शून्य)
{
  संरचना Curl_share *share = curlx_calloc(1, परिमाण(संरचना Curl_share));
  यदि(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_तालकम्_दत्त_SHARE);
#यदि_परिभाषितम् USE_MUTEX
    Curl_mutex_init(&share->तालकम्);
#यदि_अन्तः
    share->ref_गणनम् = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    यदि(!share->admin) {
      share_destroy(share);
      प्रत्यागम रिक्त;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.आन्तरिकम् = TRUE;
#यदि_परिभाषितम् DEBUGBUILD
    यदि(getenv("CURL_DEBUG"))
      share->admin->समुच्चयः.verbose = TRUE;
#यदि_अन्तः
  }
  प्रत्यागम share;
}

स्थिर uint32_t share_ref_inc(संरचना Curl_share *share)
{
  uint32_t n;
#यदि_परिभाषितम् USE_MUTEX
  Curl_mutex_acquire(&share->तालकम्);
  n = ++share->ref_गणनम्;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->तालकम्);
#अन्यथा
  n = ++share->ref_गणनम्;
  share->has_been_shared = TRUE;
#यदि_अन्तः
  प्रत्यागम n;
}

स्थिर uint32_t share_ref_dec(संरचना Curl_share *share)
{
  uint32_t n;
#यदि_परिभाषितम् USE_MUTEX
  Curl_mutex_acquire(&share->तालकम्);
  DEBUGASSERT(share->ref_गणनम्);
  n = --share->ref_गणनम्;
  Curl_mutex_release(&share->तालकम्);
#अन्यथा
  n = --share->ref_गणनम्;
#यदि_अन्तः
  प्रत्यागम n;
}

स्थिर bool share_has_been_shared(संरचना Curl_share *share)
{
  bool was_shared;
#यदि_परिभाषितम् USE_MUTEX
  Curl_mutex_acquire(&share->तालकम्);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->तालकम्);
#अन्यथा
  was_shared = share->has_been_shared;
#यदि_अन्तः
  प्रत्यागम was_shared;
}

स्थिर bool share_तालकम्_acquire(संरचना Curl_share *share,
                               संरचना Curl_easy *दत्त)
{
  यदि(share->lockfunc && share->unlockfunc &&
     (दत्त || share_has_been_shared(share))) {
    share->lockfunc(दत्त, CURL_तालकम्_दत्त_SHARE, CURL_तालकम्_ACCESS_SINGLE,
                    share->clientdata);
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

स्थिर शून्य share_तालकम्_release(संरचना Curl_share *share,
                               संरचना Curl_easy *दत्त,
                               bool locked)
{
  यदि(locked) {
    DEBUGASSERT(share->unlockfunc);
    यदि(share->unlockfunc)
      share->unlockfunc(दत्त, CURL_तालकम्_दत्त_SHARE, share->clientdata);
  }
}

स्थिर bool share_अन्तर्_use(संरचना Curl_share *share)
{
  bool अन्तर्_use;
#यदि_परिभाषितम् USE_MUTEX
  Curl_mutex_acquire(&share->तालकम्);
  अन्तर्_use = (share->ref_गणनम् > 1);
  Curl_mutex_release(&share->तालकम्);
#अन्यथा
  bool locked = share_तालकम्_acquire(share, रिक्त);
  अन्तर्_use = (share->ref_गणनम् > 1);
  share_तालकम्_release(share, रिक्त, locked);
#यदि_अन्तः
  प्रत्यागम अन्तर्_use;
}

स्थिर शून्य share_unlink(संरचना Curl_share **pshare,
                         संरचना Curl_easy *दत्त,
                         bool locked)
{
  संरचना Curl_share *share = *pshare;
  uint32_t n;

  *pshare = रिक्त;
  n = share_ref_dec(share);
  यदि(locked)
    share_तालकम्_release(share, दत्त, locked);
  यदि(!n)  /* last reference gone */
    share_destroy(share);
}

#अपरिभाषय curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  पूर्णाङ्क type;
  curl_तालकम्_function lockfunc;
  curl_unlock_function unlockfunc;
  शून्य *ptr;
  CURLSHcode res = CURLSHE_OK;
  संरचना Curl_share *share = sh;

  यदि(!GOOD_SHARE_HANDLE(share))
    प्रत्यागम CURLSHE_INVALID;

  यदि(share_अन्तर्_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    प्रत्यागम CURLSHE_अन्तर्_USE;
  }

  va_start(param, option);

  विकल्प(option) {
  पक्ष CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, पूर्णाङ्क);

    विकल्प(type) {
    पक्ष CURL_तालकम्_दत्त_DNS:
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_COOKIE:
#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      यदि(!share->cookies) {
        share->cookies = Curl_cookie_init();
        यदि(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#अन्यथा /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_HSTS:
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
      यदि(!share->hsts) {
        share->hsts = Curl_hsts_init();
        यदि(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#अन्यथा /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_SSL_SESSION:
#यदि_परिभाषितम् USE_SSL
      यदि(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        यदि(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#अन्यथा
      res = CURLSHE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_CONNECT:
      /* It is safe to set this option several times on a share. */
      यदि(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_PSL:
#यदि_अपरिभाषितम् USE_LIBPSL
      res = CURLSHE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
      भङ्ग;

    स्वभाव:
      res = CURLSHE_BAD_OPTION;
    }
    यदि(!res)
      share->specifier |= (अचिह्नित पूर्णाङ्क)(1 << type);
    भङ्ग;

  पक्ष CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, पूर्णाङ्क);
    विकल्प(type) {
    पक्ष CURL_तालकम्_दत्त_DNS:
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_COOKIE:
#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      यदि(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = रिक्त;
      }
#अन्यथा /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_HSTS:
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
      यदि(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#अन्यथा /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_SSL_SESSION:
#यदि_परिभाषितम् USE_SSL
      यदि(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = रिक्त;
      }
#अन्यथा
      res = CURLSHE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
      भङ्ग;

    पक्ष CURL_तालकम्_दत्त_CONNECT:
      भङ्ग;

    स्वभाव:
      res = CURLSHE_BAD_OPTION;
      भङ्ग;
    }
    यदि(!res)
      share->specifier &= ~(अचिह्नित पूर्णाङ्क)(1 << type);
    भङ्ग;

  पक्ष CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_तालकम्_function);
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
  यदि(!GOOD_SHARE_HANDLE(share))
    प्रत्यागम CURLSHE_INVALID;

  यदि(share_अन्तर्_use(share))
    प्रत्यागम CURLSHE_अन्तर्_USE;

  locked = share_तालकम्_acquire(share, रिक्त);
  share_unlink(&share, रिक्त, locked);
  प्रत्यागम CURLSHE_OK;
}

CURLSHcode Curl_share_तालकम्_share(संरचना Curl_share *share,
                                 संरचना Curl_easy *दत्त,
                                 curl_तालकम्_दत्त type,
                                 curl_तालकम्_access accesstype)
{
  यदि(!share)
    प्रत्यागम CURLSHE_INVALID;

  यदि(share->specifier & (अचिह्नित पूर्णाङ्क)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(दत्त, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  प्रत्यागम CURLSHE_OK;
}

CURLSHcode Curl_share_तालकम्(संरचना Curl_easy *दत्त, curl_तालकम्_दत्त type,
                           curl_तालकम्_access accesstype)
{
  प्रत्यागम Curl_share_तालकम्_share(दत्त->share, दत्त, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(संरचना Curl_share *share,
                                   संरचना Curl_easy *दत्त,
                                   curl_तालकम्_दत्त type)
{
  यदि(!share)
    प्रत्यागम CURLSHE_INVALID;

  यदि(share->specifier & (अचिह्नित पूर्णाङ्क)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(दत्त, type, share->clientdata);

  प्रत्यागम CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(संरचना Curl_easy *दत्त, curl_तालकम्_दत्त type)
{
  प्रत्यागम Curl_share_unlock_share(दत्त->share, दत्त, type);
}

CURLcode Curl_share_easy_unlink(संरचना Curl_easy *दत्त)
{
  संरचना Curl_share *share = दत्त->share;

  यदि(share) {
    bool locked = share_तालकम्_acquire(share, दत्त);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    यदि((share->specifier & (1 << CURL_तालकम्_दत्त_CONNECT))) {
      यदि(दत्त->conn)
        Curl_detach_connection(दत्त);
      दत्त->state.चरमः_conn_id = -1;
      दत्त->state.चरमः_cpid = UINT32_MAX;
    }

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    यदि(share->cookies == दत्त->cookies)
      दत्त->cookies = रिक्त;
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
    यदि(share->hsts == दत्त->hsts)
      दत्त->hsts = रिक्त;
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBPSL
    यदि(&share->psl == दत्त->psl)
      दत्त->psl = दत्त->multi ? &दत्त->multi->psl : रिक्त;
#यदि_अन्तः

    share_unlink(&दत्त->share, दत्त, locked);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_share_easy_link(संरचना Curl_easy *दत्त,
                              संरचना Curl_share *share)
{
  यदि(दत्त->share) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(share) {
    bool locked = share_तालकम्_acquire(share, दत्त);

    /* Forget identifiers from the previously used connection cache. */
    यदि((share->specifier & (1 << CURL_तालकम्_दत्त_CONNECT))) {
      दत्त->state.चरमः_conn_id = -1;
      दत्त->state.चरमः_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    दत्त->share = share;

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    यदि(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(दत्त->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      दत्त->cookies = share->cookies;
    }
#यदि_अन्तः /* CURL_DISABLE_HTTP */
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
    यदि(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&दत्त->hsts);
      दत्त->hsts = share->hsts;
    }
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBPSL
    यदि(share->specifier & (1 << CURL_तालकम्_दत्त_PSL))
      दत्त->psl = &share->psl;
#यदि_अन्तः

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_तालकम्_release(share, दत्त, locked);
  }
  प्रत्यागम CURLE_OK;
}
