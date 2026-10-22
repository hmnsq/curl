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

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "http_negotiate.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "curlx/strparse.h"


நிலையான வெறுமை http_auth_nego_reset(கட்டமைப்பு connectdata *conn,
                                 கட்டமைப்பு negotiatedata *neg_ctx,
                                 bool proxy)
{
  எனில்(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  இல்லையெனில் {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  எனில்(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_உள்ளீடு_negotiate(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn,
                              bool proxy, மாறிலி எழுத்து *header)
{
  CURLcode result;
  size_t len;

  /* Point to credentials and host */
  கட்டமைப்பு Curl_creds *creds = வெற்று;
  மாறிலி எழுத்து *host;

  /* Point to the correct struct with this */
  கட்டமைப்பு negotiatedata *neg_ctx;
  uint8_t state;

  எனில்(proxy) {
#ifndef CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#இல்லையெனில்
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }
  இல்லையெனில் {
    creds = data->state.creds;
    host = data->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  எனில்(!neg_ctx)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* Obtain the input token, if any */
  header += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&header);

  len = strlen(header);
  neg_ctx->havenegdata = len != 0;
  எனில்(!len) {
    எனில்(state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    இல்லையெனில் எனில்(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      திரும்பு CURLE_LOGIN_DENIED;
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
  எனில்(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_get_channel_binding(data, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_data);
    எனில்(result) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      திரும்பு result;
    }
  }
#இல்லையெனில்
  curlx_dyn_init(&neg_ctx->channel_binding_data, 1);
#endif /* USE_SSL */
#endif /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  result = Curl_auth_decode_spnego_message(data, creds, "HTTP",
                                           host, header, neg_ctx);

#ifdef GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_data);
#endif

  எனில்(result)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  எனில்(!result && !proxy) {
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

  திரும்பு result;
}

CURLcode Curl_output_negotiate(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn, bool proxy)
{
  கட்டமைப்பு negotiatedata *neg_ctx;
  கட்டமைப்பு auth *authp;
  uint8_t *state;
  எழுத்து *base64 = வெற்று;
  size_t len = 0;
  எழுத்து *userp;
  CURLcode result;

  எனில்(proxy) {
#ifndef CURL_DISABLE_PROXY
    authp = &data->state.authproxy;
    state = &conn->proxy_negotiate_state;
#இல்லையெனில்
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }
  இல்லையெனில் {
    authp = &data->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  எனில்(!neg_ctx)
    திரும்பு CURLE_OUT_OF_MEMORY;

  authp->done = FALSE;

  எனில்(*state == CURL_NEGO_AUTH_RECV) {
    எனில்(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  இல்லையெனில் எனில்(*state == CURL_NEGO_AUTH_SUCC) {
    எனில்(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  எனில்(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_DONE && *state != CURL_NEGO_AUTH_SUCC)) {

    எனில்(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    எனில்(!neg_ctx->context) {
      result = Curl_உள்ளீடு_negotiate(data, conn, proxy, "Negotiate");
      எனில்(result == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->done = TRUE;
        திரும்பு CURLE_OK;
      }
      இல்லையெனில் எனில்(result)
        திரும்பு result;
    }

    result = Curl_auth_create_spnego_message(neg_ctx, &base64, &len);
    எனில்(result)
      திரும்பு result;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    எனில்(proxy) {
#ifndef CURL_DISABLE_PROXY
      curlx_free(data->req.hd_proxy_auth);
      data->req.hd_proxy_auth = userp;
#endif
    }
    இல்லையெனில் {
      curlx_free(data->req.hd_auth);
      data->req.hd_auth = userp;
    }

    curlx_free(base64);

    எனில்(!userp) {
      திரும்பு CURLE_OUT_OF_MEMORY;
    }

    *state = CURL_NEGO_AUTH_SENT;
#ifdef HAVE_GSSAPI
    எனில்(neg_ctx->status == GSS_S_COMPLETE ||
       neg_ctx->status == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#இல்லையெனில்
#ifdef USE_WINDOWS_SSPI
    எனில்(neg_ctx->status == SEC_E_OK ||
       neg_ctx->status == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#endif
#endif
  }

  எனில்(*state == CURL_NEGO_AUTH_DONE || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->done = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  திரும்பு CURLE_OK;
}

#endif /* !CURL_DISABLE_HTTP && USE_SPNEGO */
