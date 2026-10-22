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

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/err.h>
#شامل "vtls/openssl.h"
#وگرنہ_اگر defined(USE_GNUTLS)
#شامل <gnutls/abstract.h>
#شامل <gnutls/gnutls.h>
#شامل <gnutls/x509.h>
#شامل <gnutls/crypto.h>
#شامل <nettle/sha2.h>
#شامل "vtls/gtls.h"
#وگرنہ_اگر defined(USE_WOLFSSL)
#شامل <wolfssl/options.h>
#شامل <wolfssl/ssl.h>
#شامل <wolfssl/quic.h>
#شامل "vtls/wolfssl.h"
#اگر_ختم

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_scache.h"
#شامل "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(ساخت Curl_peer *origin,
                                  ساخت Curl_peer *peer,
                                  ساخت ssl_filter_config *sslc,
                                  ساخت ssl_peer *ssl_peer)
{
  حرف tls_id[80];

#اگر_تعریف_شدہ USE_OPENSSL
  Curl_ossl_نسخہ(tls_id, حجم(tls_id));
#وگرنہ_اگر defined(USE_GNUTLS)
  Curl_gtls_نسخہ(tls_id, حجم(tls_id));
#وگرنہ_اگر defined(USE_WOLFSSL)
  Curl_wssl_نسخہ(tls_id, حجم(tls_id));
#ورنہ
#غلطی "no TLS lib in used, should not happen"
  واپس CURLE_FAILED_INIT;
#اگر_ختم
  اگر(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  واپس Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(ساخت curl_tls_ctx *ctx,
                             ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت ssl_peer *ssl_peer,
                             ثابت ساخت alpn_spec *alpns,
                             Curl_vquic_tls_ctx_تیاری *cb_تیاری,
                             خالی *cb_user_ڈیٹا, خالی *ssl_user_ڈیٹا,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#اگر_تعریف_شدہ USE_OPENSSL
  واپس Curl_ossl_ctx_init(&ctx->ossl, cf, ڈیٹا, ssl_peer, alpns,
                            cb_تیاری, cb_user_ڈیٹا, تہی, ssl_user_ڈیٹا,
                            session_reuse_cb);
#وگرنہ_اگر defined(USE_GNUTLS)
  واپس Curl_gtls_ctx_init(&ctx->gtls, cf, ڈیٹا, ssl_peer, alpns,
                            cb_تیاری, cb_user_ڈیٹا, ssl_user_ڈیٹا,
                            session_reuse_cb);
#وگرنہ_اگر defined(USE_WOLFSSL)
  واپس Curl_wssl_ctx_init(&ctx->wssl, cf, ڈیٹا, ssl_peer, alpns,
                            cb_تیاری, cb_user_ڈیٹا,
                            ssl_user_ڈیٹا, session_reuse_cb);
#ورنہ
#غلطی "no TLS lib in used, should not happen"
  واپس CURLE_FAILED_INIT;
#اگر_ختم
}

خالی Curl_vquic_tls_cleanup(ساخت curl_tls_ctx *ctx)
{
#اگر_تعریف_شدہ USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#وگرنہ_اگر defined(USE_GNUTLS)
  اگر(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#وگرنہ_اگر defined(USE_WOLFSSL)
  اگر(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  اگر(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#اگر_ختم
  memset(ctx, 0, حجم(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(ساخت curl_tls_ctx *ctx,
                                    ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا)
{
#اگر_تعریف_شدہ USE_OPENSSL
  اگر(!ctx->ossl.x509_store_تیاری) {
    CURLcode نتیجہ = Curl_ssl_تیاری_x509_store(cf, ڈیٹا, &ctx->ossl);
    اگر(نتیجہ)
      واپس نتیجہ;
    ctx->ossl.x509_store_تیاری = TRUE;
  }
#وگرنہ_اگر defined(USE_WOLFSSL)
  اگر(!ctx->wssl.x509_store_تیاری) {
    CURLcode نتیجہ = Curl_wssl_تیاری_x509_store(cf, ڈیٹا, &ctx->wssl);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
#وگرنہ_اگر defined(USE_GNUTLS)
  اگر(!ctx->gtls.shared_creds->trust_تیاری) {
    CURLcode نتیجہ = Curl_gtls_client_trust_تیاری(cf, ڈیٹا, &ctx->gtls);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
#ورنہ
  (خالی)ctx;
  (خالی)cf;
  (خالی)ڈیٹا;
#اگر_ختم
  واپس CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(ساخت curl_tls_ctx *ctx,
                                    ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت ssl_peer *peer)
{
  ساخت ssl_filter_config *conn_config;
  CURLcode نتیجہ = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  اگر(!conn_config)
    واپس CURLE_FAILED_INIT;

#اگر_تعریف_شدہ USE_OPENSSL
  (خالی)conn_config;
  نتیجہ = Curl_ossl_check_peer_cert(cf, ڈیٹا, &ctx->ossl, peer);
#وگرنہ_اگر defined(USE_GNUTLS)
  نتیجہ = Curl_gtls_verifyserver(
    cf, ڈیٹا, ctx->gtls.session, conn_config, &ڈیٹا->سیٹ.ssl, peer,
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY));
#وگرنہ_اگر defined(USE_WOLFSSL)
  (خالی)ڈیٹا;
  اگر(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    اگر(!cert)
      نتیجہ = CURLE_برآمد_کا_حافظہ;
    ورنہ اگر(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, لڑی_لمبائی(peer->sni), 0,
                                     تہی) == WOLFSSL_FAILURE))
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    ورنہ اگر(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  اگر(!نتیجہ)
    نتیجہ = Curl_wssl_verify_pinned(cf, ڈیٹا, &ctx->wssl);
#اگر_ختم
  /* on error, remove any session we might have in the pool */
  اگر(نتیجہ)
    Curl_ssl_scache_remove_all(cf, ڈیٹا, peer->scache_کلید);
  واپس نتیجہ;
}

bool Curl_vquic_tls_get_ssl_info(ساخت curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ساخت curl_tlssessioninfo *info)
{
#اگر_تعریف_شدہ USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (خالی *)ctx->ossl.ssl_ctx : (خالی *)ctx->ossl.ssl;
  واپس TRUE;
#وگرنہ_اگر defined(USE_GNUTLS)
  (خالی)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  واپس TRUE;
#وگرنہ_اگر defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (خالی *)ctx->wssl.ssl_ctx : (خالی *)ctx->wssl.ssl;
  واپس TRUE;
#ورنہ
  واپس FALSE;
#اگر_ختم
}

خالی Curl_vquic_report_handshake(ساخت curl_tls_ctx *ctx,
                                 ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا)
{
  (خالی)cf;
#اگر_تعریف_شدہ USE_OPENSSL
  (خالی)cf;
  Curl_ossl_report_handshake(ڈیٹا, &ctx->ossl);
#وگرنہ_اگر defined(USE_GNUTLS)
  Curl_gtls_report_handshake(ڈیٹا, &ctx->gtls);
#وگرنہ_اگر defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(ڈیٹا, &ctx->wssl);
#ورنہ
  (خالی)ڈیٹا;
  (خالی)ctx;
#اگر_ختم
}

#اگر_ختم /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
