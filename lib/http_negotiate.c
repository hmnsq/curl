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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_trc.h"
#ضمن "http_negotiate.h"
#ضمن "vauth/vauth.h"
#ضمن "vtls/vtls.h"
#ضمن "curlx/strparse.h"


ساكن فراغ http_auth_nego_reset(بنية connectdata *conn,
                                 بنية negotiatedata *neg_ctx,
                                 bool proxy)
{
  إذا(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  وإلا {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  إذا(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_ننوت_negotiate(بنية Curl_easy *معلومات, بنية connectdata *conn,
                              bool proxy, ثابت حرف *سرلیک)
{
  CURLcode پایله;
  size_t len;

  /* Point to credentials and host */
  بنية Curl_creds *creds = عدم;
  ثابت حرف *host;

  /* Point to the correct struct with this */
  بنية negotiatedata *neg_ctx;
  uint8_t state;

  إذا(proxy) {
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#وإلا
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  }
  وإلا {
    creds = معلومات->state.creds;
    host = معلومات->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  إذا(!neg_ctx)
    أرجع CURLE_بهر_د_حافظه;

  /* Obtain the input token, if any */
  سرلیک += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&سرلیک);

  len = مزي_اوږدوالی(سرلیک);
  neg_ctx->havenegdata = len != 0;
  إذا(!len) {
    إذا(state == CURL_NEGO_AUTH_SUCC) {
      infof(معلومات, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    وإلا إذا(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      أرجع CURLE_LOGIN_DENIED;
    }
  }

  /* Supports SSL channel binding for Windows ISS extended protection */
#که_تعریف_شوی USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#د_که_پای
  /* Check if the connection is using SSL and get the channel binding data */
#که_تعریف_شوی GSS_C_CHANNEL_BOUND_FLAG
#که_تعریف_شوی USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_معلومات, SSL_CB_MAX_اندازه + 1);
  إذا(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    پایله = Curl_ssl_get_channel_binding(معلومات, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_معلومات);
    إذا(پایله) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      أرجع پایله;
    }
  }
#وإلا
  curlx_dyn_init(&neg_ctx->channel_binding_معلومات, 1);
#د_که_پای /* USE_SSL */
#د_که_پای /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  پایله = Curl_auth_decode_spnego_پیغام(معلومات, creds, "HTTP",
                                           host, سرلیک, neg_ctx);

#که_تعریف_شوی GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_معلومات);
#د_که_پای

  إذا(پایله)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  إذا(!پایله && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    إذا(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, معلومات->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      أرجع CURLE_FAILED_INIT;
    }
    إذا(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      أرجع CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, معلومات->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  أرجع پایله;
}

CURLcode Curl_output_negotiate(بنية Curl_easy *معلومات,
                               بنية connectdata *conn, bool proxy)
{
  بنية negotiatedata *neg_ctx;
  بنية auth *authp;
  uint8_t *state;
  حرف *base64 = عدم;
  size_t len = 0;
  حرف *userp;
  CURLcode پایله;

  إذا(proxy) {
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    authp = &معلومات->state.authproxy;
    state = &conn->proxy_negotiate_state;
#وإلا
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  }
  وإلا {
    authp = &معلومات->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  إذا(!neg_ctx)
    أرجع CURLE_بهر_د_حافظه;

  authp->بشپړ = FALSE;

  إذا(*state == CURL_NEGO_AUTH_RECV) {
    إذا(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  وإلا إذا(*state == CURL_NEGO_AUTH_SUCC) {
    إذا(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  إذا(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_بشپړ && *state != CURL_NEGO_AUTH_SUCC)) {

    إذا(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(معلومات, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    إذا(!neg_ctx->context) {
      پایله = Curl_ننوت_negotiate(معلومات, conn, proxy, "Negotiate");
      إذا(پایله == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->بشپړ = TRUE;
        أرجع CURLE_OK;
      }
      وإلا إذا(پایله)
        أرجع پایله;
    }

    پایله = Curl_auth_create_spnego_پیغام(neg_ctx, &base64, &len);
    إذا(پایله)
      أرجع پایله;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    إذا(proxy) {
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
      curlx_free(معلومات->req.hd_proxy_auth);
      معلومات->req.hd_proxy_auth = userp;
#د_که_پای
    }
    وإلا {
      curlx_free(معلومات->req.hd_auth);
      معلومات->req.hd_auth = userp;
    }

    curlx_free(base64);

    إذا(!userp) {
      أرجع CURLE_بهر_د_حافظه;
    }

    *state = CURL_NEGO_AUTH_لېږل_شوی;
#که_تعریف_شوی HAVE_GSSAPI
    إذا(neg_ctx->حالت == GSS_S_COMPLETE ||
       neg_ctx->حالت == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_بشپړ;
    }
#وإلا
#که_تعریف_شوی USE_WINDOWS_SSPI
    إذا(neg_ctx->حالت == SEC_E_OK ||
       neg_ctx->حالت == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_بشپړ;
    }
#د_که_پای
#د_که_پای
  }

  إذا(*state == CURL_NEGO_AUTH_بشپړ || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->بشپړ = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  أرجع CURLE_OK;
}

#د_که_پای /* !CURL_DISABLE_HTTP && USE_SPNEGO */
