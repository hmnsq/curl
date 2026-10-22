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

#إذا defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#ضمن <openssl/err.h>
#ضمن "vtls/openssl.h"
#وإلاإذا defined(USE_GNUTLS)
#ضمن <gnutls/abstract.h>
#ضمن <gnutls/gnutls.h>
#ضمن <gnutls/x509.h>
#ضمن <gnutls/crypto.h>
#ضمن <nettle/sha2.h>
#ضمن "vtls/gtls.h"
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <wolfssl/options.h>
#ضمن <wolfssl/ssl.h>
#ضمن <wolfssl/quic.h>
#ضمن "vtls/wolfssl.h"
#endif

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(بنية Curl_peer *origin,
                                  بنية Curl_peer *peer,
                                  بنية ssl_filter_config *sslc,
                                  بنية ssl_peer *ssl_peer)
{
  حرف tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, حجم(tls_id));
#وإلاإذا defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, حجم(tls_id));
#وإلاإذا defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, حجم(tls_id));
#وإلا
#error "no TLS lib in used, should not happen"
  إرجاع CURLE_FAILED_INIT;
#endif
  إذا(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  إرجاع Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(بنية curl_tls_ctx *ctx,
                             بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             بنية ssl_peer *ssl_peer,
                             ثابت بنية alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             فراغ *cb_user_data, فراغ *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  إرجاع Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, عدم, ssl_user_data,
                            session_reuse_cb);
#وإلاإذا defined(USE_GNUTLS)
  إرجاع Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#وإلاإذا defined(USE_WOLFSSL)
  إرجاع Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#وإلا
#error "no TLS lib in used, should not happen"
  إرجاع CURLE_FAILED_INIT;
#endif
}

فراغ Curl_vquic_tls_cleanup(بنية curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#وإلاإذا defined(USE_GNUTLS)
  إذا(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#وإلاإذا defined(USE_WOLFSSL)
  إذا(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  إذا(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, حجم(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(بنية curl_tls_ctx *ctx,
                                    بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data)
{
#ifdef USE_OPENSSL
  إذا(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    إذا(result)
      إرجاع result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#وإلاإذا defined(USE_WOLFSSL)
  إذا(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    إذا(result)
      إرجاع result;
  }
#وإلاإذا defined(USE_GNUTLS)
  إذا(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    إذا(result)
      إرجاع result;
  }
#وإلا
  (فراغ)ctx;
  (فراغ)cf;
  (فراغ)data;
#endif
  إرجاع CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(بنية curl_tls_ctx *ctx,
                                    بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    بنية ssl_peer *peer)
{
  بنية ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  إذا(!conn_config)
    إرجاع CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (فراغ)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#وإلاإذا defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#وإلاإذا defined(USE_WOLFSSL)
  (فراغ)data;
  إذا(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    إذا(!cert)
      result = CURLE_OUT_OF_MEMORY;
    وإلا إذا(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     عدم) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    وإلا إذا(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  إذا(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  إذا(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_مفتاح);
  إرجاع result;
}

bool Curl_vquic_tls_get_ssl_info(بنية curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 بنية curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (فراغ *)ctx->ossl.ssl_ctx : (فراغ *)ctx->ossl.ssl;
  إرجاع TRUE;
#وإلاإذا defined(USE_GNUTLS)
  (فراغ)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  إرجاع TRUE;
#وإلاإذا defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (فراغ *)ctx->wssl.ssl_ctx : (فراغ *)ctx->wssl.ssl;
  إرجاع TRUE;
#وإلا
  إرجاع FALSE;
#endif
}

فراغ Curl_vquic_report_handshake(بنية curl_tls_ctx *ctx,
                                 بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data)
{
  (فراغ)cf;
#ifdef USE_OPENSSL
  (فراغ)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#وإلاإذا defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#وإلاإذا defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#وإلا
  (فراغ)data;
  (فراغ)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
