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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/err.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/openssl.h"
#ਜਾਂਜੇ defined(USE_GNUTLS)
#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/abstract.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/gnutls.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/x509.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/crypto.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/sha2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/gtls.h"
#ਜਾਂਜੇ defined(USE_WOLFSSL)
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/ssl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/quic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/wolfssl.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(ਬਣਤਰ Curl_peer *origin,
                                  ਬਣਤਰ Curl_peer *peer,
                                  ਬਣਤਰ ssl_filter_config *sslc,
                                  ਬਣਤਰ ssl_peer *ssl_peer)
{
  ਅੱਖਰ tls_id[80];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  Curl_ossl_ਸੰਸਕਰਣ(tls_id, ਆਕਾਰ(tls_id));
#ਜਾਂਜੇ defined(USE_GNUTLS)
  Curl_gtls_ਸੰਸਕਰਣ(tls_id, ਆਕਾਰ(tls_id));
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  Curl_wssl_ਸੰਸਕਰਣ(tls_id, ਆਕਾਰ(tls_id));
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "no TLS lib in used, should not happen"
  ਵਾਪਸ CURLE_FAILED_INIT;
#ਜੇ_ਅੰਤ
  ਜੇ(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  ਵਾਪਸ Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(ਬਣਤਰ curl_tls_ctx *ctx,
                             ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssl_peer *ssl_peer,
                             ਅਚਲ ਬਣਤਰ alpn_spec *alpns,
                             Curl_vquic_tls_ctx_ਤਿਆਰੀ *cb_ਤਿਆਰੀ,
                             ਖਾਲੀ *cb_user_ਅੰਕੜੇ, ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  ਵਾਪਸ Curl_ossl_ctx_init(&ctx->ossl, cf, ਅੰਕੜੇ, ssl_peer, alpns,
                            cb_ਤਿਆਰੀ, cb_user_ਅੰਕੜੇ, ਨਿਰਾ, ssl_user_ਅੰਕੜੇ,
                            session_reuse_cb);
#ਜਾਂਜੇ defined(USE_GNUTLS)
  ਵਾਪਸ Curl_gtls_ctx_init(&ctx->gtls, cf, ਅੰਕੜੇ, ssl_peer, alpns,
                            cb_ਤਿਆਰੀ, cb_user_ਅੰਕੜੇ, ssl_user_ਅੰਕੜੇ,
                            session_reuse_cb);
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  ਵਾਪਸ Curl_wssl_ctx_init(&ctx->wssl, cf, ਅੰਕੜੇ, ssl_peer, alpns,
                            cb_ਤਿਆਰੀ, cb_user_ਅੰਕੜੇ,
                            ssl_user_ਅੰਕੜੇ, session_reuse_cb);
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "no TLS lib in used, should not happen"
  ਵਾਪਸ CURLE_FAILED_INIT;
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Curl_vquic_tls_cleanup(ਬਣਤਰ curl_tls_ctx *ctx)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#ਜਾਂਜੇ defined(USE_GNUTLS)
  ਜੇ(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  ਜੇ(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  ਜੇ(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#ਜੇ_ਅੰਤ
  memset(ctx, 0, ਆਕਾਰ(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(ਬਣਤਰ curl_tls_ctx *ctx,
                                    ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  ਜੇ(!ctx->ossl.x509_store_ਤਿਆਰੀ) {
    CURLcode ਨਤੀਜਾ = Curl_ssl_ਤਿਆਰੀ_x509_store(cf, ਅੰਕੜੇ, &ctx->ossl);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ctx->ossl.x509_store_ਤਿਆਰੀ = TRUE;
  }
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  ਜੇ(!ctx->wssl.x509_store_ਤਿਆਰੀ) {
    CURLcode ਨਤੀਜਾ = Curl_wssl_ਤਿਆਰੀ_x509_store(cf, ਅੰਕੜੇ, &ctx->wssl);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜਾਂਜੇ defined(USE_GNUTLS)
  ਜੇ(!ctx->gtls.shared_creds->trust_ਤਿਆਰੀ) {
    CURLcode ਨਤੀਜਾ = Curl_gtls_client_trust_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, &ctx->gtls);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ctx;
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(ਬਣਤਰ curl_tls_ctx *ctx,
                                    ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssl_peer *peer)
{
  ਬਣਤਰ ssl_filter_config *conn_config;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਜੇ(!conn_config)
    ਵਾਪਸ CURLE_FAILED_INIT;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  (ਖਾਲੀ)conn_config;
  ਨਤੀਜਾ = Curl_ossl_check_peer_cert(cf, ਅੰਕੜੇ, &ctx->ossl, peer);
#ਜਾਂਜੇ defined(USE_GNUTLS)
  ਨਤੀਜਾ = Curl_gtls_verifyserver(
    cf, ਅੰਕੜੇ, ctx->gtls.session, conn_config, &ਅੰਕੜੇ->ਸੈੱਟ.ssl, peer,
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY));
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    ਜੇ(!cert)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਨਹੀਂਤਾਂ ਜੇ(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, ਲੜੀ_ਲੰਬਾਈ(peer->sni), 0,
                                     ਨਿਰਾ) == WOLFSSL_FAILURE))
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    ਨਹੀਂਤਾਂ ਜੇ(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_wssl_verify_pinned(cf, ਅੰਕੜੇ, &ctx->wssl);
#ਜੇ_ਅੰਤ
  /* on error, remove any session we might have in the pool */
  ਜੇ(ਨਤੀਜਾ)
    Curl_ssl_scache_remove_all(cf, ਅੰਕੜੇ, peer->scache_ਕੁੰਜੀ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_vquic_tls_get_ssl_info(ਬਣਤਰ curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ਬਣਤਰ curl_tlssessioninfo *info)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (ਖਾਲੀ *)ctx->ossl.ssl_ctx : (ਖਾਲੀ *)ctx->ossl.ssl;
  ਵਾਪਸ TRUE;
#ਜਾਂਜੇ defined(USE_GNUTLS)
  (ਖਾਲੀ)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  ਵਾਪਸ TRUE;
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (ਖਾਲੀ *)ctx->wssl.ssl_ctx : (ਖਾਲੀ *)ctx->wssl.ssl;
  ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Curl_vquic_report_handshake(ਬਣਤਰ curl_tls_ctx *ctx,
                                 ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)cf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  (ਖਾਲੀ)cf;
  Curl_ossl_report_handshake(ਅੰਕੜੇ, &ctx->ossl);
#ਜਾਂਜੇ defined(USE_GNUTLS)
  Curl_gtls_report_handshake(ਅੰਕੜੇ, &ctx->gtls);
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(ਅੰਕੜੇ, &ctx->wssl);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)ctx;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
