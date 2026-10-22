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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "http_ntlm.h"
#અન્તર્ભાવ "curl_ntlm_core.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ifdef USE_WINDOWS_SSPI
#અન્તર્ભાવ "curl_sspi.h"
#endif

CURLcode Curl_આગત_ntlm(સંરચના Curl_easy *data,
                         bool proxy,         /* if proxy or not */
                         અચલ અક્ષર *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode result = CURLE_OK;
  સંરચના connectdata *conn = data->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  જો(checkprefix("NTLM", header)) {
    સંરચના ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    જો(!ntlm)
      પાછા CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    જો(*header) {
      unsigned અક્ષર *hdr;
      size_t hdrlen;

      result = curlx_base64_decode(header, &hdr, &hdrlen);
      જો(!result) {
        સંરચના bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        result = Curl_auth_decode_ntlm_type2_message(data, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      જો(result)
        પાછા result;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    નહીંતર {
      જો(*state == NTLMSTATE_LAST) {
        infof(data, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      નહીંતર જો(*state == NTLMSTATE_TYPE3) {
        infof(data, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        પાછા CURLE_REMOTE_ACCESS_DENIED;
      }
      નહીંતર જો(*state >= NTLMSTATE_TYPE1) {
        infof(data, "NTLM handshake failure (internal error)");
        પાછા CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  પાછા result;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(સંરચના Curl_easy *data, bool proxy)
{
  અક્ષર *base64 = ખાલી;
  size_t len = 0;
  CURLcode result = CURLE_OK;
  સંરચના bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  અક્ષર **allocuserpwd;

  /* point to credentials and host */
  સંરચના Curl_creds *creds = ખાલી;
  અચલ અક્ષર *hostname = ખાલી;

  /* point to the correct struct with this */
  સંરચના ntlmdata *ntlm;
  curlntlm *state;
  સંરચના auth *authp;
  સંરચના connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(data);

  જો(proxy) {
#ifndef CURL_DISABLE_PROXY
    allocuserpwd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &data->state.authproxy;
#નહીંતર
    પાછા CURLE_NOT_BUILT_IN;
#endif
  }
  નહીંતર {
    allocuserpwd = &data->req.hd_auth;
    creds = data->state.creds;
    hostname = data->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &data->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  જો(!ntlm)
    પાછા CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#ifdef USE_WINDOWS_SSPI
  જો(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    result = Curl_sspi_global_init();
    જો(!Curl_pSecFn)
      પાછા result;
  }
  ntlm->sslContext = conn->sslContext;
#endif

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  જો(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  પસંદગી(*state) {
  સ્થિતિ NTLMSTATE_TYPE1:
  મૂળભૂત:  /* for the weird cases we (re)start here */
    જો(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      જો(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        પાછા CURLE_FAILED_INIT;
      }
      જો(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        પાછા CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, data->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    result = Curl_auth_create_ntlm_type1_message(data, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    જો(!result) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      જો(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        જો(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
    તોડો;

  સ્થિતિ NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    result = Curl_auth_create_ntlm_type3_message(data, creds, ntlm, &ntlmmsg);
    જો(!result && Curl_bufref_len(&ntlmmsg)) {
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      જો(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        જો(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
        નહીંતર {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    તોડો;

  સ્થિતિ NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    જો(proxy)
      data->info.proxyauthpicked = CURLAUTH_NTLM;
    નહીંતર
      data->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    તોડો;
  }
  Curl_bufref_free(&ntlmmsg);

  પાછા result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NTLM */
