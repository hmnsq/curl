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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_ntlm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ntlm_core.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sspi.h"
#ਜੇ_ਅੰਤ

CURLcode Curl_ਨਿਵੇਸ਼_ntlm(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         bool proxy,         /* if proxy or not */
                         ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ) /* rest of the www-authenticate:
ਸਿਰਲੇਖ*/
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  ਜੇ(checkprefix("NTLM", ਸਿਰਲੇਖ)) {
    ਬਣਤਰ ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    ਜੇ(!ntlm)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਸਿਰਲੇਖ += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&ਸਿਰਲੇਖ);
    ਜੇ(*ਸਿਰਲੇਖ) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *hdr;
      size_t hdrlen;

      ਨਤੀਜਾ = curlx_base64_decode(ਸਿਰਲੇਖ, &hdr, &hdrlen);
      ਜੇ(!ਨਤੀਜਾ) {
        ਬਣਤਰ bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_ਸੈੱਟ(&hdrbuf, hdr, hdrlen, curl_free);
        ਨਤੀਜਾ = Curl_auth_decode_ntlm_type2_ਸੁਨੇਹਾ(ਅੰਕੜੇ, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(*state == NTLMSTATE_ਆਖਰੀ) {
        infof(ਅੰਕੜੇ, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      ਨਹੀਂਤਾਂ ਜੇ(*state == NTLMSTATE_TYPE3) {
        infof(ਅੰਕੜੇ, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        ਵਾਪਸ CURLE_REMOTE_ACCESS_DENIED;
      }
      ਨਹੀਂਤਾਂ ਜੇ(*state >= NTLMSTATE_TYPE1) {
        infof(ਅੰਕੜੇ, "NTLM handshake failure (internal error)");
        ਵਾਪਸ CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool proxy)
{
  ਅੱਖਰ *base64 = ਨਿਰਾ;
  size_t len = 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  ਅੱਖਰ **allocuserpwd;

  /* point to credentials and host */
  ਬਣਤਰ Curl_creds *creds = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *hostname = ਨਿਰਾ;

  /* point to the correct struct with this */
  ਬਣਤਰ ntlmdata *ntlm;
  curlntlm *state;
  ਬਣਤਰ auth *authp;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(ਅੰਕੜੇ);

  ਜੇ(proxy) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    allocuserpwd = &ਅੰਕੜੇ->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &ਅੰਕੜੇ->state.authproxy;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    allocuserpwd = &ਅੰਕੜੇ->req.hd_auth;
    creds = ਅੰਕੜੇ->state.creds;
    hostname = ਅੰਕੜੇ->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &ਅੰਕੜੇ->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  ਜੇ(!ntlm)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  authp->ਪੂਰਾ = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  ਜੇ(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    ਨਤੀਜਾ = Curl_sspi_global_init();
    ਜੇ(!Curl_pSecFn)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ntlm->sslContext = conn->sslContext;
#ਜੇ_ਅੰਤ

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  ਜੇ(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_ਆਖਰੀ;

  ਚੋਣ(*state) {
  ਹਾਲਤ NTLMSTATE_TYPE1:
  ਮੂਲ:  /* for the weird cases we (re)start here */
    ਜੇ(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      ਜੇ(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, ਅੰਕੜੇ->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        ਵਾਪਸ CURLE_FAILED_INIT;
      }
      ਜੇ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        ਵਾਪਸ CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, ਅੰਕੜੇ->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    ਨਤੀਜਾ = Curl_auth_create_ntlm_type1_ਸੁਨੇਹਾ(ਅੰਕੜੇ, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    ਜੇ(!ਨਤੀਜਾ) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      ਨਤੀਜਾ = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ਜੇ(!ਨਤੀਜਾ) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ਜੇ(!*allocuserpwd)
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    ਰੋਕੋ;

  ਹਾਲਤ NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    ਨਤੀਜਾ = Curl_auth_create_ntlm_type3_ਸੁਨੇਹਾ(ਅੰਕੜੇ, creds, ntlm, &ntlmmsg);
    ਜੇ(!ਨਤੀਜਾ && Curl_bufref_len(&ntlmmsg)) {
      ਨਤੀਜਾ = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ਜੇ(!ਨਤੀਜਾ) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ਜੇ(!*allocuserpwd)
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਨਹੀਂਤਾਂ {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->ਪੂਰਾ = TRUE;
        }
      }
    }
    ਰੋਕੋ;

  ਹਾਲਤ NTLMSTATE_ਆਖਰੀ:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    ਜੇ(proxy)
      ਅੰਕੜੇ->info.proxyauthpicked = CURLAUTH_NTLM;
    ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->ਪੂਰਾ = TRUE;
    ਰੋਕੋ;
  }
  Curl_bufref_free(&ntlmmsg);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_NTLM */
