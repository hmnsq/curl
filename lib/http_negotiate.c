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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_negotiate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"


ਸਥਿਰ ਖਾਲੀ http_auth_nego_reset(ਬਣਤਰ connectdata *conn,
                                 ਬਣਤਰ negotiatedata *neg_ctx,
                                 bool proxy)
{
  ਜੇ(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  ਨਹੀਂਤਾਂ {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  ਜੇ(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_ਨਿਵੇਸ਼_negotiate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                              bool proxy, ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ)
{
  CURLcode ਨਤੀਜਾ;
  size_t len;

  /* Point to credentials and host */
  ਬਣਤਰ Curl_creds *creds = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *host;

  /* Point to the correct struct with this */
  ਬਣਤਰ negotiatedata *neg_ctx;
  uint8_t state;

  ਜੇ(proxy) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    creds = ਅੰਕੜੇ->state.creds;
    host = ਅੰਕੜੇ->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ਜੇ(!neg_ctx)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Obtain the input token, if any */
  ਸਿਰਲੇਖ += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&ਸਿਰਲੇਖ);

  len = ਲੜੀ_ਲੰਬਾਈ(ਸਿਰਲੇਖ);
  neg_ctx->havenegdata = len != 0;
  ਜੇ(!len) {
    ਜੇ(state == CURL_NEGO_AUTH_SUCC) {
      infof(ਅੰਕੜੇ, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ਨਹੀਂਤਾਂ ਜੇ(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ਵਾਪਸ CURLE_LOGIN_DENIED;
    }
  }

  /* Supports SSL channel binding for Windows ISS extended protection */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#ਜੇ_ਅੰਤ
  /* Check if the connection is using SSL and get the channel binding data */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GSS_C_CHANNEL_BOUND_FLAG
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_ਅੰਕੜੇ, SSL_CB_MAX_ਮਾਪ + 1);
  ਜੇ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ਨਤੀਜਾ = Curl_ssl_get_channel_binding(ਅੰਕੜੇ, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
#ਨਹੀਂਤਾਂ
  curlx_dyn_init(&neg_ctx->channel_binding_ਅੰਕੜੇ, 1);
#ਜੇ_ਅੰਤ /* USE_SSL */
#ਜੇ_ਅੰਤ /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  ਨਤੀਜਾ = Curl_auth_decode_spnego_ਸੁਨੇਹਾ(ਅੰਕੜੇ, creds, "HTTP",
                                           host, ਸਿਰਲੇਖ, neg_ctx);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ

  ਜੇ(ਨਤੀਜਾ)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  ਜੇ(!ਨਤੀਜਾ && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    ਜੇ(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, ਅੰਕੜੇ->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
    ਜੇ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, ਅੰਕੜੇ->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_output_negotiate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn, bool proxy)
{
  ਬਣਤਰ negotiatedata *neg_ctx;
  ਬਣਤਰ auth *authp;
  uint8_t *state;
  ਅੱਖਰ *base64 = ਨਿਰਾ;
  size_t len = 0;
  ਅੱਖਰ *userp;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(proxy) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    authp = &ਅੰਕੜੇ->state.authproxy;
    state = &conn->proxy_negotiate_state;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    authp = &ਅੰਕੜੇ->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ਜੇ(!neg_ctx)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  authp->ਪੂਰਾ = FALSE;

  ਜੇ(*state == CURL_NEGO_AUTH_RECV) {
    ਜੇ(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(*state == CURL_NEGO_AUTH_SUCC) {
    ਜੇ(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  ਜੇ(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_ਪੂਰਾ && *state != CURL_NEGO_AUTH_SUCC)) {

    ਜੇ(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(ਅੰਕੜੇ, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ਜੇ(!neg_ctx->context) {
      ਨਤੀਜਾ = Curl_ਨਿਵੇਸ਼_negotiate(ਅੰਕੜੇ, conn, proxy, "Negotiate");
      ਜੇ(ਨਤੀਜਾ == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->ਪੂਰਾ = TRUE;
        ਵਾਪਸ CURLE_OK;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਨਤੀਜਾ = Curl_auth_create_spnego_ਸੁਨੇਹਾ(neg_ctx, &base64, &len);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    ਜੇ(proxy) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
      curlx_free(ਅੰਕੜੇ->req.hd_proxy_auth);
      ਅੰਕੜੇ->req.hd_proxy_auth = userp;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ {
      curlx_free(ਅੰਕੜੇ->req.hd_auth);
      ਅੰਕੜੇ->req.hd_auth = userp;
    }

    curlx_free(base64);

    ਜੇ(!userp) {
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    *state = CURL_NEGO_AUTH_ਭੇਜਿਆ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI
    ਜੇ(neg_ctx->ਸਥਿਤੀ == GSS_S_COMPLETE ||
       neg_ctx->ਸਥਿਤੀ == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_ਪੂਰਾ;
    }
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
    ਜੇ(neg_ctx->ਸਥਿਤੀ == SEC_E_OK ||
       neg_ctx->ਸਥਿਤੀ == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_ਪੂਰਾ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
  }

  ਜੇ(*state == CURL_NEGO_AUTH_ਪੂਰਾ || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->ਪੂਰਾ = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_SPNEGO */
