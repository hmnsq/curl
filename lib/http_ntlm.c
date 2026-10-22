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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "http_ntlm.h"
#ಅನ್ತರ್ಭಾವ "curl_ntlm_core.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_ಒಳಹರಿವು_ntlm(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         bool proxy,         /* if proxy or not */
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ) /* rest of the www-authenticate:
ಶೀರ್ಷಿಕೆ*/
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  ಆದರೆ(checkprefix("NTLM", ಶೀರ್ಷಿಕೆ)) {
    ರಚನೆ ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    ಆದರೆ(!ntlm)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಶೀರ್ಷಿಕೆ += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&ಶೀರ್ಷಿಕೆ);
    ಆದರೆ(*ಶೀರ್ಷಿಕೆ) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *hdr;
      size_t hdrlen;

      ಫಲಿತಾಂಶ = curlx_base64_decode(ಶೀರ್ಷಿಕೆ, &hdr, &hdrlen);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        ರಚನೆ bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_ಗಣ(&hdrbuf, hdr, hdrlen, curl_free);
        ಫಲಿತಾಂಶ = Curl_auth_decode_ntlm_type2_ಸಂದೇಶ(ದತ್ತಾಂಶ, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(*state == NTLMSTATE_ಕೊನೆಯ) {
        infof(ದತ್ತಾಂಶ, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*state == NTLMSTATE_TYPE3) {
        infof(ದತ್ತಾಂಶ, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        ಹಿಂತಿರುಗಿ CURLE_REMOTE_ACCESS_DENIED;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*state >= NTLMSTATE_TYPE1) {
        infof(ದತ್ತಾಂಶ, "NTLM handshake failure (internal error)");
        ಹಿಂತಿರುಗಿ CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool proxy)
{
  ಅಕ್ಷರ *base64 = ಖಾಲಿ;
  size_t len = 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  ಅಕ್ಷರ **allocuserpwd;

  /* point to credentials and host */
  ರಚನೆ Curl_creds *creds = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname = ಖಾಲಿ;

  /* point to the correct struct with this */
  ರಚನೆ ntlmdata *ntlm;
  curlntlm *state;
  ರಚನೆ auth *authp;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(ದತ್ತಾಂಶ);

  ಆದರೆ(proxy) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    allocuserpwd = &ದತ್ತಾಂಶ->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &ದತ್ತಾಂಶ->state.authproxy;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    allocuserpwd = &ದತ್ತಾಂಶ->req.hd_auth;
    creds = ದತ್ತಾಂಶ->state.creds;
    hostname = ದತ್ತಾಂಶ->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &ದತ್ತಾಂಶ->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  ಆದರೆ(!ntlm)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  authp->ಮುಗಿದ = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  ಆದರೆ(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    ಫಲಿತಾಂಶ = Curl_sspi_global_init();
    ಆದರೆ(!Curl_pSecFn)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ntlm->sslContext = conn->sslContext;
#ಆದರೆ_ಅಂತ್ಯ

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  ಆದರೆ(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_ಕೊನೆಯ;

  ಆಯ್ಕೆ(*state) {
  ಸ್ಥಿತಿ NTLMSTATE_TYPE1:
  ಪೂರ್ವನಿಯೋಜಿತ:  /* for the weird cases we (re)start here */
    ಆದರೆ(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      ಆದರೆ(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, ದತ್ತಾಂಶ->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
      }
      ಆದರೆ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, ದತ್ತಾಂಶ->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    ಫಲಿತಾಂಶ = Curl_auth_create_ntlm_type1_ಸಂದೇಶ(ದತ್ತಾಂಶ, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      ಫಲಿತಾಂಶ = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ಆದರೆ(!*allocuserpwd)
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    ಫಲಿತಾಂಶ = Curl_auth_create_ntlm_type3_ಸಂದೇಶ(ದತ್ತಾಂಶ, creds, ntlm, &ntlmmsg);
    ಆದರೆ(!ಫಲಿತಾಂಶ && Curl_bufref_len(&ntlmmsg)) {
      ಫಲಿತಾಂಶ = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ಆದರೆ(!*allocuserpwd)
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಇಲ್ಲದಿದ್ದರೆ {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->ಮುಗಿದ = TRUE;
        }
      }
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ NTLMSTATE_ಕೊನೆಯ:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    ಆದರೆ(proxy)
      ದತ್ತಾಂಶ->info.proxyauthpicked = CURLAUTH_NTLM;
    ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->ಮುಗಿದ = TRUE;
    ನಿಲ್ಲಿಸು;
  }
  Curl_bufref_free(&ntlmmsg);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_NTLM */
