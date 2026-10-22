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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "http_negotiate.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"


ಸ್ಥಿರ ಶೂನ್ಯ http_auth_nego_reset(ರಚನೆ connectdata *conn,
                                 ರಚನೆ negotiatedata *neg_ctx,
                                 bool proxy)
{
  ಆದರೆ(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  ಇಲ್ಲದಿದ್ದರೆ {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  ಆದರೆ(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_ಒಳಹರಿವು_negotiate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                              bool proxy, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t len;

  /* Point to credentials and host */
  ರಚನೆ Curl_creds *creds = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host;

  /* Point to the correct struct with this */
  ರಚನೆ negotiatedata *neg_ctx;
  uint8_t state;

  ಆದರೆ(proxy) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    creds = ದತ್ತಾಂಶ->state.creds;
    host = ದತ್ತಾಂಶ->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ಆದರೆ(!neg_ctx)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Obtain the input token, if any */
  ಶೀರ್ಷಿಕೆ += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&ಶೀರ್ಷಿಕೆ);

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಶೀರ್ಷಿಕೆ);
  neg_ctx->havenegdata = len != 0;
  ಆದರೆ(!len) {
    ಆದರೆ(state == CURL_NEGO_AUTH_SUCC) {
      infof(ದತ್ತಾಂಶ, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
    }
  }

  /* Supports SSL channel binding for Windows ISS extended protection */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#ಆದರೆ_ಅಂತ್ಯ
  /* Check if the connection is using SSL and get the channel binding data */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GSS_C_CHANNEL_BOUND_FLAG
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_ದತ್ತಾಂಶ, SSL_CB_MAX_ಗಾತ್ರ + 1);
  ಆದರೆ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ಫಲಿತಾಂಶ = Curl_ssl_get_channel_binding(ದತ್ತಾಂಶ, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  curlx_dyn_init(&neg_ctx->channel_binding_ದತ್ತಾಂಶ, 1);
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */
#ಆದರೆ_ಅಂತ್ಯ /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  ಫಲಿತಾಂಶ = Curl_auth_decode_spnego_ಸಂದೇಶ(ದತ್ತಾಂಶ, creds, "HTTP",
                                           host, ಶೀರ್ಷಿಕೆ, neg_ctx);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(ಫಲಿತಾಂಶ)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  ಆದರೆ(!ಫಲಿತಾಂಶ && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    ಆದರೆ(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, ದತ್ತಾಂಶ->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
    ಆದರೆ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, ದತ್ತಾಂಶ->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_output_negotiate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn, bool proxy)
{
  ರಚನೆ negotiatedata *neg_ctx;
  ರಚನೆ auth *authp;
  uint8_t *state;
  ಅಕ್ಷರ *base64 = ಖಾಲಿ;
  size_t len = 0;
  ಅಕ್ಷರ *userp;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(proxy) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    authp = &ದತ್ತಾಂಶ->state.authproxy;
    state = &conn->proxy_negotiate_state;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    authp = &ದತ್ತಾಂಶ->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ಆದರೆ(!neg_ctx)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  authp->ಮುಗಿದ = FALSE;

  ಆದರೆ(*state == CURL_NEGO_AUTH_RECV) {
    ಆದರೆ(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*state == CURL_NEGO_AUTH_SUCC) {
    ಆದರೆ(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  ಆದರೆ(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_ಮುಗಿದ && *state != CURL_NEGO_AUTH_SUCC)) {

    ಆದರೆ(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(ದತ್ತಾಂಶ, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ಆದರೆ(!neg_ctx->context) {
      ಫಲಿತಾಂಶ = Curl_ಒಳಹರಿವು_negotiate(ದತ್ತಾಂಶ, conn, proxy, "Negotiate");
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->ಮುಗಿದ = TRUE;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಫಲಿತಾಂಶ = Curl_auth_create_spnego_ಸಂದೇಶ(neg_ctx, &base64, &len);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    ಆದರೆ(proxy) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
      curlx_free(ದತ್ತಾಂಶ->req.hd_proxy_auth);
      ದತ್ತಾಂಶ->req.hd_proxy_auth = userp;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      curlx_free(ದತ್ತಾಂಶ->req.hd_auth);
      ದತ್ತಾಂಶ->req.hd_auth = userp;
    }

    curlx_free(base64);

    ಆದರೆ(!userp) {
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    *state = CURL_NEGO_AUTH_ಕಳುಹಿಸಿದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI
    ಆದರೆ(neg_ctx->ಸ್ಥಿತಿ_2 == GSS_S_COMPLETE ||
       neg_ctx->ಸ್ಥಿತಿ_2 == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_ಮುಗಿದ;
    }
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
    ಆದರೆ(neg_ctx->ಸ್ಥಿತಿ_2 == SEC_E_OK ||
       neg_ctx->ಸ್ಥಿತಿ_2 == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_ಮುಗಿದ;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(*state == CURL_NEGO_AUTH_ಮುಗಿದ || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->ಮುಗಿದ = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_SPNEGO */
