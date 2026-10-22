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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "http_negotiate.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "curlx/strparse.h"


സ്ഥിതം ശൂന്യം http_auth_nego_reset(ഘടന connectdata *conn,
                                 ഘടന negotiatedata *neg_ctx,
                                 bool proxy)
{
  എങ്കിൽ(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  അല്ലെങ്കിൽ {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  എങ്കിൽ(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_നിവേശം_negotiate(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                              bool proxy, സ്ഥിരം അക്ഷരം *header)
{
  CURLcode result;
  size_t len;

  /* Point to credentials and host */
  ഘടന Curl_creds *creds = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *host;

  /* Point to the correct struct with this */
  ഘടന negotiatedata *neg_ctx;
  uint8_t state;

  എങ്കിൽ(proxy) {
#ifndef CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#അല്ലെങ്കിൽ
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  }
  അല്ലെങ്കിൽ {
    creds = data->state.creds;
    host = data->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  എങ്കിൽ(!neg_ctx)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Obtain the input token, if any */
  header += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&header);

  len = strlen(header);
  neg_ctx->havenegdata = len != 0;
  എങ്കിൽ(!len) {
    എങ്കിൽ(state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      തിരികെ CURLE_LOGIN_DENIED;
    }
  }

  /* Supports SSL channel binding for Windows ISS extended protection */
#ifdef USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#endif
  /* Check if the connection is using SSL and get the channel binding data */
#ifdef GSS_C_CHANNEL_BOUND_FLAG
#ifdef USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_data, SSL_CB_MAX_SIZE + 1);
  എങ്കിൽ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_get_channel_binding(data, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_data);
    എങ്കിൽ(result) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      തിരികെ result;
    }
  }
#അല്ലെങ്കിൽ
  curlx_dyn_init(&neg_ctx->channel_binding_data, 1);
#endif /* USE_SSL */
#endif /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  result = Curl_auth_decode_spnego_message(data, creds, "HTTP",
                                           host, header, neg_ctx);

#ifdef GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_data);
#endif

  എങ്കിൽ(result)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  എങ്കിൽ(!result && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    എങ്കിൽ(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      തിരികെ CURLE_FAILED_INIT;
    }
    എങ്കിൽ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      തിരികെ CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, data->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  തിരികെ result;
}

CURLcode Curl_output_negotiate(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn, bool proxy)
{
  ഘടന negotiatedata *neg_ctx;
  ഘടന auth *authp;
  uint8_t *state;
  അക്ഷരം *base64 = ശൂന്യ;
  size_t len = 0;
  അക്ഷരം *userp;
  CURLcode result;

  എങ്കിൽ(proxy) {
#ifndef CURL_DISABLE_PROXY
    authp = &data->state.authproxy;
    state = &conn->proxy_negotiate_state;
#അല്ലെങ്കിൽ
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  }
  അല്ലെങ്കിൽ {
    authp = &data->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  എങ്കിൽ(!neg_ctx)
    തിരികെ CURLE_OUT_OF_MEMORY;

  authp->done = FALSE;

  എങ്കിൽ(*state == CURL_NEGO_AUTH_RECV) {
    എങ്കിൽ(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(*state == CURL_NEGO_AUTH_SUCC) {
    എങ്കിൽ(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  എങ്കിൽ(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_DONE && *state != CURL_NEGO_AUTH_SUCC)) {

    എങ്കിൽ(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    എങ്കിൽ(!neg_ctx->context) {
      result = Curl_നിവേശം_negotiate(data, conn, proxy, "Negotiate");
      എങ്കിൽ(result == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->done = TRUE;
        തിരികെ CURLE_OK;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(result)
        തിരികെ result;
    }

    result = Curl_auth_create_spnego_message(neg_ctx, &base64, &len);
    എങ്കിൽ(result)
      തിരികെ result;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    എങ്കിൽ(proxy) {
#ifndef CURL_DISABLE_PROXY
      curlx_free(data->req.hd_proxy_auth);
      data->req.hd_proxy_auth = userp;
#endif
    }
    അല്ലെങ്കിൽ {
      curlx_free(data->req.hd_auth);
      data->req.hd_auth = userp;
    }

    curlx_free(base64);

    എങ്കിൽ(!userp) {
      തിരികെ CURLE_OUT_OF_MEMORY;
    }

    *state = CURL_NEGO_AUTH_SENT;
#ifdef HAVE_GSSAPI
    എങ്കിൽ(neg_ctx->status == GSS_S_COMPLETE ||
       neg_ctx->status == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#അല്ലെങ്കിൽ
#ifdef USE_WINDOWS_SSPI
    എങ്കിൽ(neg_ctx->status == SEC_E_OK ||
       neg_ctx->status == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#endif
#endif
  }

  എങ്കിൽ(*state == CURL_NEGO_AUTH_DONE || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->done = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  തിരികെ CURLE_OK;
}

#endif /* !CURL_DISABLE_HTTP && USE_SPNEGO */
