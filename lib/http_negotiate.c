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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "http_negotiate.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "curlx/strparse.h"


स्थिर शून्य http_auth_nego_reset(संरचना connectdata *conn,
                                 संरचना negotiatedata *neg_ctx,
                                 bool proxy)
{
  यदि(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  अन्यथा {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  यदि(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_निवेश_negotiate(संरचना Curl_easy *data, संरचना connectdata *conn,
                              bool proxy, ध्रुव वर्ण *header)
{
  CURLcode result;
  size_t len;

  /* Point to credentials and host */
  संरचना Curl_creds *creds = रिक्त;
  ध्रुव वर्ण *host;

  /* Point to the correct struct with this */
  संरचना negotiatedata *neg_ctx;
  uint8_t state;

  यदि(proxy) {
#ifndef CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  }
  अन्यथा {
    creds = data->state.creds;
    host = data->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  यदि(!neg_ctx)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* Obtain the input token, if any */
  header += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&header);

  len = strlen(header);
  neg_ctx->havenegdata = len != 0;
  यदि(!len) {
    यदि(state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    अन्यथा यदि(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      प्रत्यागम CURLE_LOGIN_DENIED;
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
  यदि(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_get_channel_binding(data, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_data);
    यदि(result) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      प्रत्यागम result;
    }
  }
#अन्यथा
  curlx_dyn_init(&neg_ctx->channel_binding_data, 1);
#endif /* USE_SSL */
#endif /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  result = Curl_auth_decode_spnego_message(data, creds, "HTTP",
                                           host, header, neg_ctx);

#ifdef GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_data);
#endif

  यदि(result)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  यदि(!result && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    यदि(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      प्रत्यागम CURLE_FAILED_INIT;
    }
    यदि(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      प्रत्यागम CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, data->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  प्रत्यागम result;
}

CURLcode Curl_output_negotiate(संरचना Curl_easy *data,
                               संरचना connectdata *conn, bool proxy)
{
  संरचना negotiatedata *neg_ctx;
  संरचना auth *authp;
  uint8_t *state;
  वर्ण *base64 = रिक्त;
  size_t len = 0;
  वर्ण *userp;
  CURLcode result;

  यदि(proxy) {
#ifndef CURL_DISABLE_PROXY
    authp = &data->state.authproxy;
    state = &conn->proxy_negotiate_state;
#अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  }
  अन्यथा {
    authp = &data->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  यदि(!neg_ctx)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  authp->done = FALSE;

  यदि(*state == CURL_NEGO_AUTH_RECV) {
    यदि(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  अन्यथा यदि(*state == CURL_NEGO_AUTH_SUCC) {
    यदि(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  यदि(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_DONE && *state != CURL_NEGO_AUTH_SUCC)) {

    यदि(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    यदि(!neg_ctx->context) {
      result = Curl_निवेश_negotiate(data, conn, proxy, "Negotiate");
      यदि(result == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->done = TRUE;
        प्रत्यागम CURLE_OK;
      }
      अन्यथा यदि(result)
        प्रत्यागम result;
    }

    result = Curl_auth_create_spnego_message(neg_ctx, &base64, &len);
    यदि(result)
      प्रत्यागम result;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    यदि(proxy) {
#ifndef CURL_DISABLE_PROXY
      curlx_free(data->req.hd_proxy_auth);
      data->req.hd_proxy_auth = userp;
#endif
    }
    अन्यथा {
      curlx_free(data->req.hd_auth);
      data->req.hd_auth = userp;
    }

    curlx_free(base64);

    यदि(!userp) {
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }

    *state = CURL_NEGO_AUTH_SENT;
#ifdef HAVE_GSSAPI
    यदि(neg_ctx->status == GSS_S_COMPLETE ||
       neg_ctx->status == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#अन्यथा
#ifdef USE_WINDOWS_SSPI
    यदि(neg_ctx->status == SEC_E_OK ||
       neg_ctx->status == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#endif
#endif
  }

  यदि(*state == CURL_NEGO_AUTH_DONE || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->done = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  प्रत्यागम CURLE_OK;
}

#endif /* !CURL_DISABLE_HTTP && USE_SPNEGO */
