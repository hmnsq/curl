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
#அந்தர்பாவ "curl_setup.h"

#எனில் defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#அந்தர்பாவ <openssl/err.h>
#அந்தர்பாவ "vtls/openssl.h"
#அல்லதெனில் defined(USE_GNUTLS)
#அந்தர்பாவ <gnutls/abstract.h>
#அந்தர்பாவ <gnutls/gnutls.h>
#அந்தர்பாவ <gnutls/x509.h>
#அந்தர்பாவ <gnutls/crypto.h>
#அந்தர்பாவ <nettle/sha2.h>
#அந்தர்பாவ "vtls/gtls.h"
#அல்லதெனில் defined(USE_WOLFSSL)
#அந்தர்பாவ <wolfssl/options.h>
#அந்தர்பாவ <wolfssl/ssl.h>
#அந்தர்பாவ <wolfssl/quic.h>
#அந்தர்பாவ "vtls/wolfssl.h"
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(கட்டமைப்பு Curl_peer *origin,
                                  கட்டமைப்பு Curl_peer *peer,
                                  கட்டமைப்பு ssl_filter_config *sslc,
                                  கட்டமைப்பு ssl_peer *ssl_peer)
{
  எழுத்து tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, அளவு(tls_id));
#அல்லதெனில் defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, அளவு(tls_id));
#அல்லதெனில் defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, அளவு(tls_id));
#இல்லையெனில்
#error "no TLS lib in used, should not happen"
  திரும்பு CURLE_FAILED_INIT;
#endif
  எனில்(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  திரும்பு Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(கட்டமைப்பு curl_tls_ctx *ctx,
                             கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssl_peer *ssl_peer,
                             மாறிலி கட்டமைப்பு alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             வெறுமை *cb_user_data, வெறுமை *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  திரும்பு Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, வெற்று, ssl_user_data,
                            session_reuse_cb);
#அல்லதெனில் defined(USE_GNUTLS)
  திரும்பு Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#அல்லதெனில் defined(USE_WOLFSSL)
  திரும்பு Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#இல்லையெனில்
#error "no TLS lib in used, should not happen"
  திரும்பு CURLE_FAILED_INIT;
#endif
}

வெறுமை Curl_vquic_tls_cleanup(கட்டமைப்பு curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#அல்லதெனில் defined(USE_GNUTLS)
  எனில்(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#அல்லதெனில் defined(USE_WOLFSSL)
  எனில்(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  எனில்(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, அளவு(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(கட்டமைப்பு curl_tls_ctx *ctx,
                                    கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data)
{
#ifdef USE_OPENSSL
  எனில்(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    எனில்(result)
      திரும்பு result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#அல்லதெனில் defined(USE_WOLFSSL)
  எனில்(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    எனில்(result)
      திரும்பு result;
  }
#அல்லதெனில் defined(USE_GNUTLS)
  எனில்(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    எனில்(result)
      திரும்பு result;
  }
#இல்லையெனில்
  (வெறுமை)ctx;
  (வெறுமை)cf;
  (வெறுமை)data;
#endif
  திரும்பு CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(கட்டமைப்பு curl_tls_ctx *ctx,
                                    கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssl_peer *peer)
{
  கட்டமைப்பு ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  எனில்(!conn_config)
    திரும்பு CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (வெறுமை)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#அல்லதெனில் defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#அல்லதெனில் defined(USE_WOLFSSL)
  (வெறுமை)data;
  எனில்(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    எனில்(!cert)
      result = CURLE_OUT_OF_MEMORY;
    இல்லையெனில் எனில்(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     வெற்று) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    இல்லையெனில் எனில்(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  எனில்(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  எனில்(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_சாவி);
  திரும்பு result;
}

bool Curl_vquic_tls_get_ssl_info(கட்டமைப்பு curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 கட்டமைப்பு curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (வெறுமை *)ctx->ossl.ssl_ctx : (வெறுமை *)ctx->ossl.ssl;
  திரும்பு TRUE;
#அல்லதெனில் defined(USE_GNUTLS)
  (வெறுமை)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  திரும்பு TRUE;
#அல்லதெனில் defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (வெறுமை *)ctx->wssl.ssl_ctx : (வெறுமை *)ctx->wssl.ssl;
  திரும்பு TRUE;
#இல்லையெனில்
  திரும்பு FALSE;
#endif
}

வெறுமை Curl_vquic_report_handshake(கட்டமைப்பு curl_tls_ctx *ctx,
                                 கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data)
{
  (வெறுமை)cf;
#ifdef USE_OPENSSL
  (வெறுமை)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#அல்லதெனில் defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#அல்லதெனில் defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
