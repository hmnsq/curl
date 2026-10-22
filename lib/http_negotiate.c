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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "curl_trc.h"
#समावेश "http_negotiate.h"
#समावेश "vauth/vauth.h"
#समावेश "vtls/vtls.h"
#समावेश "curlx/strparse.h"


अटल व्योम http_auth_nego_reset(काष्ठा connectdata *conn,
                                 काष्ठा negotiatedata *neg_ctx,
                                 bool proxy)
{
  अगर(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  अन्यथा {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  अगर(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_निवेश_negotiate(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                              bool proxy, स्थिर अक्षर *header)
{
  CURLcode परिणाम;
  माप_प्रकार len;

  /* Point to credentials and host */
  काष्ठा Curl_creds *creds = शून्य;
  स्थिर अक्षर *host;

  /* Point to the correct struct with this */
  काष्ठा negotiatedata *neg_ctx;
  uint8_t state;

  अगर(proxy) {
#अगर_अघोषित CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#अन्यथा
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }
  अन्यथा {
    creds = तथ्य->state.creds;
    host = तथ्य->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  अगर(!neg_ctx)
    वापस CURLE_OUT_OF_MEMORY;

  /* Obtain the input token, if any */
  header += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&header);

  len = म_माप(header);
  neg_ctx->havenegdata = len != 0;
  अगर(!len) {
    अगर(state == CURL_NEGO_AUTH_SUCC) {
      infof(तथ्य, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    अन्यथा अगर(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      वापस CURLE_LOGIN_DENIED;
    }
  }

  /* Supports SSL channel binding for Windows ISS extended protection */
#अगर_घोषित USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#पूर्ण_अगर
  /* Check if the connection is using SSL and get the channel binding data */
#अगर_घोषित GSS_C_CHANNEL_BOUND_FLAG
#अगर_घोषित USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_तथ्य, SSL_CB_MAX_आकार + 1);
  अगर(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_get_channel_binding(तथ्य, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_तथ्य);
    अगर(परिणाम) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      वापस परिणाम;
    }
  }
#अन्यथा
  curlx_dyn_init(&neg_ctx->channel_binding_तथ्य, 1);
#पूर्ण_अगर /* USE_SSL */
#पूर्ण_अगर /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  परिणाम = Curl_auth_decode_spnego_message(तथ्य, creds, "HTTP",
                                           host, header, neg_ctx);

#अगर_घोषित GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_तथ्य);
#पूर्ण_अगर

  अगर(परिणाम)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  अगर(!परिणाम && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    अगर(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, तथ्य->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      वापस CURLE_FAILED_INIT;
    }
    अगर(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      वापस CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, तथ्य->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  वापस परिणाम;
}

CURLcode Curl_output_negotiate(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn, bool proxy)
{
  काष्ठा negotiatedata *neg_ctx;
  काष्ठा auth *authp;
  uint8_t *state;
  अक्षर *base64 = शून्य;
  माप_प्रकार len = 0;
  अक्षर *userp;
  CURLcode परिणाम;

  अगर(proxy) {
#अगर_अघोषित CURL_DISABLE_PROXY
    authp = &तथ्य->state.authproxy;
    state = &conn->proxy_negotiate_state;
#अन्यथा
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }
  अन्यथा {
    authp = &तथ्य->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  अगर(!neg_ctx)
    वापस CURLE_OUT_OF_MEMORY;

  authp->done = FALSE;

  अगर(*state == CURL_NEGO_AUTH_RECV) {
    अगर(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  अन्यथा अगर(*state == CURL_NEGO_AUTH_SUCC) {
    अगर(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  अगर(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_DONE && *state != CURL_NEGO_AUTH_SUCC)) {

    अगर(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(तथ्य, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    अगर(!neg_ctx->context) {
      परिणाम = Curl_निवेश_negotiate(तथ्य, conn, proxy, "Negotiate");
      अगर(परिणाम == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->done = TRUE;
        वापस CURLE_OK;
      }
      अन्यथा अगर(परिणाम)
        वापस परिणाम;
    }

    परिणाम = Curl_auth_create_spnego_message(neg_ctx, &base64, &len);
    अगर(परिणाम)
      वापस परिणाम;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    अगर(proxy) {
#अगर_अघोषित CURL_DISABLE_PROXY
      curlx_free(तथ्य->req.hd_proxy_auth);
      तथ्य->req.hd_proxy_auth = userp;
#पूर्ण_अगर
    }
    अन्यथा {
      curlx_free(तथ्य->req.hd_auth);
      तथ्य->req.hd_auth = userp;
    }

    curlx_free(base64);

    अगर(!userp) {
      वापस CURLE_OUT_OF_MEMORY;
    }

    *state = CURL_NEGO_AUTH_SENT;
#अगर_घोषित HAVE_GSSAPI
    अगर(neg_ctx->status == GSS_S_COMPLETE ||
       neg_ctx->status == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#अन्यथा
#अगर_घोषित USE_WINDOWS_SSPI
    अगर(neg_ctx->status == SEC_E_OK ||
       neg_ctx->status == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#पूर्ण_अगर
#पूर्ण_अगर
  }

  अगर(*state == CURL_NEGO_AUTH_DONE || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->done = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  वापस CURLE_OK;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_SPNEGO */
