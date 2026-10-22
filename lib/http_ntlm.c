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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "strcase.h"
#ضمن "http_ntlm.h"
#ضمن "curl_ntlm_core.h"
#ضمن "curlx/base64.h"
#ضمن "vauth/vauth.h"
#ضمن "url.h"
#ضمن "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#اگر_تعریف_شده USE_WINDOWS_SSPI
#ضمن "curl_sspi.h"
#پایان_اگر

CURLcode Curl_درون_داد_ntlm(بنية Curl_easy *داده,
                         bool proxy,         /* if proxy or not */
                         ثابت حرف *سرآیند) /* rest of the www-authenticate:
سرآیند*/
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  إذا(checkprefix("NTLM", سرآیند)) {
    بنية ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    إذا(!ntlm)
      أرجع CURLE_بیرون_از_حافظه;

    سرآیند += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&سرآیند);
    إذا(*سرآیند) {
      بیعلامت حرف *hdr;
      size_t hdrlen;

      نتیجه = curlx_base64_decode(سرآیند, &hdr, &hdrlen);
      إذا(!نتیجه) {
        بنية bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_مجموعه(&hdrbuf, hdr, hdrlen, curl_free);
        نتیجه = Curl_auth_decode_ntlm_type2_پیام(داده, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      إذا(نتیجه)
        أرجع نتیجه;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    وإلا {
      إذا(*state == NTLMSTATE_آخرین) {
        infof(داده, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      وإلا إذا(*state == NTLMSTATE_TYPE3) {
        infof(داده, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        أرجع CURLE_REMOTE_ACCESS_DENIED;
      }
      وإلا إذا(*state >= NTLMSTATE_TYPE1) {
        infof(داده, "NTLM handshake failure (internal error)");
        أرجع CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  أرجع نتیجه;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(بنية Curl_easy *داده, bool proxy)
{
  حرف *base64 = عدم;
  size_t len = 0;
  CURLcode نتیجه = CURLE_OK;
  بنية bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  حرف **allocuserpwd;

  /* point to credentials and host */
  بنية Curl_creds *creds = عدم;
  ثابت حرف *hostname = عدم;

  /* point to the correct struct with this */
  بنية ntlmdata *ntlm;
  curlntlm *state;
  بنية auth *authp;
  بنية connectdata *conn = داده->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(داده);

  إذا(proxy) {
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    allocuserpwd = &داده->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &داده->state.authproxy;
#وإلا
    أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
  }
  وإلا {
    allocuserpwd = &داده->req.hd_auth;
    creds = داده->state.creds;
    hostname = داده->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &داده->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  إذا(!ntlm)
    أرجع CURLE_بیرون_از_حافظه;
  authp->انجامشده = FALSE;

#اگر_تعریف_شده USE_WINDOWS_SSPI
  إذا(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    نتیجه = Curl_sspi_global_init();
    إذا(!Curl_pSecFn)
      أرجع نتیجه;
  }
  ntlm->sslContext = conn->sslContext;
#پایان_اگر

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  إذا(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_آخرین;

  بدل(*state) {
  حالة NTLMSTATE_TYPE1:
  افتراضي:  /* for the weird cases we (re)start here */
    إذا(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      إذا(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, داده->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        أرجع CURLE_FAILED_INIT;
      }
      إذا(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        أرجع CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, داده->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    نتیجه = Curl_auth_create_ntlm_type1_پیام(داده, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    إذا(!نتیجه) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      نتیجه = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      إذا(!نتیجه) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        إذا(!*allocuserpwd)
          نتیجه = CURLE_بیرون_از_حافظه;
      }
    }
    اكسر;

  حالة NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    نتیجه = Curl_auth_create_ntlm_type3_پیام(داده, creds, ntlm, &ntlmmsg);
    إذا(!نتیجه && Curl_bufref_len(&ntlmmsg)) {
      نتیجه = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      إذا(!نتیجه) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        إذا(!*allocuserpwd)
          نتیجه = CURLE_بیرون_از_حافظه;
        وإلا {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->انجامشده = TRUE;
        }
      }
    }
    اكسر;

  حالة NTLMSTATE_آخرین:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    إذا(proxy)
      داده->info.proxyauthpicked = CURLAUTH_NTLM;
    وإلا
      داده->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->انجامشده = TRUE;
    اكسر;
  }
  Curl_bufref_free(&ntlmmsg);

  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_HTTP && USE_NTLM */
