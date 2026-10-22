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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "multiif.h"
#समावेश "curl_threads.h"
#समावेश "curl_share.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "hsts.h"
#समावेश "url.h"

अटल व्योम share_destroy(काष्ठा Curl_share *share)
{
  अगर(!share)
    वापस;

  अगर(share->specifier & (1 << CURL_LOCK_तथ्य_CONNECT)) {
    Curl_cpool_destroy(&share->cpool, share->admin);
  }

  Curl_dnscache_destroy(&share->dnscache);

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
  Curl_cookie_cleanup(share->cookies);
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_HSTS
  Curl_hsts_cleanup(&share->hsts);
#पूर्ण_अगर

#अगर_घोषित USE_SSL
  अगर(share->ssl_scache) {
    Curl_ssl_scache_destroy(share->ssl_scache);
    share->ssl_scache = शून्य;
  }
#पूर्ण_अगर

  Curl_psl_destroy(&share->psl);
  Curl_close(&share->admin);

#अगर_घोषित USE_MUTEX
  Curl_mutex_destroy(&share->lock);
#पूर्ण_अगर
  curlx_memzero(share, माप(*share));
  curlx_free(share);
}

CURLSH *curl_share_init(व्योम)
{
  काष्ठा Curl_share *share = curlx_calloc(1, माप(काष्ठा Curl_share));
  अगर(share) {
    share->magic = CURL_GOOD_SHARE;
    share->specifier |= (1 << CURL_LOCK_तथ्य_SHARE);
#अगर_घोषित USE_MUTEX
    Curl_mutex_init(&share->lock);
#पूर्ण_अगर
    share->ref_गिनती = 1;
    Curl_dnscache_init(&share->dnscache, 23);
    share->admin = curl_easy_init();
    अगर(!share->admin) {
      share_destroy(share);
      वापस शून्य;
    }
    /* admin handles have mid 0 */
    share->admin->mid = 0;
    share->admin->state.internal = TRUE;
#अगर_घोषित DEBUGBUILD
    अगर(दो_पर्या("CURL_DEBUG"))
      share->admin->set.verbose = TRUE;
#पूर्ण_अगर
  }
  वापस share;
}

अटल uint32_t share_ref_inc(काष्ठा Curl_share *share)
{
  uint32_t n;
#अगर_घोषित USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  n = ++share->ref_गिनती;
  share->has_been_shared = TRUE;
  Curl_mutex_release(&share->lock);
#अन्यथा
  n = ++share->ref_गिनती;
  share->has_been_shared = TRUE;
#पूर्ण_अगर
  वापस n;
}

अटल uint32_t share_ref_dec(काष्ठा Curl_share *share)
{
  uint32_t n;
#अगर_घोषित USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  DEBUGASSERT(share->ref_गिनती);
  n = --share->ref_गिनती;
  Curl_mutex_release(&share->lock);
#अन्यथा
  n = --share->ref_गिनती;
#पूर्ण_अगर
  वापस n;
}

अटल bool share_has_been_shared(काष्ठा Curl_share *share)
{
  bool was_shared;
#अगर_घोषित USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  was_shared = share->has_been_shared;
  Curl_mutex_release(&share->lock);
#अन्यथा
  was_shared = share->has_been_shared;
#पूर्ण_अगर
  वापस was_shared;
}

अटल bool share_lock_acquire(काष्ठा Curl_share *share,
                               काष्ठा Curl_easy *तथ्य)
{
  अगर(share->lockfunc && share->unlockfunc &&
     (तथ्य || share_has_been_shared(share))) {
    share->lockfunc(तथ्य, CURL_LOCK_तथ्य_SHARE, CURL_LOCK_ACCESS_SINGLE,
                    share->clientdata);
    वापस TRUE;
  }
  वापस FALSE;
}

अटल व्योम share_lock_release(काष्ठा Curl_share *share,
                               काष्ठा Curl_easy *तथ्य,
                               bool locked)
{
  अगर(locked) {
    DEBUGASSERT(share->unlockfunc);
    अगर(share->unlockfunc)
      share->unlockfunc(तथ्य, CURL_LOCK_तथ्य_SHARE, share->clientdata);
  }
}

अटल bool share_in_use(काष्ठा Curl_share *share)
{
  bool in_use;
#अगर_घोषित USE_MUTEX
  Curl_mutex_acquire(&share->lock);
  in_use = (share->ref_गिनती > 1);
  Curl_mutex_release(&share->lock);
#अन्यथा
  bool locked = share_lock_acquire(share, शून्य);
  in_use = (share->ref_गिनती > 1);
  share_lock_release(share, शून्य, locked);
#पूर्ण_अगर
  वापस in_use;
}

अटल व्योम share_unlink(काष्ठा Curl_share **pshare,
                         काष्ठा Curl_easy *तथ्य,
                         bool locked)
{
  काष्ठा Curl_share *share = *pshare;
  uint32_t n;

  *pshare = शून्य;
  n = share_ref_dec(share);
  अगर(locked)
    share_lock_release(share, तथ्य, locked);
  अगर(!n)  /* last reference gone */
    share_destroy(share);
}

