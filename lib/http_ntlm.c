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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "http_ntlm.h"
#ଅନ୍ତର୍ଭାଵ "curl_ntlm_core.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ifdef USE_WINDOWS_SSPI
#ଅନ୍ତର୍ଭାଵ "curl_sspi.h"
#endif

CURLcode Curl_ନିବେଶ_ntlm(ସଂରଚନା Curl_easy *data,
                         bool proxy,         /* if proxy or not */
                         ଅଚଳ ଅକ୍ଷର *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  ଯଦି(checkprefix("NTLM", header)) {
    ସଂରଚନା ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    ଯଦି(!ntlm)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    ଯଦି(*header) {
      unsigned ଅକ୍ଷର *hdr;
      size_t hdrlen;

      result = curlx_base64_decode(header, &hdr, &hdrlen);
      ଯଦି(!result) {
        ସଂରଚନା bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        result = Curl_auth_decode_ntlm_type2_message(data, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    ଅନ୍ୟଥା {
      ଯଦି(*state == NTLMSTATE_LAST) {
        infof(data, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      ଅନ୍ୟଥା ଯଦି(*state == NTLMSTATE_TYPE3) {
        infof(data, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        ଫେରନ୍ତୁ CURLE_REMOTE_ACCESS_DENIED;
      }
      ଅନ୍ୟଥା ଯଦି(*state >= NTLMSTATE_TYPE1) {
        infof(data, "NTLM handshake failure (internal error)");
        ଫେରନ୍ତୁ CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  ଫେରନ୍ତୁ result;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(ସଂରଚନା Curl_easy *data, bool proxy)
{
  ଅକ୍ଷର *base64 = ଖାଲି;
  size_t len = 0;
  CURLcode result = CURLE_OK;
  ସଂରଚନା bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  ଅକ୍ଷର **allocuserpwd;

  /* point to credentials and host */
  ସଂରଚନା Curl_creds *creds = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *hostname = ଖାଲି;

  /* point to the correct struct with this */
  ସଂରଚନା ntlmdata *ntlm;
  curlntlm *state;
  ସଂରଚନା auth *authp;
  ସଂରଚନା connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(data);

  ଯଦି(proxy) {
#ifndef CURL_DISABLE_PROXY
    allocuserpwd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &data->state.authproxy;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  }
  ଅନ୍ୟଥା {
    allocuserpwd = &data->req.hd_auth;
    creds = data->state.creds;
    hostname = data->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &data->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  ଯଦି(!ntlm)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#ifdef USE_WINDOWS_SSPI
  ଯଦି(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    result = Curl_sspi_global_init();
    ଯଦି(!Curl_pSecFn)
      ଫେରନ୍ତୁ result;
  }
  ntlm->sslContext = conn->sslContext;
#endif

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  ଯଦି(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  ବାଛ(*state) {
  ସ୍ଥିତି NTLMSTATE_TYPE1:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  /* for the weird cases we (re)start here */
    ଯଦି(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      ଯଦି(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        ଫେରନ୍ତୁ CURLE_FAILED_INIT;
      }
      ଯଦି(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        ଫେରନ୍ତୁ CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, data->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    result = Curl_auth_create_ntlm_type1_message(data, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    ଯଦି(!result) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ଯଦି(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ଯଦି(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    result = Curl_auth_create_ntlm_type3_message(data, creds, ntlm, &ntlmmsg);
    ଯଦି(!result && Curl_bufref_len(&ntlmmsg)) {
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      ଯଦି(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        ଯଦି(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
        ଅନ୍ୟଥା {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    ଯଦି(proxy)
      data->info.proxyauthpicked = CURLAUTH_NTLM;
    ଅନ୍ୟଥା
      data->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    ଭାଙ୍ଗ;
  }
  Curl_bufref_free(&ntlmmsg);

  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NTLM */
