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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_trc.h"
#شامل "http_negotiate.h"
#شامل "vauth/vauth.h"
#شامل "vtls/vtls.h"
#شامل "curlx/strparse.h"


ساکن خالی http_auth_nego_reset(ساخت connectdata *conn,
                                 ساخت negotiatedata *neg_ctx,
                                 bool proxy)
{
  اگر(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  ورنہ {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  اگر(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_داخلہ_negotiate(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                              bool proxy, ثابت حرف *سرخی)
{
  CURLcode نتیجہ;
  size_t len;

  /* Point to credentials and host */
  ساخت Curl_creds *creds = تہی;
  ثابت حرف *host;

  /* Point to the correct struct with this */
  ساخت negotiatedata *neg_ctx;
  uint8_t state;

  اگر(proxy) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#ورنہ
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }
  ورنہ {
    creds = ڈیٹا->state.creds;
    host = ڈیٹا->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  اگر(!neg_ctx)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Obtain the input token, if any */
  سرخی += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&سرخی);

  len = لڑی_لمبائی(سرخی);
  neg_ctx->havenegdata = len != 0;
  اگر(!len) {
    اگر(state == CURL_NEGO_AUTH_SUCC) {
      infof(ڈیٹا, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ورنہ اگر(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      واپس CURLE_LOGIN_DENIED;
    }
  }

  /* Supports SSL channel binding for Windows ISS extended protection */
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#اگر_ختم
  /* Check if the connection is using SSL and get the channel binding data */
#اگر_تعریف_شدہ GSS_C_CHANNEL_BOUND_FLAG
#اگر_تعریف_شدہ USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_ڈیٹا, SSL_CB_MAX_سائز + 1);
  اگر(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = Curl_ssl_get_channel_binding(ڈیٹا, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_ڈیٹا);
    اگر(نتیجہ) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      واپس نتیجہ;
    }
  }
#ورنہ
  curlx_dyn_init(&neg_ctx->channel_binding_ڈیٹا, 1);
#اگر_ختم /* USE_SSL */
#اگر_ختم /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  نتیجہ = Curl_auth_decode_spnego_پیغام(ڈیٹا, creds, "HTTP",
                                           host, سرخی, neg_ctx);

#اگر_تعریف_شدہ GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_ڈیٹا);
#اگر_ختم

  اگر(نتیجہ)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  اگر(!نتیجہ && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    اگر(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, ڈیٹا->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      واپس CURLE_FAILED_INIT;
    }
    اگر(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      واپس CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, ڈیٹا->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  واپس نتیجہ;
}

CURLcode Curl_output_negotiate(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn, bool proxy)
{
  ساخت negotiatedata *neg_ctx;
  ساخت auth *authp;
  uint8_t *state;
  حرف *base64 = تہی;
  size_t len = 0;
  حرف *userp;
  CURLcode نتیجہ;

  اگر(proxy) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    authp = &ڈیٹا->state.authproxy;
    state = &conn->proxy_negotiate_state;
#ورنہ
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }
  ورنہ {
    authp = &ڈیٹا->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  اگر(!neg_ctx)
    واپس CURLE_برآمد_کا_حافظہ;

  authp->مکمل = FALSE;

  اگر(*state == CURL_NEGO_AUTH_RECV) {
    اگر(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  ورنہ اگر(*state == CURL_NEGO_AUTH_SUCC) {
    اگر(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  اگر(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_مکمل && *state != CURL_NEGO_AUTH_SUCC)) {

    اگر(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(ڈیٹا, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    اگر(!neg_ctx->context) {
      نتیجہ = Curl_داخلہ_negotiate(ڈیٹا, conn, proxy, "Negotiate");
      اگر(نتیجہ == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->مکمل = TRUE;
        واپس CURLE_OK;
      }
      ورنہ اگر(نتیجہ)
        واپس نتیجہ;
    }

    نتیجہ = Curl_auth_create_spnego_پیغام(neg_ctx, &base64, &len);
    اگر(نتیجہ)
      واپس نتیجہ;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    اگر(proxy) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
      curlx_free(ڈیٹا->req.hd_proxy_auth);
      ڈیٹا->req.hd_proxy_auth = userp;
#اگر_ختم
    }
    ورنہ {
      curlx_free(ڈیٹا->req.hd_auth);
      ڈیٹا->req.hd_auth = userp;
    }

    curlx_free(base64);

    اگر(!userp) {
      واپس CURLE_برآمد_کا_حافظہ;
    }

    *state = CURL_NEGO_AUTH_بھیجا;
#اگر_تعریف_شدہ HAVE_GSSAPI
    اگر(neg_ctx->حالت_2 == GSS_S_COMPLETE ||
       neg_ctx->حالت_2 == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_مکمل;
    }
#ورنہ
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
    اگر(neg_ctx->حالت_2 == SEC_E_OK ||
       neg_ctx->حالت_2 == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_مکمل;
    }
#اگر_ختم
#اگر_ختم
  }

  اگر(*state == CURL_NEGO_AUTH_مکمل || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->مکمل = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  واپس CURLE_OK;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && USE_SPNEGO */
