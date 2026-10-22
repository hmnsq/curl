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

#اگر defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#شامل <openssl/err.h>
#شامل "vtls/openssl.h"
#وگرنهاگر defined(USE_GNUTLS)
#شامل <gnutls/abstract.h>
#شامل <gnutls/gnutls.h>
#شامل <gnutls/x509.h>
#شامل <gnutls/crypto.h>
#شامل <nettle/sha2.h>
#شامل "vtls/gtls.h"
#وگرنهاگر defined(USE_WOLFSSL)
#شامل <wolfssl/options.h>
#شامل <wolfssl/ssl.h>
#شامل <wolfssl/quic.h>
#شامل "vtls/wolfssl.h"
#endif

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_scache.h"
#شامل "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(ساختار Curl_peer *origin,
                                  ساختار Curl_peer *peer,
                                  ساختار ssl_filter_config *sslc,
                                  ساختار ssl_peer *ssl_peer)
{
  نویسه tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, اندازه(tls_id));
#وگرنهاگر defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, اندازه(tls_id));
#وگرنهاگر defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, اندازه(tls_id));
#وگرنه
#error "no TLS lib in used, should not happen"
  بازگشت CURLE_FAILED_INIT;
#endif
  اگر(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  بازگشت Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(ساختار curl_tls_ctx *ctx,
                             ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             ساختار ssl_peer *ssl_peer,
                             ثابت ساختار alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             تهی *cb_user_data, تهی *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  بازگشت Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, پوچ, ssl_user_data,
                            session_reuse_cb);
#وگرنهاگر defined(USE_GNUTLS)
  بازگشت Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#وگرنهاگر defined(USE_WOLFSSL)
  بازگشت Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#وگرنه
#error "no TLS lib in used, should not happen"
  بازگشت CURLE_FAILED_INIT;
#endif
}

تهی Curl_vquic_tls_cleanup(ساختار curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#وگرنهاگر defined(USE_GNUTLS)
  اگر(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#وگرنهاگر defined(USE_WOLFSSL)
  اگر(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  اگر(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, اندازه(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(ساختار curl_tls_ctx *ctx,
                                    ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data)
{
#ifdef USE_OPENSSL
  اگر(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    اگر(result)
      بازگشت result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#وگرنهاگر defined(USE_WOLFSSL)
  اگر(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    اگر(result)
      بازگشت result;
  }
#وگرنهاگر defined(USE_GNUTLS)
  اگر(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    اگر(result)
      بازگشت result;
  }
#وگرنه
  (تهی)ctx;
  (تهی)cf;
  (تهی)data;
#endif
  بازگشت CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(ساختار curl_tls_ctx *ctx,
                                    ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار ssl_peer *peer)
{
  ساختار ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  اگر(!conn_config)
    بازگشت CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (تهی)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#وگرنهاگر defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#وگرنهاگر defined(USE_WOLFSSL)
  (تهی)data;
  اگر(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    اگر(!cert)
      result = CURLE_OUT_OF_MEMORY;
    وگرنه اگر(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     پوچ) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    وگرنه اگر(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  اگر(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  اگر(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_کلید);
  بازگشت result;
}

bool Curl_vquic_tls_get_ssl_info(ساختار curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ساختار curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (تهی *)ctx->ossl.ssl_ctx : (تهی *)ctx->ossl.ssl;
  بازگشت TRUE;
#وگرنهاگر defined(USE_GNUTLS)
  (تهی)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  بازگشت TRUE;
#وگرنهاگر defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (تهی *)ctx->wssl.ssl_ctx : (تهی *)ctx->wssl.ssl;
  بازگشت TRUE;
#وگرنه
  بازگشت FALSE;
#endif
}

تهی Curl_vquic_report_handshake(ساختار curl_tls_ctx *ctx,
                                 ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data)
{
  (تهی)cf;
#ifdef USE_OPENSSL
  (تهی)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#وگرنهاگر defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#وگرنهاگر defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#وگرنه
  (تهی)data;
  (تهی)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