#अघोषित curl_share_setopt
CURLSHcode curl_share_setopt(CURLSH *sh, CURLSHoption option, ...)
{
  बहु_सूची param;
  पूर्णांक type;
  curl_lock_function lockfunc;
  curl_unlock_function unlockfunc;
  व्योम *ptr;
  CURLSHcode res = CURLSHE_OK;
  काष्ठा Curl_share *share = sh;

  अगर(!GOOD_SHARE_HANDLE(share))
    वापस CURLSHE_INVALID;

  अगर(share_in_use(share)) {
    /* do not allow setting options while one or more handles are already
       using this share */
    वापस CURLSHE_IN_USE;
  }

  बहु_शुरू(param, option);

  चयन(option) {
  हाल CURLSHOPT_SHARE:
    /* this is a type this share will share */
    type = बहु_तर्क(param, पूर्णांक);

    चयन(type) {
    हाल CURL_LOCK_तथ्य_DNS:
      अवरोध;

    हाल CURL_LOCK_तथ्य_COOKIE:
#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      अगर(!share->cookies) {
        share->cookies = Curl_cookie_init();
        अगर(!share->cookies)
          res = CURLSHE_NOMEM;
      }
#अन्यथा /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#पूर्ण_अगर
      अवरोध;

    हाल CURL_LOCK_तथ्य_HSTS:
#अगर_अघोषित CURL_DISABLE_HSTS
      अगर(!share->hsts) {
        share->hsts = Curl_hsts_init();
        अगर(!share->hsts)
          res = CURLSHE_NOMEM;
      }
#अन्यथा /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#पूर्ण_अगर
      अवरोध;

    हाल CURL_LOCK_तथ्य_SSL_SESSION:
#अगर_घोषित USE_SSL
      अगर(!share->ssl_scache) {
        /* There is no way (yet) for the application to configure the
         * session cache size, shared between many transfers. As for curl
         * itself, a high session count will impact startup time. Also, the
         * scache is not optimized for several hundreds of peers.
         * Keep it at a reasonable level. */
        अगर(Curl_ssl_scache_create(25, 2, &share->ssl_scache))
          res = CURLSHE_NOMEM;
      }
#अन्यथा
      res = CURLSHE_NOT_BUILT_IN;
#पूर्ण_अगर
      अवरोध;

    हाल CURL_LOCK_तथ्य_CONNECT:
      /* It is safe to set this option several times on a share. */
      अगर(!share->cpool.initialized) {
        Curl_cpool_init(&share->cpool, share, 103);
      }
      अवरोध;

    हाल CURL_LOCK_तथ्य_PSL:
#अगर_अघोषित USE_LIBPSL
      res = CURLSHE_NOT_BUILT_IN;
#पूर्ण_अगर
      अवरोध;

    शेष:
      res = CURLSHE_BAD_OPTION;
    }
    अगर(!res)
      share->specifier |= (अचिन्हित पूर्णांक)(1 << type);
    अवरोध;

  हाल CURLSHOPT_UNSHARE:
    /* this is a type this share will no longer share */
    type = बहु_तर्क(param, पूर्णांक);
    चयन(type) {
    हाल CURL_LOCK_तथ्य_DNS:
      अवरोध;

    हाल CURL_LOCK_तथ्य_COOKIE:
#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
      अगर(share->cookies) {
        Curl_cookie_cleanup(share->cookies);
        share->cookies = शून्य;
      }
#अन्यथा /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
      res = CURLSHE_NOT_BUILT_IN;
#पूर्ण_अगर
      अवरोध;

    हाल CURL_LOCK_तथ्य_HSTS:
#अगर_अघोषित CURL_DISABLE_HSTS
      अगर(share->hsts) {
        Curl_hsts_cleanup(&share->hsts);
      }
#अन्यथा /* CURL_DISABLE_HSTS */
      res = CURLSHE_NOT_BUILT_IN;
#पूर्ण_अगर
      अवरोध;

    हाल CURL_LOCK_तथ्य_SSL_SESSION:
#अगर_घोषित USE_SSL
      अगर(share->ssl_scache) {
        Curl_ssl_scache_destroy(share->ssl_scache);
        share->ssl_scache = शून्य;
      }
#अन्यथा
      res = CURLSHE_NOT_BUILT_IN;
#पूर्ण_अगर
      अवरोध;

    हाल CURL_LOCK_तथ्य_CONNECT:
      अवरोध;

    शेष:
      res = CURLSHE_BAD_OPTION;
      अवरोध;
    }
    अगर(!res)
      share->specifier &= ~(अचिन्हित पूर्णांक)(1 << type);
    अवरोध;

  हाल CURLSHOPT_LOCKFUNC:
    lockfunc = बहु_तर्क(param, curl_lock_function);
    share->lockfunc = lockfunc;
    अवरोध;

  हाल CURLSHOPT_UNLOCKFUNC:
    unlockfunc = बहु_तर्क(param, curl_unlock_function);
    share->unlockfunc = unlockfunc;
    अवरोध;

  हाल CURLSHOPT_USERDATA:
    ptr = बहु_तर्क(param, व्योम *);
    share->clientdata = ptr;
    अवरोध;

  शेष:
    res = CURLSHE_BAD_OPTION;
    अवरोध;
  }

  बहु_पूर्ण(param);

  वापस res;
}

