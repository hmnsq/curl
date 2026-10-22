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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

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
#ifdef USE_WINDOWS_SSPI
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#endif

CURLcode Curl_ಒಳಹರಿವು_ntlm(ಸಂರಚನಾ Curl_easy *data,
                         bool proxy,         /* if proxy or not */
                         ಧ್ರುವ ವರ್ಣ *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  ಯದಿ(checkprefix("NTLM", header)) {
    ಸಂರಚನಾ ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    ಯದಿ(!ntlm)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    ಯದಿ(*header) {
      unsigned ವರ್ಣ *hdr;
      size_t hdrlen;

      result = curlx_base64_decode(header, &hdr, &hdrlen);
      ಯದಿ(!result) {
        ಸಂರಚನಾ bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        result = Curl_auth_decode_ntlm_type2_message(data, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(*state == NTLMSTATE_LAST) {
        infof(data, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      ಅನ್ಯಥಾ ಯದಿ(*state == NTLMSTATE_TYPE3) {
        infof(data, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_ACCESS_DENIED;
      }
      ಅನ್ಯಥಾ ಯದಿ(*state >= NTLMSTATE_TYPE1) {
        infof(data, "NTLM handshake failure (internal error)");
        ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(ಸಂರಚನಾ Curl_easy *data, bool proxy)
{
  ವರ್ಣ *base64 = ರಿಕ್ತ;
  size_t len = 0;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  ವರ್ಣ **allocuserpwd;

  /* point to credentials and host */
  ಸಂರಚನಾ Curl_creds *creds = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *hostname = ರಿಕ್ತ;

  /* point to the correct struct with this */
  ಸಂರಚನಾ ntlmdata *ntlm;
  curlntlm *state;
  ಸಂರಚನಾ auth *authp;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(data);

  ಯದಿ(proxy) {
#ifndef CURL_DISABLE_PROXY
    allocuserpwd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &data->state.authproxy;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }
  ಅನ್ಯಥಾ {
    allocuserpwd = &data->req.hd_auth;
    creds = data->state.creds;
    hostname = data->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &data->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  ಯದಿ(!ntlm)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#ifdef USE_WINDOWS_SSPI
  ಯದಿ(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    result = Curl_sspi_global_init();
    ಯದಿ(!Curl_pSecFn)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ntlm->sslContext = conn->sslContext;
#endif

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  ಯದಿ(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  ವಿಕಲ್ಪ(*state) {
  ಪಕ್ಷ NTLMSTATE_TYPE1:
  ಸ್ವಭಾವ:  /* for the weird cases we (re)start here */
    ಯದಿ(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      ಯದಿ(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
      }
      ಯದಿ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, data->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    result = Curl_auth_create_ntlm_type1_message(data, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    ಯದಿ(!result) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ಯದಿ(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ಯದಿ(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    result = Curl_auth_create_ntlm_type3_message(data, creds, ntlm, &ntlmmsg);
    ಯದಿ(!result && Curl_bufref_len(&ntlmmsg)) {
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ಯದಿ(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ಯದಿ(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
        ಅನ್ಯಥಾ {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    ಯದಿ(proxy)
      data->info.proxyauthpicked = CURLAUTH_NTLM;
    ಅನ್ಯಥಾ
      data->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    ಭಙ್ಗ;
  }
  Curl_bufref_free(&ntlmmsg);

  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NTLM */
