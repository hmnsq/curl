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

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "strcase.h"
#समावेश "http_ntlm.h"
#समावेश "curl_ntlm_core.h"
#समावेश "curlx/base64.h"
#समावेश "vauth/vauth.h"
#समावेश "url.h"
#समावेश "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#अगर_घोषित USE_WINDOWS_SSPI
#समावेश "curl_sspi.h"
#पूर्ण_अगर

CURLcode Curl_निवेश_ntlm(काष्ठा Curl_easy *तथ्य,
                         bool proxy,         /* if proxy or not */
                         स्थिर अक्षर *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  अगर(checkprefix("NTLM", header)) {
    काष्ठा ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    अगर(!ntlm)
      वापस CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    अगर(*header) {
      अचिन्हित अक्षर *hdr;
      माप_प्रकार hdrlen;

      परिणाम = curlx_base64_decode(header, &hdr, &hdrlen);
      अगर(!परिणाम) {
        काष्ठा bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        परिणाम = Curl_auth_decode_ntlm_type2_message(तथ्य, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      अगर(परिणाम)
        वापस परिणाम;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    अन्यथा {
      अगर(*state == NTLMSTATE_LAST) {
        infof(तथ्य, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      अन्यथा अगर(*state == NTLMSTATE_TYPE3) {
        infof(तथ्य, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        वापस CURLE_REMOTE_ACCESS_DENIED;
      }
      अन्यथा अगर(*state >= NTLMSTATE_TYPE1) {
        infof(तथ्य, "NTLM handshake failure (internal error)");
        वापस CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  वापस परिणाम;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(काष्ठा Curl_easy *तथ्य, bool proxy)
{
  अक्षर *base64 = शून्य;
  माप_प्रकार len = 0;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  अक्षर **allocuserpwd;

  /* point to credentials and host */
  काष्ठा Curl_creds *creds = शून्य;
  स्थिर अक्षर *hostname = शून्य;

  /* point to the correct struct with this */
  काष्ठा ntlmdata *ntlm;
  curlntlm *state;
  काष्ठा auth *authp;
  काष्ठा connectdata *conn = तथ्य->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(तथ्य);

  अगर(proxy) {
#अगर_अघोषित CURL_DISABLE_PROXY
    allocuserpwd = &तथ्य->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &तथ्य->state.authproxy;
#अन्यथा
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }
  अन्यथा {
    allocuserpwd = &तथ्य->req.hd_auth;
    creds = तथ्य->state.creds;
    hostname = तथ्य->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &तथ्य->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  अगर(!ntlm)
    वापस CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#अगर_घोषित USE_WINDOWS_SSPI
  अगर(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    परिणाम = Curl_sspi_global_init();
    अगर(!Curl_pSecFn)
      वापस परिणाम;
  }
  ntlm->sslContext = conn->sslContext;
#पूर्ण_अगर

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  अगर(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  चयन(*state) {
  हाल NTLMSTATE_TYPE1:
  शेष:  /* for the weird cases we (re)start here */
    अगर(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      अगर(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, तथ्य->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        वापस CURLE_FAILED_INIT;
      }
      अगर(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        वापस CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, तथ्य->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    परिणाम = Curl_auth_create_ntlm_type1_message(तथ्य, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    अगर(!परिणाम) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      परिणाम = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      अगर(!परिणाम) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        अगर(!*allocuserpwd)
          परिणाम = CURLE_OUT_OF_MEMORY;
      }
    }
    अवरोध;

  हाल NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    परिणाम = Curl_auth_create_ntlm_type3_message(तथ्य, creds, ntlm, &ntlmmsg);
    अगर(!परिणाम && Curl_bufref_len(&ntlmmsg)) {
      परिणाम = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      अगर(!परिणाम) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        अगर(!*allocuserpwd)
          परिणाम = CURLE_OUT_OF_MEMORY;
        अन्यथा {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    अवरोध;

  हाल NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    अगर(proxy)
      तथ्य->info.proxyauthpicked = CURLAUTH_NTLM;
    अन्यथा
      तथ्य->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    अवरोध;
  }
  Curl_bufref_free(&ntlmmsg);

  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_NTLM */