CURLSHcode curl_share_cleanup(CURLSH *sh)
{
  काष्ठा Curl_share *share = sh;
  bool locked;
  अगर(!GOOD_SHARE_HANDLE(share))
    वापस CURLSHE_INVALID;

  अगर(share_in_use(share))
    वापस CURLSHE_IN_USE;

  locked = share_lock_acquire(share, शून्य);
  share_unlink(&share, शून्य, locked);
  वापस CURLSHE_OK;
}

CURLSHcode Curl_share_lock_share(काष्ठा Curl_share *share,
                                 काष्ठा Curl_easy *तथ्य,
                                 curl_lock_तथ्य type,
                                 curl_lock_access accesstype)
{
  अगर(!share)
    वापस CURLSHE_INVALID;

  अगर(share->specifier & (अचिन्हित पूर्णांक)(1 << type) &&
     share->lockfunc) /* only call this if set! */
    share->lockfunc(तथ्य, type, accesstype, share->clientdata);
  /* else if we do not share this, pretend successful lock */

  वापस CURLSHE_OK;
}

CURLSHcode Curl_share_lock(काष्ठा Curl_easy *तथ्य, curl_lock_तथ्य type,
                           curl_lock_access accesstype)
{
  वापस Curl_share_lock_share(तथ्य->share, तथ्य, type, accesstype);
}

CURLSHcode Curl_share_unlock_share(काष्ठा Curl_share *share,
                                   काष्ठा Curl_easy *तथ्य,
                                   curl_lock_तथ्य type)
{
  अगर(!share)
    वापस CURLSHE_INVALID;

  अगर(share->specifier & (अचिन्हित पूर्णांक)(1 << type) &&
     share->unlockfunc) /* only call this if set! */
    share->unlockfunc(तथ्य, type, share->clientdata);

  वापस CURLSHE_OK;
}

CURLSHcode Curl_share_unlock(काष्ठा Curl_easy *तथ्य, curl_lock_तथ्य type)
{
  वापस Curl_share_unlock_share(तथ्य->share, तथ्य, type);
}

CURLcode Curl_share_easy_unlink(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_share *share = तथ्य->share;

  अगर(share) {
    bool locked = share_lock_acquire(share, तथ्य);

    /* If share caches connections, detach any existing connection and
     * forget its identifier. */
    अगर((share->specifier & (1 << CURL_LOCK_तथ्य_CONNECT))) {
      अगर(तथ्य->conn)
        Curl_detach_connection(तथ्य);
      तथ्य->state.last_conn_id = -1;
      तथ्य->state.last_cpid = UINT32_MAX;
    }

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    अगर(share->cookies == तथ्य->cookies)
      तथ्य->cookies = शून्य;
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_HSTS
    अगर(share->hsts == तथ्य->hsts)
      तथ्य->hsts = शून्य;
#पूर्ण_अगर
#अगर_घोषित USE_LIBPSL
    अगर(&share->psl == तथ्य->psl)
      तथ्य->psl = तथ्य->multi ? &तथ्य->multi->psl : शून्य;
#पूर्ण_अगर

    share_unlink(&तथ्य->share, तथ्य, locked);
  }
  वापस CURLE_OK;
}

CURLcode Curl_share_easy_link(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_share *share)
{
  अगर(तथ्य->share) {
    DEBUGASSERT(0);
    वापस CURLE_FAILED_INIT;
  }

  अगर(share) {
    bool locked = share_lock_acquire(share, तथ्य);

    /* Forget identifiers from the previously used connection cache. */
    अगर((share->specifier & (1 << CURL_LOCK_तथ्य_CONNECT))) {
      तथ्य->state.last_conn_id = -1;
      तथ्य->state.last_cpid = UINT32_MAX;
    }

    share_ref_inc(share);
    तथ्य->share = share;

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)
    अगर(share->cookies) {
      /* use shared cookie list, first free own one if any */
      Curl_cookie_cleanup(तथ्य->cookies);
      /* enable cookies since we now use a share that uses cookies! */
      तथ्य->cookies = share->cookies;
    }
#पूर्ण_अगर /* CURL_DISABLE_HTTP */
#अगर_अघोषित CURL_DISABLE_HSTS
    अगर(share->hsts) {
      /* first free the private one if any */
      Curl_hsts_cleanup(&तथ्य->hsts);
      तथ्य->hsts = share->hsts;
    }
#पूर्ण_अगर
#अगर_घोषित USE_LIBPSL
    अगर(share->specifier & (1 << CURL_LOCK_तथ्य_PSL))
      तथ्य->psl = &share->psl;
#पूर्ण_अगर

    /* check for host cache not needed,
     * it will be done by curl_easy_perform */
    share_lock_release(share, तथ्य, locked);
  }
  वापस CURLE_OK;
}
