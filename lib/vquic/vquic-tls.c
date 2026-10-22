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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#ଅନ୍ତର୍ଭାଵ <openssl/err.h>
#ଅନ୍ତର୍ଭାଵ "vtls/openssl.h"
#ନଚେତଯଦି defined(USE_GNUTLS)
#ଅନ୍ତର୍ଭାଵ <gnutls/abstract.h>
#ଅନ୍ତର୍ଭାଵ <gnutls/gnutls.h>
#ଅନ୍ତର୍ଭାଵ <gnutls/x509.h>
#ଅନ୍ତର୍ଭାଵ <gnutls/crypto.h>
#ଅନ୍ତର୍ଭାଵ <nettle/sha2.h>
#ଅନ୍ତର୍ଭାଵ "vtls/gtls.h"
#ନଚେତଯଦି defined(USE_WOLFSSL)
#ଅନ୍ତର୍ଭାଵ <wolfssl/options.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/ssl.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/quic.h>
#ଅନ୍ତର୍ଭାଵ "vtls/wolfssl.h"
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(ସଂରଚନା Curl_peer *origin,
                                  ସଂରଚନା Curl_peer *peer,
                                  ସଂରଚନା ssl_filter_config *sslc,
                                  ସଂରଚନା ssl_peer *ssl_peer)
{
  ଅକ୍ଷର tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, ଆକାର(tls_id));
#ନଚେତଯଦି defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, ଆକାର(tls_id));
#ନଚେତଯଦି defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, ଆକାର(tls_id));
#ଅନ୍ୟଥା
#error "no TLS lib in used, should not happen"
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
#endif
  ଯଦି(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  ଫେରନ୍ତୁ Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(ସଂରଚନା curl_tls_ctx *ctx,
                             ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssl_peer *ssl_peer,
                             ଅଚଳ ସଂରଚନା alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             ଶୂନ୍ୟ *cb_user_data, ଶୂନ୍ୟ *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  ଫେରନ୍ତୁ Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ଖାଲି, ssl_user_data,
                            session_reuse_cb);
#ନଚେତଯଦି defined(USE_GNUTLS)
  ଫେରନ୍ତୁ Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#ନଚେତଯଦି defined(USE_WOLFSSL)
  ଫେରନ୍ତୁ Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#ଅନ୍ୟଥା
#error "no TLS lib in used, should not happen"
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
#endif
}

ଶୂନ୍ୟ Curl_vquic_tls_cleanup(ସଂରଚନା curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#ନଚେତଯଦି defined(USE_GNUTLS)
  ଯଦି(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#ନଚେତଯଦି defined(USE_WOLFSSL)
  ଯଦି(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  ଯଦି(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, ଆକାର(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(ସଂରଚନା curl_tls_ctx *ctx,
                                    ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data)
{
#ifdef USE_OPENSSL
  ଯଦି(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#ନଚେତଯଦି defined(USE_WOLFSSL)
  ଯଦି(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
#ନଚେତଯଦି defined(USE_GNUTLS)
  ଯଦି(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)ctx;
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(ସଂରଚନା curl_tls_ctx *ctx,
                                    ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssl_peer *peer)
{
  ସଂରଚନା ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  ଯଦି(!conn_config)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (ଶୂନ୍ୟ)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#ନଚେତଯଦି defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#ନଚେତଯଦି defined(USE_WOLFSSL)
  (ଶୂନ୍ୟ)data;
  ଯଦି(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    ଯଦି(!cert)
      result = CURLE_OUT_OF_MEMORY;
    ଅନ୍ୟଥା ଯଦି(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     ଖାଲି) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    ଅନ୍ୟଥା ଯଦି(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  ଯଦି(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  ଯଦି(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_ଚାବି);
  ଫେରନ୍ତୁ result;
}

bool Curl_vquic_tls_get_ssl_info(ସଂରଚନା curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ସଂରଚନା curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (ଶୂନ୍ୟ *)ctx->ossl.ssl_ctx : (ଶୂନ୍ୟ *)ctx->ossl.ssl;
  ଫେରନ୍ତୁ TRUE;
#ନଚେତଯଦି defined(USE_GNUTLS)
  (ଶୂନ୍ୟ)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  ଫେରନ୍ତୁ TRUE;
#ନଚେତଯଦି defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (ଶୂନ୍ୟ *)ctx->wssl.ssl_ctx : (ଶୂନ୍ୟ *)ctx->wssl.ssl;
  ଫେରନ୍ତୁ TRUE;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ FALSE;
#endif
}

ଶୂନ୍ୟ Curl_vquic_report_handshake(ସଂରଚନା curl_tls_ctx *ctx,
                                 ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data)
{
  (ଶୂନ୍ୟ)cf;
#ifdef USE_OPENSSL
  (ଶୂନ୍ୟ)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#ନଚେତଯଦି defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#ନଚେତଯଦି defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
