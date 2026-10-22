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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "url.h"

ಸ್ಥಿರ ಶೂನ್ಯ share_destroy(ರಚನೆ Curl_share *share)
{
  ಆದರೆ(!share)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(share->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಆದರೆ(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = ಖಾಲಿ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  Curl_mutex_destroy(&share->ಬೀಗ);
#ಆದರೆ_ಅಂತ್ಯ
  curlx_memzero(share, ಗಾತ್ರ(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(ಶೂನ್ಯ)
{
  ರಚನೆ Curl_share *share = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_share));
  ಆದರೆ(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_SHARE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
    Curl_mutex_init(&share->ಬೀಗ);
#ಆದರೆ_ಅಂತ್ಯ
    share->ref_ಎಣಿಕೆ = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    ಆದರೆ(!share->admin) {
      share_destroy(share);
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.ಆಂತರಿಕ = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಆದರೆ(getenv("CURL_DEBUG"))
      share->admin->ಗಣ.verbose = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಹಿಂತಿರುಗಿ share;
}

ಸ್ಥಿರ uint32_t share_ref_inc(ರಚನೆ Curl_share *share)
{
  uint32_t n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  Curl_mutex_acquire(&share->ಬೀಗ);
  n = ++share->ref_ಎಣಿಕೆ;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->ಬೀಗ);
#ಇಲ್ಲದಿದ್ದರೆ
  n = ++share->ref_ಎಣಿಕೆ;
  share->has_been_shared = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ uint32_t share_ref_dec(ರಚನೆ Curl_share *share)
{
  uint32_t n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  Curl_mutex_acquire(&share->ಬೀಗ);
  DEBUGASSERT(share->ref_ಎಣಿಕೆ);
  n = --share->ref_ಎಣಿಕೆ;
  Curl_mutex_release(&share->ಬೀಗ);
#ಇಲ್ಲದಿದ್ದರೆ
  n = --share->ref_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ bool share_has_been_shared(ರಚನೆ Curl_share *share)
{
  bool was_shared;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  Curl_mutex_acquire(&share->ಬೀಗ);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->ಬೀಗ);
#ಇಲ್ಲದಿದ್ದರೆ
  was_shared = share->has_been_shared;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ was_shared;
}

ಸ್ಥಿರ bool share_ಬೀಗ_acquire(ರಚನೆ Curl_share *share,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(share->lockfunc && share->unlockfunc &&
     (ದತ್ತಾಂಶ || share_has_been_shared(share))) {
    share->lockfunc(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_SHARE, CURL_ಬೀಗ_ACCESS_SINGLE,
                    share->clientdata);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ share_ಬೀಗ_release(ರಚನೆ Curl_share *share,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool locked)
{
  ಆದರೆ(locked) {
    DEBUGASSERT(share->unlockfunc);
    ಆದರೆ(share->unlockfunc)
      share->unlockfunc(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_SHARE, share->clientdata);
  }
}

ಸ್ಥಿರ bool share_ಒಳ_use(ರಚನೆ Curl_share *share)
{
  bool ಒಳ_use;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  Curl_mutex_acquire(&share->ಬೀಗ);
  ಒಳ_use = (share->ref_ಎಣಿಕೆ > 1);
  Curl_mutex_release(&share->ಬೀಗ);
#ಇಲ್ಲದಿದ್ದರೆ
  bool locked = share_ಬೀಗ_acquire(share, ಖಾಲಿ);
  ಒಳ_use = (share->ref_ಎಣಿಕೆ > 1);
  share_ಬೀಗ_release(share, ಖಾಲಿ, locked);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಒಳ_use;
}

ಸ್ಥಿರ ಶೂನ್ಯ share_unlink(ರಚನೆ Curl_share **pshare,
                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         bool locked)
{
  ರಚನೆ Curl_share *share = *pshare;
  uint32_t n;

  *pshare = ಖಾಲಿ;
  n = share_ref_dec(share);
  ಆದರೆ(locked)
    share_ಬೀಗ_release(share, ದತ್ತಾಂಶ, locked);
  ಆದರೆ(!n)  /* last reference gone */
    share_destroy(share);
}

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  va_list param;
  ಪೂರ್ಣಾಂಕ type;
  curl_ಬೀಗ_function lockfunc;
  curl_unlock_function unlockfunc;
  ಶೂನ್ಯ *ptr;
  CURLSHcode res = CURLSHE_OK;
  ರಚನೆ Curl_share *share = sh;

  ಆದರೆ(!GOOD_SHARE_HANDLE(share))
    ಹಿಂತಿರುಗಿ CURLSHE_INVALID;

  ಆದರೆ(share_ಒಳ_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    ಹಿಂತಿರುಗಿ CURLSHE_ಒಳ_USE;
  }

  va_start(param, option);

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = va_arg(param, ಪೂರ್ಣಾಂಕ);

    ಆಯ್ಕೆ(type) {
    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_DNS:
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE:
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ಆದರೆ(!share->cookies) {
        share->cookies = Curl_cookie_init();
        ಆದರೆ(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_HSTS:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
      ಆದರೆ(!share->hsts) {
        share->hsts = Curl_hsts_init();
        ಆದರೆ(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_SSL_SESSION:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
      ಆದರೆ(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        ಆದರೆ(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#ಇಲ್ಲದಿದ್ದರೆ
      res = CURLSHE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT:
      /* It is safe to set this option several times on a share. */
      ಆದರೆ(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_LIBPSL
      res = CURLSHE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      res = CURLSHE_BAD_OPTION;
    }
    ಆದರೆ(!res)
      share->specifier |= (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(1 << type);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = va_arg(param, ಪೂರ್ಣಾಂಕ);
    ಆಯ್ಕೆ(type) {
    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_DNS:
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_COOKIE:
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      ಆದರೆ(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = ಖಾಲಿ;
      }
#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_HSTS:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
      ಆದರೆ(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_SSL_SESSION:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
      ಆದರೆ(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = ಖಾಲಿ;
      }
#ಇಲ್ಲದಿದ್ದರೆ
      res = CURLSHE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT:
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      res = CURLSHE_BAD_OPTION;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(!res)
      share->specifier &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(1 << type);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLSHOPT_LOCKFUNC:
    lockfunc = va_arg(param, curl_ಬೀಗ_function);
    share->lockfunc = lockfunc;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLSHOPT_UNLOCKFUNC:
    unlockfunc = va_arg(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURLSHOPT_USERDATA:
    ptr = va_arg(param, ಶೂನ್ಯ *);
    share->clientdata = ptr;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    res = CURLSHE_BAD_OPTION;
    ನಿಲ್ಲಿಸು;
  }

  va_end(param);

  ಹಿಂತಿರುಗಿ res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  ರಚನೆ Curl_share *share = sh;
  bool locked;
  ಆದರೆ(!GOOD_SHARE_HANDLE(share))
    ಹಿಂತಿರುಗಿ CURLSHE_INVALID;

  ಆದರೆ(share_ಒಳ_use(share))
    ಹಿಂತಿರುಗಿ CURLSHE_ಒಳ_USE;

  locked = share_ಬೀಗ_acquire(share, ಖಾಲಿ);
  share_unlink(&share, ಖಾಲಿ, locked);
  ಹಿಂತಿರುಗಿ CURLSHE_OK;
}

CURLSHcode Curl_share_ಬೀಗ_share(ರಚನೆ Curl_share *share,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 curl_ಬೀಗ_ದತ್ತಾಂಶ type,
                                 curl_ಬೀಗ_access accesstype)
{
  ಆದರೆ(!share)
    ಹಿಂತಿರುಗಿ CURLSHE_INVALID;

  ಆದರೆ(share->specifier & (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(ದತ್ತಾಂಶ, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  ಹಿಂತಿರುಗಿ CURLSHE_OK;
}

CURLSHcode Curl_share_ಬೀಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_ಬೀಗ_ದತ್ತಾಂಶ type,
                           curl_ಬೀಗ_access accesstype)
{
  ಹಿಂತಿರುಗಿ Curl_share_ಬೀಗ_share(ದತ್ತಾಂಶ->share, ದತ್ತಾಂಶ, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(ರಚನೆ Curl_share *share,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   curl_ಬೀಗ_ದತ್ತಾಂಶ type)
{
  ಆದರೆ(!share)
    ಹಿಂತಿರುಗಿ CURLSHE_INVALID;

  ಆದರೆ(share->specifier & (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(ದತ್ತಾಂಶ, type, share->clientdata);

  ಹಿಂತಿರುಗಿ CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_ಬೀಗ_ದತ್ತಾಂಶ type)
{
  ಹಿಂತಿರುಗಿ Curl_share_unlock_share(ದತ್ತಾಂಶ->share, ದತ್ತಾಂಶ, type);
}

CURLcode Curl_share_easy_unlink(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_share *share = ದತ್ತಾಂಶ->share;

  ಆದರೆ(share) {
    bool locked = share_ಬೀಗ_acquire(share, ದತ್ತಾಂಶ);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    ಆದರೆ((share->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT))) {
      ಆದರೆ(ದತ್ತಾಂಶ->conn)
        Curl_detach_connection(ದತ್ತಾಂಶ);
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
    }

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ಆದರೆ(share->cookies == ದತ್ತಾಂಶ->cookies)
      ದತ್ತಾಂಶ->cookies = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
    ಆದರೆ(share->hsts == ದತ್ತಾಂಶ->hsts)
      ದತ್ತಾಂಶ->hsts = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
    ಆದರೆ(&share->psl == ದತ್ತಾಂಶ->psl)
      ದತ್ತಾಂಶ->psl = ದತ್ತಾಂಶ->multi ? &ದತ್ತಾಂಶ->multi->psl : ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    share_unlink(&ದತ್ತಾಂಶ->share, ದತ್ತಾಂಶ, locked);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_share_easy_link(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_share *share)
{
  ಆದರೆ(ದತ್ತಾಂಶ->share) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಆದರೆ(share) {
    bool locked = share_ಬೀಗ_acquire(share, ದತ್ತಾಂಶ);

    /* Forget identifiers from the previously used connection cache. */
    ಆದರೆ((share->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_CONNECT))) {
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
      ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    ದತ್ತಾಂಶ->share = share;

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    ಆದರೆ(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(ದತ್ತಾಂಶ->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      ದತ್ತಾಂಶ->cookies = share->cookies;
    }
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
    ಆದರೆ(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&ದತ್ತಾಂಶ->hsts);
      ದತ್ತಾಂಶ->hsts = share->hsts;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
    ಆದರೆ(share->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL))
      ದತ್ತಾಂಶ->psl = &share->psl;
#ಆದರೆ_ಅಂತ್ಯ

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_ಬೀಗ_release(share, ದತ್ತಾಂಶ, locked);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
