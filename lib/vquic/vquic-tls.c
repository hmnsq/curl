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

#ಆದರೆ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <gnutls/abstract.h>
#ಅನ್ತರ್ಭಾವ <gnutls/gnutls.h>
#ಅನ್ತರ್ಭಾವ <gnutls/x509.h>
#ಅನ್ತರ್ಭಾವ <gnutls/crypto.h>
#ಅನ್ತರ್ಭಾವ <nettle/sha2.h>
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/ssl.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/quic.h>
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(ರಚನೆ Curl_peer *origin,
                                  ರಚನೆ Curl_peer *peer,
                                  ರಚನೆ ssl_filter_config *sslc,
                                  ರಚನೆ ssl_peer *ssl_peer)
{
  ಅಕ್ಷರ tls_id[80];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  Curl_ossl_ಆವೃತ್ತಿ(tls_id, ಗಾತ್ರ(tls_id));
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  Curl_gtls_ಆವೃತ್ತಿ(tls_id, ಗಾತ್ರ(tls_id));
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  Curl_wssl_ಆವೃತ್ತಿ(tls_id, ಗಾತ್ರ(tls_id));
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "no TLS lib in used, should not happen"
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  ಹಿಂತಿರುಗಿ Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(ರಚನೆ curl_tls_ctx *ctx,
                             ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssl_peer *ssl_peer,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns,
                             Curl_vquic_tls_ctx_ಸಿದ್ಧತೆ *cb_ಸಿದ್ಧತೆ,
                             ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  ಹಿಂತಿರುಗಿ Curl_ossl_ctx_init(&ctx->ossl, cf, ದತ್ತಾಂಶ, ssl_peer, alpns,
                            cb_ಸಿದ್ಧತೆ, cb_user_ದತ್ತಾಂಶ, ಖಾಲಿ, ssl_user_ದತ್ತಾಂಶ,
                            session_reuse_cb);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  ಹಿಂತಿರುಗಿ Curl_gtls_ctx_init(&ctx->gtls, cf, ದತ್ತಾಂಶ, ssl_peer, alpns,
                            cb_ಸಿದ್ಧತೆ, cb_user_ದತ್ತಾಂಶ, ssl_user_ದತ್ತಾಂಶ,
                            session_reuse_cb);
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  ಹಿಂತಿರುಗಿ Curl_wssl_ctx_init(&ctx->wssl, cf, ದತ್ತಾಂಶ, ssl_peer, alpns,
                            cb_ಸಿದ್ಧತೆ, cb_user_ದತ್ತಾಂಶ,
                            ssl_user_ದತ್ತಾಂಶ, session_reuse_cb);
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "no TLS lib in used, should not happen"
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Curl_vquic_tls_cleanup(ರಚನೆ curl_tls_ctx *ctx)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  ಆದರೆ(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  ಆದರೆ(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  ಆದರೆ(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#ಆದರೆ_ಅಂತ್ಯ
  memset(ctx, 0, ಗಾತ್ರ(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(ರಚನೆ curl_tls_ctx *ctx,
                                    ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  ಆದರೆ(!ctx->ossl.x509_store_ಸಿದ್ಧತೆ) {
    CURLcode ಫಲಿತಾಂಶ = Curl_ssl_ಸಿದ್ಧತೆ_x509_store(cf, ದತ್ತಾಂಶ, &ctx->ossl);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ctx->ossl.x509_store_ಸಿದ್ಧತೆ = TRUE;
  }
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  ಆದರೆ(!ctx->wssl.x509_store_ಸಿದ್ಧತೆ) {
    CURLcode ಫಲಿತಾಂಶ = Curl_wssl_ಸಿದ್ಧತೆ_x509_store(cf, ದತ್ತಾಂಶ, &ctx->wssl);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  ಆದರೆ(!ctx->gtls.shared_creds->trust_ಸಿದ್ಧತೆ) {
    CURLcode ಫಲಿತಾಂಶ = Curl_gtls_client_trust_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, &ctx->gtls);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ctx;
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(ರಚನೆ curl_tls_ctx *ctx,
                                    ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssl_peer *peer)
{
  ರಚನೆ ssl_filter_config *conn_config;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಆದರೆ(!conn_config)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  (ಶೂನ್ಯ)conn_config;
  ಫಲಿತಾಂಶ = Curl_ossl_check_peer_cert(cf, ದತ್ತಾಂಶ, &ctx->ossl, peer);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  ಫಲಿತಾಂಶ = Curl_gtls_verifyserver(
    cf, ದತ್ತಾಂಶ, ctx->gtls.session, conn_config, &ದತ್ತಾಂಶ->ಗಣ.ssl, peer,
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY));
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    ಆದರೆ(!cert)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->sni), 0,
                                     ಖಾಲಿ) == WOLFSSL_FAILURE))
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_wssl_verify_pinned(cf, ದತ್ತಾಂಶ, &ctx->wssl);
#ಆದರೆ_ಅಂತ್ಯ
  /* on error, remove any session we might have in the pool */
  ಆದರೆ(ಫಲಿತಾಂಶ)
    Curl_ssl_scache_remove_all(cf, ದತ್ತಾಂಶ, peer->scache_ಕೀಲಿ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_vquic_tls_get_ssl_info(ರಚನೆ curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ರಚನೆ curl_tlssessioninfo *info)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (ಶೂನ್ಯ *)ctx->ossl.ssl_ctx : (ಶೂನ್ಯ *)ctx->ossl.ssl;
  ಹಿಂತಿರುಗಿ TRUE;
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  (ಶೂನ್ಯ)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  ಹಿಂತಿರುಗಿ TRUE;
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (ಶೂನ್ಯ *)ctx->wssl.ssl_ctx : (ಶೂನ್ಯ *)ctx->wssl.ssl;
  ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Curl_vquic_report_handshake(ರಚನೆ curl_tls_ctx *ctx,
                                 ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)cf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  (ಶೂನ್ಯ)cf;
  Curl_ossl_report_handshake(ದತ್ತಾಂಶ, &ctx->ossl);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  Curl_gtls_report_handshake(ದತ್ತಾಂಶ, &ctx->gtls);
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(ದತ್ತಾಂಶ, &ctx->wssl);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)ctx;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
