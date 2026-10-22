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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "http_ntlm.h"
#அந்தர்பாவ "curl_ntlm_core.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ifdef USE_WINDOWS_SSPI
#அந்தர்பாவ "curl_sspi.h"
#endif

CURLcode Curl_உள்ளீடு_ntlm(கட்டமைப்பு Curl_easy *data,
                         bool proxy,         /* if proxy or not */
                         மாறிலி எழுத்து *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு connectdata *conn = data->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  எனில்(checkprefix("NTLM", header)) {
    கட்டமைப்பு ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    எனில்(!ntlm)
      திரும்பு CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    எனில்(*header) {
      unsigned எழுத்து *hdr;
      size_t hdrlen;

      result = curlx_base64_decode(header, &hdr, &hdrlen);
      எனில்(!result) {
        கட்டமைப்பு bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        result = Curl_auth_decode_ntlm_type2_message(data, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      எனில்(result)
        திரும்பு result;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    இல்லையெனில் {
      எனில்(*state == NTLMSTATE_LAST) {
        infof(data, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      இல்லையெனில் எனில்(*state == NTLMSTATE_TYPE3) {
        infof(data, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        திரும்பு CURLE_REMOTE_ACCESS_DENIED;
      }
      இல்லையெனில் எனில்(*state >= NTLMSTATE_TYPE1) {
        infof(data, "NTLM handshake failure (internal error)");
        திரும்பு CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  திரும்பு result;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(கட்டமைப்பு Curl_easy *data, bool proxy)
{
  எழுத்து *base64 = வெற்று;
  size_t len = 0;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  எழுத்து **allocuserpwd;

  /* point to credentials and host */
  கட்டமைப்பு Curl_creds *creds = வெற்று;
  மாறிலி எழுத்து *hostname = வெற்று;

  /* point to the correct struct with this */
  கட்டமைப்பு ntlmdata *ntlm;
  curlntlm *state;
  கட்டமைப்பு auth *authp;
  கட்டமைப்பு connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(data);

  எனில்(proxy) {
#ifndef CURL_DISABLE_PROXY
    allocuserpwd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &data->state.authproxy;
#இல்லையெனில்
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }
  இல்லையெனில் {
    allocuserpwd = &data->req.hd_auth;
    creds = data->state.creds;
    hostname = data->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &data->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  எனில்(!ntlm)
    திரும்பு CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#ifdef USE_WINDOWS_SSPI
  எனில்(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    result = Curl_sspi_global_init();
    எனில்(!Curl_pSecFn)
      திரும்பு result;
  }
  ntlm->sslContext = conn->sslContext;
#endif

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  எனில்(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  தேர்வு(*state) {
  நிலை NTLMSTATE_TYPE1:
  இயல்பு:  /* for the weird cases we (re)start here */
    எனில்(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      எனில்(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        திரும்பு CURLE_FAILED_INIT;
      }
      எனில்(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        திரும்பு CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, data->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    result = Curl_auth_create_ntlm_type1_message(data, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    எனில்(!result) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      எனில்(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        எனில்(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
    நிறுத்து;

  நிலை NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    result = Curl_auth_create_ntlm_type3_message(data, creds, ntlm, &ntlmmsg);
    எனில்(!result && Curl_bufref_len(&ntlmmsg)) {
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      எனில்(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        எனில்(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
        இல்லையெனில் {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    நிறுத்து;

  நிலை NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    எனில்(proxy)
      data->info.proxyauthpicked = CURLAUTH_NTLM;
    இல்லையெனில்
      data->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    நிறுத்து;
  }
  Curl_bufref_free(&ntlmmsg);

  திரும்பு result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NTLM */
