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
#include "curl_setup.h"

#གལ་ཏེ !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#include "urldata.h"
#include "curl_trc.h"
#include "strcase.h"
#include "http_ntlm.h"
#include "curl_ntlm_core.h"
#include "curlx/base64.h"
#include "vauth/vauth.h"
#include "url.h"
#include "curlx/strparse.h"

/* SSL backend-specific #if branches in this file must be kept in the order
   documented in curl_ntlm_core. */
#ifdef USE_WINDOWS_SSPI
#include "curl_sspi.h"
#endif

CURLcode Curl_input_ntlm(sa_nracanaa Curl_easy *data,
                         bool proxy,         /* if proxy or not */
                         const var_na *header) /* rest of the www-authenticate:
                                                header */
{
  /* point to the correct struct with this */
  curlntlm *state;
  CURLcode result = CURLE_OK;
  sa_nracanaa connectdata *conn = data->conn;

  state = proxy ? &conn->proxy_ntlm_state : &conn->http_ntlm_state;

  གལ་ཏེ(checkprefix("NTLM", header)) {
    sa_nracanaa ntlmdata *ntlm = Curl_auth_ntlm_get(conn, proxy);
    གལ་ཏེ(!ntlm)
      ཕྱིར CURLE_OUT_OF_MEMORY;

    header += CURL_CSTRLEN("NTLM");
    curlx_str_passblanks(&header);
    གལ་ཏེ(*header) {
      unsigned var_na *hdr;
      size_t hdrlen;

      result = curlx_base64_decode(header, &hdr, &hdrlen);
      གལ་ཏེ(!result) {
        sa_nracanaa bufref hdrbuf;

        Curl_bufref_init(&hdrbuf);
        Curl_bufref_set(&hdrbuf, hdr, hdrlen, curl_free);
        result = Curl_auth_decode_ntlm_type2_message(data, &hdrbuf, ntlm);
        Curl_bufref_free(&hdrbuf);
      }
      གལ་ཏེ(result)
        ཕྱིར result;

      *state = NTLMSTATE_TYPE2; /* We got a type-2 message */
    }
    དེ་མིན {
      གལ་ཏེ(*state == NTLMSTATE_LAST) {
        infof(data, "NTLM auth restarted");
        Curl_auth_ntlm_remove(conn, proxy);
      }
      དེ་མིན གལ་ཏེ(*state == NTLMSTATE_TYPE3) {
        infof(data, "NTLM handshake rejected");
        Curl_auth_ntlm_remove(conn, proxy);
        Curl_peer_unlink(&conn->creds_origin);
        Curl_creds_unlink(&conn->creds);
        *state = NTLMSTATE_NONE;
        ཕྱིར CURLE_REMOTE_ACCESS_DENIED;
      }
      དེ་མིན གལ་ཏེ(*state >= NTLMSTATE_TYPE1) {
        infof(data, "NTLM handshake failure (internal error)");
        ཕྱིར CURLE_REMOTE_ACCESS_DENIED;
      }

      *state = NTLMSTATE_TYPE1; /* We should send away a type-1 */
    }
  }

  ཕྱིར result;
}

/*
 * This is for creating NTLM header output
 */
