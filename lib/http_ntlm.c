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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "strcase.h"
#شامل "http_ntlm.h"
#شامل "curl_ntlm_core.h"
#شامل "curlx/base64.h"
#شامل "vauth/vauth.h"
#شامل "url.h"
#شامل "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
#شامل "curl_sspi.h"
#اگر_ختم

CURLcode Curl_داخلہ_ntlm(ساخت Curl_easy *ڈیٹا,
                         bool proxy,         /* if proxy or not */
                         ثابت حرف *سرخی) /* rest of the www-authenticate:
سرخی*/
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  اگر(checkprefix("NTLM", سرخی)) {
    ساخت ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    اگر(!ntlm)
      واپس CURLE_برآمد_کا_حافظہ;

    سرخی += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&سرخی);
    اگر(*سرخی) {
      غیرعلامتی حرف *hdr;
      size_t hdrlen;

      نتیجہ = curlx_base64_decode(سرخی, &hdr, &hdrlen);
      اگر(!نتیجہ) {
        ساخت bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_سیٹ(&hdrbuf, hdr, hdrlen, curl_free);
        نتیجہ = Curl_auth_decode_ntlm_type2_پیغام(ڈیٹا, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      اگر(نتیجہ)
        واپس نتیجہ;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    ورنہ {
      اگر(*state == NTLMSTATE_آخری) {
        infof(ڈیٹا, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      ورنہ اگر(*state == NTLMSTATE_TYPE3) {
        infof(ڈیٹا, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        واپس CURLE_REMOTE_ACCESS_DENIED;
      }
      ورنہ اگر(*state >= NTLMSTATE_TYPE1) {
        infof(ڈیٹا, "NTLM handshake failure (internal error)");
        واپس CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  واپس نتیجہ;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(ساخت Curl_easy *ڈیٹا, bool proxy)
{
  حرف *base64 = تہی;
  size_t len = 0;
  CURLcode نتیجہ = CURLE_OK;
  ساخت bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  حرف **allocuserpwd;

  /* point to credentials and host */
  ساخت Curl_creds *creds = تہی;
  ثابت حرف *hostname = تہی;

  /* point to the correct struct with this */
  ساخت ntlmdata *ntlm;
  curlntlm *state;
  ساخت auth *authp;
  ساخت connectdata *conn = ڈیٹا->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(ڈیٹا);

  اگر(proxy) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    allocuserpwd = &ڈیٹا->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &ڈیٹا->state.authproxy;
#ورنہ
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }
  ورنہ {
    allocuserpwd = &ڈیٹا->req.hd_auth;
    creds = ڈیٹا->state.creds;
    hostname = ڈیٹا->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &ڈیٹا->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  اگر(!ntlm)
    واپس CURLE_برآمد_کا_حافظہ;
  authp->مکمل = FALSE;

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  اگر(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    نتیجہ = Curl_sspi_global_init();
    اگر(!Curl_pSecFn)
      واپس نتیجہ;
  }
  ntlm->sslContext = conn->sslContext;
#اگر_ختم

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  اگر(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_آخری;

  انتخاب(*state) {
  حالت NTLMSTATE_TYPE1:
  طےشدہ:  /* for the weird cases we (re)start here */
    اگر(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      اگر(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, ڈیٹا->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        واپس CURLE_FAILED_INIT;
      }
      اگر(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        واپس CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, ڈیٹا->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    نتیجہ = Curl_auth_create_ntlm_type1_پیغام(ڈیٹا, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    اگر(!نتیجہ) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      نتیجہ = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      اگر(!نتیجہ) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        اگر(!*allocuserpwd)
          نتیجہ = CURLE_برآمد_کا_حافظہ;
      }
    }
    توڑو;

  حالت NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    نتیجہ = Curl_auth_create_ntlm_type3_پیغام(ڈیٹا, creds, ntlm, &ntlmmsg);
    اگر(!نتیجہ && Curl_bufref_len(&ntlmmsg)) {
      نتیجہ = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      اگر(!نتیجہ) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        اگر(!*allocuserpwd)
          نتیجہ = CURLE_برآمد_کا_حافظہ;
        ورنہ {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->مکمل = TRUE;
        }
      }
    }
    توڑو;

  حالت NTLMSTATE_آخری:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    اگر(proxy)
      ڈیٹا->info.proxyauthpicked = CURLAUTH_NTLM;
    ورنہ
      ڈیٹا->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->مکمل = TRUE;
    توڑو;
  }
  Curl_bufref_free(&ntlmmsg);

  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && USE_NTLM */
