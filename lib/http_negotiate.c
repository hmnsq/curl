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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "http_negotiate.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "curlx/strparse.h"


স্থির শূন্য http_auth_nego_reset(সংরচনা connectdata *conn,
                                 সংরচনা negotiatedata *neg_ctx,
                                 bool proxy)
{
  যদি(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  অন্যথা {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  যদি(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_প্রবেশ_negotiate(সংরচনা Curl_easy *data, সংরচনা connectdata *conn,
                              bool proxy, ধ্রুব বর্ণ *header)
{
  CURLcode result;
  size_t len;

  /* Point to credentials and host */
  সংরচনা Curl_creds *creds = রিক্ত;
  ধ্রুব বর্ণ *host;

  /* Point to the correct struct with this */
  সংরচনা negotiatedata *neg_ctx;
  uint8_t state;

  যদি(proxy) {
#ifndef CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#অন্যথা
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  }
  অন্যথা {
    creds = data->state.creds;
    host = data->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  যদি(!neg_ctx)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  /* Obtain the input token, if any */
  header += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&header);

  len = strlen(header);
  neg_ctx->havenegdata = len != 0;
  যদি(!len) {
    যদি(state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    অন্যথা যদি(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      প্রত্যাগম CURLE_LOGIN_DENIED;
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
  যদি(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_get_channel_binding(data, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_data);
    যদি(result) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      প্রত্যাগম result;
    }
  }
#অন্যথা
  curlx_dyn_init(&neg_ctx->channel_binding_data, 1);
#endif /* USE_SSL */
#endif /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  result = Curl_auth_decode_spnego_message(data, creds, "HTTP",
                                           host, header, neg_ctx);

#ifdef GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_data);
#endif

  যদি(result)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  যদি(!result && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    যদি(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      প্রত্যাগম CURLE_FAILED_INIT;
    }
    যদি(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      প্রত্যাগম CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, data->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  প্রত্যাগম result;
}

CURLcode Curl_output_negotiate(সংরচনা Curl_easy *data,
                               সংরচনা connectdata *conn, bool proxy)
{
  সংরচনা negotiatedata *neg_ctx;
  সংরচনা auth *authp;
  uint8_t *state;
  বর্ণ *base64 = রিক্ত;
  size_t len = 0;
  বর্ণ *userp;
  CURLcode result;

  যদি(proxy) {
#ifndef CURL_DISABLE_PROXY
    authp = &data->state.authproxy;
    state = &conn->proxy_negotiate_state;
#অন্যথা
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  }
  অন্যথা {
    authp = &data->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  যদি(!neg_ctx)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  authp->done = FALSE;

  যদি(*state == CURL_NEGO_AUTH_RECV) {
    যদি(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  অন্যথা যদি(*state == CURL_NEGO_AUTH_SUCC) {
    যদি(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  যদি(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_DONE && *state != CURL_NEGO_AUTH_SUCC)) {

    যদি(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    যদি(!neg_ctx->context) {
      result = Curl_প্রবেশ_negotiate(data, conn, proxy, "Negotiate");
      যদি(result == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->done = TRUE;
        প্রত্যাগম CURLE_OK;
      }
      অন্যথা যদি(result)
        প্রত্যাগম result;
    }

    result = Curl_auth_create_spnego_message(neg_ctx, &base64, &len);
    যদি(result)
      প্রত্যাগম result;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    যদি(proxy) {
#ifndef CURL_DISABLE_PROXY
      curlx_free(data->req.hd_proxy_auth);
      data->req.hd_proxy_auth = userp;
#endif
    }
    অন্যথা {
      curlx_free(data->req.hd_auth);
      data->req.hd_auth = userp;
    }

    curlx_free(base64);

    যদি(!userp) {
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    *state = CURL_NEGO_AUTH_SENT;
#ifdef HAVE_GSSAPI
    যদি(neg_ctx->status == GSS_S_COMPLETE ||
       neg_ctx->status == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#অন্যথা
#ifdef USE_WINDOWS_SSPI
    যদি(neg_ctx->status == SEC_E_OK ||
       neg_ctx->status == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#endif
#endif
  }

  যদি(*state == CURL_NEGO_AUTH_DONE || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->done = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  প্রত্যাগম CURLE_OK;
}

#endif /* !CURL_DISABLE_HTTP && USE_SPNEGO */
