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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <gnutls/abstract.h>
#ಅನ್ತರ್ಭಾವ <gnutls/gnutls.h>
#ಅನ್ತರ್ಭಾವ <gnutls/x509.h>
#ಅನ್ತರ್ಭಾವ <gnutls/crypto.h>
#ಅನ್ತರ್ಭಾವ <nettle/sha2.h>
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/ssl.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/quic.h>
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(ಸಂರಚನಾ Curl_peer *origin,
                                  ಸಂರಚನಾ Curl_peer *peer,
                                  ಸಂರಚನಾ ssl_filter_config *sslc,
                                  ಸಂರಚನಾ ssl_peer *ssl_peer)
{
  ವರ್ಣ tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, ಪರಿಮಾಣ(tls_id));
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, ಪರಿಮಾಣ(tls_id));
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, ಪರಿಮಾಣ(tls_id));
#ಅನ್ಯಥಾ
#error "no TLS lib in used, should not happen"
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
#endif
  ಯದಿ(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(ಸಂರಚನಾ curl_tls_ctx *ctx,
                             ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssl_peer *ssl_peer,
                             ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             ಶೂನ್ಯ *cb_user_data, ಶೂನ್ಯ *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  ಪ್ರತ್ಯಾಗಮ Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ರಿಕ್ತ, ssl_user_data,
                            session_reuse_cb);
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  ಪ್ರತ್ಯಾಗಮ Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  ಪ್ರತ್ಯಾಗಮ Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#ಅನ್ಯಥಾ
#error "no TLS lib in used, should not happen"
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
#endif
}

ಶೂನ್ಯ Curl_vquic_tls_cleanup(ಸಂರಚನಾ curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  ಯದಿ(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  ಯದಿ(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  ಯದಿ(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, ಪರಿಮಾಣ(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                    ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data)
{
#ifdef USE_OPENSSL
  ಯದಿ(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  ಯದಿ(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  ಯದಿ(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)ctx;
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                    ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssl_peer *peer)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಯದಿ(!conn_config)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (ಶೂನ್ಯ)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  (ಶೂನ್ಯ)data;
  ಯದಿ(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    ಯದಿ(!cert)
      result = CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ ಯದಿ(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     ರಿಕ್ತ) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    ಅನ್ಯಥಾ ಯದಿ(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  ಯದಿ(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  ಯದಿ(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_ಕೀಲಿ);
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_vquic_tls_get_ssl_info(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ಸಂರಚನಾ curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (ಶೂನ್ಯ *)ctx->ossl.ssl_ctx : (ಶೂನ್ಯ *)ctx->ossl.ssl;
  ಪ್ರತ್ಯಾಗಮ TRUE;
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  (ಶೂನ್ಯ)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  ಪ್ರತ್ಯಾಗಮ TRUE;
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (ಶೂನ್ಯ *)ctx->wssl.ssl_ctx : (ಶೂನ್ಯ *)ctx->wssl.ssl;
  ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

ಶೂನ್ಯ Curl_vquic_report_handshake(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                 ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  (ಶೂನ್ಯ)cf;
#ifdef USE_OPENSSL
  (ಶೂನ್ಯ)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
