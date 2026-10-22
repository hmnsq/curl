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
#include "curl_setup.h"

#အကယ်၍ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

#include "urldata.h"
#include "cfilters.h"
#include "curl_trc.h"
#include "http_negotiate.h"
#include "vauth/vauth.h"
#include "vtls/vtls.h"
#include "curlx/strparse.h"


static void http_auth_nego_reset(sa_nracanaa connectdata *conn,
                                 sa_nracanaa negotiatedata *neg_ctx,
                                 bool proxy)
{
  အကယ်၍(proxy)
    conn->proxy_negotiate_state = CURL_NEGO_AUTH_NONE;
  မဟုတ်ပါက {
    conn->http_negotiate_state = CURL_NEGO_AUTH_NONE;
    Curl_peer_unlink(&conn->creds_origin);
    Curl_creds_unlink(&conn->creds);
  }
  အကယ်၍(neg_ctx)
    Curl_auth_cleanup_spnego(neg_ctx);
}

CURLcode Curl_input_negotiate(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                              bool proxy, const var_na *header)
{
  CURLcode result;
  size_t len;

  /* Point to credentials and host */
  sa_nracanaa Curl_creds *creds = NULL;
  const var_na *host;

  /* Point to the correct struct with this */
  sa_nracanaa negotiatedata *neg_ctx;
  uint8_t state;

  အကယ်၍(proxy) {
#ifndef CURL_DISABLE_PROXY
    creds = conn->http_proxy.creds;
    host = conn->http_proxy.peer->hostname;
    state = conn->proxy_negotiate_state;
#မဟုတ်ပါက
    ပြန်ပေး CURLE_NOT_BUILT_IN;
#endif
  }
  မဟုတ်ပါက {
    creds = data->state.creds;
    host = data->state.origin->hostname;
    state = conn->http_negotiate_state;
  }

  neg_ctx = Curl_auth_nego_get(conn, proxy);
  အကယ်၍(!neg_ctx)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  /* Obtain the input token, if any */
  header += CURL_CSTRLEN("Negotiate");
  curlx_str_passblanks(&header);

  len = strlen(header);
  neg_ctx->havenegdata = len != 0;
  အကယ်၍(!len) {
    အကယ်၍(state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Negotiate auth restarted");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    မဟုတ်ပါက အကယ်၍(state != CURL_NEGO_AUTH_NONE) {
      /* The server rejected our authentication and has not supplied any more
      negotiation mechanisms */
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ပြန်ပေး CURLE_LOGIN_DENIED;
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
  အကယ်၍(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_get_channel_binding(data, FIRSTSOCKET,
                                          &neg_ctx->channel_binding_data);
    အကယ်၍(result) {
      http_auth_nego_reset(conn, neg_ctx, proxy);
      ပြန်ပေး result;
    }
  }
#မဟုတ်ပါက
  curlx_dyn_init(&neg_ctx->channel_binding_data, 1);
#endif /* USE_SSL */
#endif /* GSS_C_CHANNEL_BOUND_FLAG */

  /* Initialize the security context and decode our challenge */
  result = Curl_auth_decode_spnego_message(data, creds, "HTTP",
                                           host, header, neg_ctx);

#ifdef GSS_C_CHANNEL_BOUND_FLAG
  curlx_dyn_free(&neg_ctx->channel_binding_data);
#endif

  အကယ်၍(result)
    http_auth_nego_reset(conn, neg_ctx, proxy);

  အကယ်၍(!result && !proxy) {
    /* Start it up. From this time onwards, the connection is tied
     * tp the credentials used. */
    အကယ်၍(conn->creds_origin &&
       !Curl_peer_equal(conn->creds_origin, data->state.origin)) {
      DEBUGASSERT(0); /* should not happen. */
      ပြန်ပေး CURLE_FAILED_INIT;
    }
    အကယ်၍(conn->creds && !Curl_creds_same(creds, conn->creds)) {
      DEBUGASSERT(0); /* should not happen. */
      ပြန်ပေး CURLE_FAILED_INIT;
    }
    Curl_peer_link(&conn->creds_origin, data->state.origin);
    Curl_creds_link(&conn->creds, creds);
  }

  ပြန်ပေး result;
}

CURLcode Curl_output_negotiate(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn, bool proxy)
{
  sa_nracanaa negotiatedata *neg_ctx;
  sa_nracanaa auth *authp;
  uint8_t *state;
  var_na *base64 = NULL;
  size_t len = 0;
  var_na *userp;
  CURLcode result;

  အကယ်၍(proxy) {
#ifndef CURL_DISABLE_PROXY
    authp = &data->state.authproxy;
    state = &conn->proxy_negotiate_state;
#မဟုတ်ပါက
    ပြန်ပေး CURLE_NOT_BUILT_IN;
#endif
  }
  မဟုတ်ပါက {
    authp = &data->state.authhost;
    state = &conn->http_negotiate_state;
  }
  neg_ctx = Curl_auth_nego_get(conn, proxy);
  အကယ်၍(!neg_ctx)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  authp->done = FALSE;

  အကယ်၍(*state == CURL_NEGO_AUTH_RECV) {
    အကယ်၍(neg_ctx->havenegdata) {
      neg_ctx->havemultiplerequests = TRUE;
    }
  }
  မဟုတ်ပါက အကယ်၍(*state == CURL_NEGO_AUTH_SUCC) {
    အကယ်၍(!neg_ctx->havenoauthpersist) {
      neg_ctx->noauthpersist = !neg_ctx->havemultiplerequests;
    }
  }

  အကယ်၍(neg_ctx->noauthpersist ||
     (*state != CURL_NEGO_AUTH_DONE && *state != CURL_NEGO_AUTH_SUCC)) {

    အကယ်၍(neg_ctx->noauthpersist && *state == CURL_NEGO_AUTH_SUCC) {
      infof(data, "Curl_output_negotiate, "
            "no persistent authentication: cleanup existing context");
      http_auth_nego_reset(conn, neg_ctx, proxy);
    }
    အကယ်၍(!neg_ctx->context) {
      result = Curl_input_negotiate(data, conn, proxy, "Negotiate");
      အကယ်၍(result == CURLE_AUTH_ERROR) {
        /* negotiate auth failed, let's continue unauthenticated to stay
         * compatible with the behavior before curl-7_64_0-158-g6c6035532 */
        authp->done = TRUE;
        ပြန်ပေး CURLE_OK;
      }
      မဟုတ်ပါက အကယ်၍(result)
        ပြန်ပေး result;
    }

    result = Curl_auth_create_spnego_message(neg_ctx, &base64, &len);
    အကယ်၍(result)
      ပြန်ပေး result;

    userp = curl_maprintf("%sAuthorization: Negotiate %s\r\n",
                          proxy ? "Proxy-" : "", base64);

    အကယ်၍(proxy) {
#ifndef CURL_DISABLE_PROXY
      curlx_free(data->req.hd_proxy_auth);
      data->req.hd_proxy_auth = userp;
#endif
    }
    မဟုတ်ပါက {
      curlx_free(data->req.hd_auth);
      data->req.hd_auth = userp;
    }

    curlx_free(base64);

    အကယ်၍(!userp) {
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
    }

    *state = CURL_NEGO_AUTH_SENT;
#ifdef HAVE_GSSAPI
    အကယ်၍(neg_ctx->status == GSS_S_COMPLETE ||
       neg_ctx->status == GSS_S_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#မဟုတ်ပါက
#ifdef USE_WINDOWS_SSPI
    အကယ်၍(neg_ctx->status == SEC_E_OK ||
       neg_ctx->status == SEC_I_CONTINUE_NEEDED) {
      *state = CURL_NEGO_AUTH_DONE;
    }
#endif
#endif
  }

  အကယ်၍(*state == CURL_NEGO_AUTH_DONE || *state == CURL_NEGO_AUTH_SUCC) {
    /* connection is already authenticated,
     * do not send a header in future requests */
    authp->done = TRUE;
  }

  neg_ctx->havenegdata = FALSE;

  ပြန်ပေး CURLE_OK;
}

#endif /* !CURL_DISABLE_HTTP && USE_SPNEGO */
