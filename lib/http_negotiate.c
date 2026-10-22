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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "http_negotiate.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "curlx/strparse.h"


స్థిర శూన్య http_auth_nego_reset(నిర్మాణం connectdata *conn,
                                 నిర్మాణం negotiatedata *neg_ctx,
                                 bool proxy)
{
  ఒకవేళ(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  లేకపోతే {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  ఒకవేళ(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_ఆదానం_negotiate(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                              bool proxy, స్థిరరాశి అక్షరం *శీర్షిక)
{
  CURLcode ఫలితం;
  size_t len;

  /* Point to credentials and host */
  నిర్మాణం Curl_creds *creds = ఖాళీ;
  స్థిరరాశి అక్షరం *host;

  /* Point to the correct struct with this */
  నిర్మాణం negotiatedata *neg_ctx;
  uint8_t state;

  ఒకవేళ(proxy) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#లేకపోతే
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }
  లేకపోతే {
    creds = దత్తాంశం->state.creds;
    host = దత్తాంశం->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ఒకవేళ(!neg_ctx)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Obtain the input token, if any */
  శీర్షిక += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&శీర్షిక);

  len = అక్షరశ్రేణి_పొడవు(శీర్షిక);
  neg_ctx->havenegdata = len != 0;
  ఒకవేళ(!len) {
    ఒకవేళ(state == CURL_NEGO_AUTH_SUCC) {
      infof(దత్తాంశం, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    లేకపోతే ఒకవేళ(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      తిరిగి CURLE_LOGIN_DENIED;
    }
  }

  /* Supports SSL channel binding for Windows ISS extended protection */
#నిర్వచితమైతే USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#అయితే_ముగింపు
  /* Check if the connection is using SSL and get the channel binding data */
#నిర్వచితమైతే GSS_C_CHANNEL_BOUND_FLAG
#నిర్వచితమైతే USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_దత్తాంశం, SSL_CB_MAX_కొలత + 1);
  ఒకవేళ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ఫలితం = Curl_ssl_get_channel_binding(దత్తాంశం, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_దత్తాంశం);
    ఒకవేళ(ఫలితం) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      తిరిగి ఫలితం;
    }
  }
#లేకపోతే
  curlx_dyn_init(&neg_ctx->channel_binding_దత్తాంశం, 1);
#అయితే_ముగింపు /* USE_SSL */
#అయితే_ముగింపు /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  ఫలితం = Curl_auth_decode_spnego_సందేశం(దత్తాంశం, creds, "HTTP",
                                           host, శీర్షిక, neg_ctx);

#నిర్వచితమైతే GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_దత్తాంశం);
#అయితే_ముగింపు

  ఒకవేళ(ఫలితం)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  ఒకవేళ(!ఫలితం && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    ఒకవేళ(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, దత్తాంశం->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      తిరిగి CURLE_FAILED_INIT;
    }
    ఒకవేళ(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      తిరిగి CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, దత్తాంశం->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  తిరిగి ఫలితం;
}

CURLcode Curl_output_negotiate(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn, bool proxy)
{
  నిర్మాణం negotiatedata *neg_ctx;
  నిర్మాణం auth *authp;
  uint8_t *state;
  అక్షరం *base64 = ఖాళీ;
  size_t len = 0;
  అక్షరం *userp;
  CURLcode ఫలితం;

  ఒకవేళ(proxy) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    authp = &దత్తాంశం->state.authproxy;
    state = &conn->proxy_negotiate_state;
#లేకపోతే
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }
  లేకపోతే {
    authp = &దత్తాంశం->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  ఒకవేళ(!neg_ctx)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  authp->పూర్తి = FALSE;

  ఒకవేళ(*state == CURL_NEGO_AUTH_RECV) {
    ఒకవేళ(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  లేకపోతే ఒకవేళ(*state == CURL_NEGO_AUTH_SUCC) {
    ఒకవేళ(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  ఒకవేళ(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_పూర్తి && *state != CURL_NEGO_AUTH_SUCC)) {

    ఒకవేళ(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(దత్తాంశం, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    ఒకవేళ(!neg_ctx->context) {
      ఫలితం = Curl_ఆదానం_negotiate(దత్తాంశం, conn, proxy, "Negotiate");
      ఒకవేళ(ఫలితం == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->పూర్తి = TRUE;
        తిరిగి CURLE_OK;
      }
      లేకపోతే ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }

    ఫలితం = Curl_auth_create_spnego_సందేశం(neg_ctx, &base64, &len);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    ఒకవేళ(proxy) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
      curlx_free(దత్తాంశం->req.hd_proxy_auth);
      దత్తాంశం->req.hd_proxy_auth = userp;
#అయితే_ముగింపు
    }
    లేకపోతే {
      curlx_free(దత్తాంశం->req.hd_auth);
      దత్తాంశం->req.hd_auth = userp;
    }

    curlx_free(base64);

    ఒకవేళ(!userp) {
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    *state = CURL_NEGO_AUTH_పంపిన;
#నిర్వచితమైతే HAVE_GSSAPI
    ఒకవేళ(neg_ctx->స్థితి_2 == GSS_S_COMPLETE ||
       neg_ctx->స్థితి_2 == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_పూర్తి;
    }
#లేకపోతే
#నిర్వచితమైతే USE_WINDOWS_SSPI
    ఒకవేళ(neg_ctx->స్థితి_2 == SEC_E_OK ||
       neg_ctx->స్థితి_2 == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_పూర్తి;
    }
#అయితే_ముగింపు
#అయితే_ముగింపు
  }

  ఒకవేళ(*state == CURL_NEGO_AUTH_పూర్తి || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->పూర్తి = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  తిరిగి CURLE_OK;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_SPNEGO */
