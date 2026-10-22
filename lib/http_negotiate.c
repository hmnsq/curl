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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "http_negotiate.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"


ಸ್ಥಿರ ಶೂನ್ಯ http_auth_nego_reset(ಸಂರಚನಾ connectdata *conn,
                                 ಸಂರಚನಾ negotiatedata *neg_ctx,
                                 bool proxy)
{
  ಯದಿ(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  ಅನ್ಯಥಾ {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  ಯದಿ(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_ಒಳಹರಿವು_negotiate(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                              bool proxy, ಧ್ರುವ ವರ್ಣ *header)
{
  CURLcode result;
  size_t len;

  /* Point to credentials and host */
  ಸಂರಚನಾ Curl_creds *creds = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *host;

  /* Point to the correct struct with this */
  ಸಂರಚನಾ negotiatedata *neg_ctx;
  uint8_t state;

  ಯದಿ(proxy) {
#ifndef CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }
  ಅನ್ಯಥಾ {
    creds = data->state.creds;
    host = data->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ಯದಿ(!neg_ctx)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* Obtain the input token, if any */
  header += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&header);

  len = strlen(header);
  neg_ctx->havenegdata = len != 0;
  ಯದಿ(!len) {
    ಯದಿ(state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ಅನ್ಯಥಾ ಯದಿ(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ಪ್ರತ್ಯಾಗಮ CURLE_LOGIN_DENIED;
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
  ಯದಿ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_get_channel_binding(data, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_data);
    ಯದಿ(result) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
#ಅನ್ಯಥಾ
  curlx_dyn_init(&neg_ctx->channel_binding_data, 1);
#endif /* USE_SSL */
#endif /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  result = Curl_auth_decode_spnego_message(data, creds, "HTTP",
                                           host, header, neg_ctx);

#ifdef GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_data);
#endif

  ಯದಿ(result)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  ಯದಿ(!result && !proxy) {
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

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_output_negotiate(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn, bool proxy)
{
  ಸಂರಚನಾ negotiatedata *neg_ctx;
  ಸಂರಚನಾ auth *authp;
  uint8_t *state;
  ವರ್ಣ *base64 = ರಿಕ್ತ;
  size_t len = 0;
  ವರ್ಣ *userp;
  CURLcode result;

  ಯದಿ(proxy) {
#ifndef CURL_DISABLE_PROXY
    authp = &data->state.authproxy;
    state = &conn->proxy_negotiate_state;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }
  ಅನ್ಯಥಾ {
    authp = &data->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ಯದಿ(!neg_ctx)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  authp->done = FALSE;

  ಯದಿ(*state == CURL_NEGO_AUTH_RECV) {
    ಯದಿ(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(*state == CURL_NEGO_AUTH_SUCC) {
    ಯದಿ(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  ಯದಿ(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_DONE && *state != CURL_NEGO_AUTH_SUCC)) {

    ಯದಿ(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ಯದಿ(!neg_ctx->context) {
      result = Curl_ಒಳಹರಿವು_negotiate(data, conn, proxy, "Negotiate");
      ಯದಿ(result == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->done = TRUE;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಅನ್ಯಥಾ ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }

    result = Curl_auth_create_spnego_message(neg_ctx, &base64, &len);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    ಯದಿ(proxy) {
#ifndef CURL_DISABLE_PROXY
      curlx_free(data->req.hd_proxy_auth);
      data->req.hd_proxy_auth = userp;
#endif
    }
    ಅನ್ಯಥಾ {
      curlx_free(data->req.hd_auth);
      data->req.hd_auth = userp;
    }

    curlx_free(base64);

    ಯದಿ(!userp) {
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }

    *state = CURL_NEGO_AUTH_SENT;
#ifdef HAVE_GSSAPI
    ಯದಿ(neg_ctx->status == GSS_S_COMPLETE ||
       neg_ctx->status == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#ಅನ್ಯಥಾ
#ifdef USE_WINDOWS_SSPI
    ಯದಿ(neg_ctx->status == SEC_E_OK ||
       neg_ctx->status == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#endif
#endif
  }

  ಯದಿ(*state == CURL_NEGO_AUTH_DONE || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->done = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#endif /* !CURL_DISABLE_HTTP && USE_SPNEGO */