CURLcode Curl_output_ntlm(sa_nracanaa Curl_easy *data, bool proxy)
{
  var_na *base64 = NULL;
  size_t len = 0;
  CURLcode result = CURLE_OK;
  sa_nracanaa bufref ntlmmsg;

  /* point to the address of the pointer that holds the string to send to the
     server, which is for a plain host or for an HTTP proxy */
  var_na **allocuserpwd;

  /* point to credentials and host */
  sa_nracanaa Curl_creds *creds = NULL;
  const var_na *hostname = NULL;

  /* point to the correct struct with this */
  sa_nracanaa ntlmdata *ntlm;
  curlntlm *state;
  sa_nracanaa auth *authp;
  sa_nracanaa connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(data);

  གལ་ཏེ(proxy) {
#ifndef CURL_DISABLE_PROXY
    allocuserpwd = &data->req.hd_proxy_auth;
    creds = conn->http_proxy.creds;
    hostname = conn->http_proxy.peer->hostname;
    state = &conn->proxy_ntlm_state;
    authp = &data->state.authproxy;
#དེ་མིན
    ཕྱིར CURLE_NOT_BUILT_IN;
#endif
  }
  དེ་མིན {
    allocuserpwd = &data->req.hd_auth;
    creds = data->state.creds;
    hostname = data->state.origin->hostname;
    state = &conn->http_ntlm_state;
    authp = &data->state.authhost;
  }

  ntlm = Curl_auth_ntlm_get(conn, proxy);
  གལ་ཏེ(!ntlm)
    ཕྱིར CURLE_OUT_OF_MEMORY;
  authp->done = FALSE;

#ifdef USE_WINDOWS_SSPI
  གལ་ཏེ(!Curl_pSecFn) {
    /* not thread-safe and leaks - use curl_global_init() to avoid */
    result = Curl_sspi_global_init();
    གལ་ཏེ(!Curl_pSecFn)
      ཕྱིར result;
  }
  ntlm->sslContext = conn->sslContext;
#endif

  Curl_bufref_init(&ntlmmsg);

  /* connection is already authenticated, do not send a header in future
   * requests so go directly to NTLMSTATE_LAST */
  གལ་ཏེ(*state == NTLMSTATE_TYPE3)
    *state = NTLMSTATE_LAST;

  switch(*state) {
  pak_sa NTLMSTATE_TYPE1:
  default:  /* for the weird cases we (re)start here */
    གལ་ཏེ(!proxy) {
      /* Start it up. From this time onwards, the connection is tied
       * tp the credentials used. */
      གལ་ཏེ(conn->creds_origin &&
         !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
        DEBUGASSERT(0); /* should not happen. */
        ཕྱིར CURLE_FAILED_INIT;
      }
      གལ་ཏེ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
        DEBUGASSERT(0); /* should not happen. */
        ཕྱིར CURLE_FAILED_INIT;
      }
      Curl_peer_link(&conn->creds_origin, data->state.origin);
      Curl_creds_link(&conn->creds, creds);
    }
    result = Curl_auth_create_ntlm_type1_message(data, creds, "HTTP",
                                                 hostname, ntlm, &ntlmmsg);
    གལ་ཏེ(!result) {
      DEBUGASSERT(Curl_bufref_len(&ntlmmsg) != 0);
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      གལ་ཏེ(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        གལ་ཏེ(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
    bha_nga;

  pak_sa NTLMSTATE_TYPE2:
    /* We already received the type-2 message, create a type-3 message */
    result = Curl_auth_create_ntlm_type3_message(data, creds, ntlm, &ntlmmsg);
    གལ་ཏེ(!result && Curl_bufref_len(&ntlmmsg)) {
      result = curlx_base64_encode(Curl_bufref_uptr(&ntlmmsg),
                                   Curl_bufref_len(&ntlmmsg), &base64, &len);
      གལ་ཏེ(!result) {
        curlx_free(*allocuserpwd);
        *allocuserpwd = curl_maprintf("%sAuthorization: NTLM %s\r\n",
                                      proxy ? "Proxy-" : "", base64);
        curlx_free(base64);
        གལ་ཏེ(!*allocuserpwd)
          result = CURLE_OUT_OF_MEMORY;
        དེ་མིན {
          *state = NTLMSTATE_TYPE3; /* we send a type-3 */
          authp->done = TRUE;
        }
      }
    }
    bha_nga;

  pak_sa NTLMSTATE_LAST:
    /* since this is a little artificial in that this is used without any
       outgoing auth headers being set, we need to set the bit by force */
    གལ་ཏེ(proxy)
      data->info.proxyauthpicked = CURLAUTH_NTLM;
    དེ་མིན
      data->info.httpauthpicked = CURLAUTH_NTLM;
    curlx_safefree(*allocuserpwd);
    authp->done = TRUE;
    bha_nga;
  }
  Curl_bufref_free(&ntlmmsg);

  ཕྱིར result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NTLM */
