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
#अन्तर्भाव "curl_setup.h"

#जर !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "http_ntlm.h"
#अन्तर्भाव "curl_ntlm_core.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ifdef USE_WINDOWS_SSPI
#अन्तर्भाव "curl_sspi.h"
#endif

CURLcode Curl_आदान_ntlm(संरचना Curl_easy *data,
                         bool proxy,         /* if proxy or not */
                         स्थिरांक अक्षर *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode result = CURLE_OK;
  संरचना connectdata *conn = data->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  जर(checkprefix("NTLM", header)) {
    संरचना ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    जर(!ntlm)
      परत CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    जर(*header) {
      अचिन्हित अक्षर *hdr;
      माप_प्रकार hdrlen;

      result = curlx_base64_decode(header, &hdr, &hdrlen);
      जर(!result) {
        संरचना bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        result = Curl_auth_decode_ntlm_type2_message(data, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      जर(result)
        परत result;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    अन्यथा {
      जर(*state == NTLMSTATE_LAST) {
        infof(data, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      अन्यथा जर(*state == NTLMSTATE_TYPE3) {
        infof(data, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        परत CURLE_REMOTE_ACCESS_DENIED;
      }
      अन्यथा जर(*state >= NTLMSTATE_TYPE1) {
        infof(data, "NTLM handshake failure (internal error)");
        परत CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  परत result;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(संरचना Curl_easy *data, bool proxy)
{
  अक्षर *base64 = रिक्त;
  माप_प्रकार len = 0;
  CURLcode result = CURLE_OK;
  संरचना bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  अक्षर **allocuserpwd;

  /* point to credentials and host */
  संरचना Curl_creds *creds = रिक्त;
  स्थिरांक अक्षर *hostname = रिक्त;

  /* point to the correct struct with this */
  संरचना ntlmdata *ntlm;
  curlntlm *state;
  संरचना auth *authp;
  संरचना connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(data);

  जर(proxy) {
#ifndef CURL_DISABLE_PROXY
    allocuserpwd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &data->state.authproxy;
#अन्यथा
    परत CURLE_NOT_BUILT_IN;
#endif
  }
  अन्यथा {
    allocuserpwd = &data->req.hd_auth;
    creds = data->state.creds;
    hostname = data->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &data->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  जर(!ntlm)
    परत CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#ifdef USE_WINDOWS_SSPI
  जर(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    result = Curl_sspi_global_init();
    जर(!Curl_pSecFn)
      परत result;
  }
  ntlm->sslContext = conn->sslContext;
#endif

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  जर(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  निवड(*state) {
  स्थिती NTLMSTATE_TYPE1:
  पूर्वनिर्धारित:  /* for the weird cases we (re)start here */
    जर(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      जर(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        परत CURLE_FAILED_INIT;
      }
      जर(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        परत CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, data->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    result = Curl_auth_create_ntlm_type1_message(data, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    जर(!result) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      जर(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        जर(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
    थांबा;

  स्थिती NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    result = Curl_auth_create_ntlm_type3_message(data, creds, ntlm, &ntlmmsg);
    जर(!result && Curl_bufref_len(&ntlmmsg)) {
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      जर(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        जर(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
        अन्यथा {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    थांबा;

  स्थिती NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    जर(proxy)
      data->info.proxyauthpicked = CURLAUTH_NTLM;
    अन्यथा
      data->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    थांबा;
  }
  Curl_bufref_free(&ntlmmsg);

  परत result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NTLM */
