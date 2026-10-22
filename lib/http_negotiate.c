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

CURLcode Curl_निवेशः_negotiate(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                              bool proxy, ध्रुव वर्ण *शीर्षकम्)
{
  CURLcode परिणाम;
  size_t len;

  /* Point to credentials and host */
  संरचना Curl_creds *creds = रिक्त;
  ध्रुव वर्ण *host;

  /* Point to the correct struct with this */
  संरचना negotiatedata *neg_ctx;
  uint8_t state;

  यदि(proxy) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }
  अन्यथा {
    creds = दत्त->state.creds;
    host = दत्त->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  यदि(!neg_ctx)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Obtain the input token, if any */
  शीर्षकम् += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&शीर्षकम्);

  len = वाक्यदैर्घ्यम्(शीर्षकम्);
  neg_ctx->havenegdata = len != 0;
  यदि(!len) {
    यदि(state == CURL_NEGO_AUTH_SUCC) {
      infof(दत्त, "Negotiate auth restarted");
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
#यदि_परिभाषितम् USE_WINDOWS_SSPI
  neg_ctx->sslContext = conn->sslContext;
#यदि_अन्तः
  /* Check if the connection is using SSL and get the channel binding data */
#यदि_परिभाषितम् GSS_C_CHANNEL_BOUND_FLAG
#यदि_परिभाषितम् USE_SSL
  curlx_dyn_init(&neg_ctx->channel_binding_दत्त, SSL_CB_MAX_आकार + 1);
  यदि(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_get_channel_binding(दत्त, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_दत्त);
    यदि(परिणाम) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      प्रत्यागम परिणाम;
    }
  }
#अन्यथा
  curlx_dyn_init(&neg_ctx->channel_binding_दत्त, 1);
#यदि_अन्तः /* USE_SSL */
#यदि_अन्तः /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  परिणाम = Curl_auth_decode_spnego_सन्देशः(दत्त, creds, "HTTP",
                                           host, शीर्षकम्, neg_ctx);

#यदि_परिभाषितम् GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_दत्त);
#यदि_अन्तः

  यदि(परिणाम)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  यदि(!परिणाम && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    यदि(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, दत्त->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      प्रत्यागम CURLE_FAILED_INIT;
    }
    यदि(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      प्रत्यागम CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, दत्त->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  प्रत्यागम परिणाम;
}

CURLcode Curl_output_negotiate(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn, bool proxy)
{
  संरचना negotiatedata *neg_ctx;
  संरचना auth *authp;
  uint8_t *state;
  वर्ण *base64 = रिक्त;
  size_t len = 0;
  वर्ण *userp;
  CURLcode परिणाम;

  यदि(proxy) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    authp = &दत्त->state.authproxy;
    state = &conn->proxy_negotiate_state;
#अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }
  अन्यथा {
    authp = &दत्त->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  यदि(!neg_ctx)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  authp->समाप्तम् = FALSE;

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
     (*state != CURL_NEGO_AUTH_समाप्तम् && *state != CURL_NEGO_AUTH_SUCC)) {

    यदि(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(दत्त, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    यदि(!neg_ctx->context) {
      परिणाम = Curl_निवेशः_negotiate(दत्त, conn, proxy, "Negotiate");
      यदि(परिणाम == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->समाप्तम् = TRUE;
        प्रत्यागम CURLE_OK;
      }
      अन्यथा यदि(परिणाम)
        प्रत्यागम परिणाम;
    }

    परिणाम = Curl_auth_create_spnego_सन्देशः(neg_ctx, &base64, &len);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    यदि(proxy) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
      curlx_free(दत्त->req.hd_proxy_auth);
      दत्त->req.hd_proxy_auth = userp;
#यदि_अन्तः
    }
    अन्यथा {
      curlx_free(दत्त->req.hd_auth);
      दत्त->req.hd_auth = userp;
    }

    curlx_free(base64);

    यदि(!userp) {
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    *state = CURL_NEGO_AUTH_प्रेषितम्;
#यदि_परिभाषितम् HAVE_GSSAPI
    यदि(neg_ctx->स्थितिः == GSS_S_COMPLETE ||
       neg_ctx->स्थितिः == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_समाप्तम्;
    }
#अन्यथा
#यदि_परिभाषितम् USE_WINDOWS_SSPI
    यदि(neg_ctx->स्थितिः == SEC_E_OK ||
       neg_ctx->स्थितिः == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_समाप्तम्;
    }
#यदि_अन्तः
#यदि_अन्तः
  }

  यदि(*state == CURL_NEGO_AUTH_समाप्तम् || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->समाप्तम् = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  प्रत्यागम CURLE_OK;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && USE_SPNEGO */
