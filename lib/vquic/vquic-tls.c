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

#اگر_تعریف_شده USE_OPENSSL
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
#پایان_اگر

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

#اگر_تعریف_شده USE_OPENSSL
  Curl_ossl_نسخه(tls_id, حجم(tls_id));
#وإلاإذا defined(USE_GNUTLS)
  Curl_gtls_نسخه(tls_id, حجم(tls_id));
#وإلاإذا defined(USE_WOLFSSL)
  Curl_wssl_نسخه(tls_id, حجم(tls_id));
#وإلا
#خطا "no TLS lib in used, should not happen"
  أرجع CURLE_FAILED_INIT;
#پایان_اگر
  إذا(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  أرجع Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(بنية curl_tls_ctx *ctx,
                             بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             بنية ssl_peer *ssl_peer,
                             ثابت بنية alpn_spec *alpns,
                             Curl_vquic_tls_ctx_راهاندازی *cb_راهاندازی,
                             فراغ *cb_user_داده, فراغ *ssl_user_داده,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#اگر_تعریف_شده USE_OPENSSL
  أرجع Curl_ossl_ctx_init(&ctx->ossl, cf, داده, ssl_peer, alpns,
                            cb_راهاندازی, cb_user_داده, عدم, ssl_user_داده,
                            session_reuse_cb);
#وإلاإذا defined(USE_GNUTLS)
  أرجع Curl_gtls_ctx_init(&ctx->gtls, cf, داده, ssl_peer, alpns,
                            cb_راهاندازی, cb_user_داده, ssl_user_داده,
                            session_reuse_cb);
#وإلاإذا defined(USE_WOLFSSL)
  أرجع Curl_wssl_ctx_init(&ctx->wssl, cf, داده, ssl_peer, alpns,
                            cb_راهاندازی, cb_user_داده,
                            ssl_user_داده, session_reuse_cb);
#وإلا
#خطا "no TLS lib in used, should not happen"
  أرجع CURLE_FAILED_INIT;
#پایان_اگر
}

فراغ Curl_vquic_tls_cleanup(بنية curl_tls_ctx *ctx)
{
#اگر_تعریف_شده USE_OPENSSL
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
#پایان_اگر
  memset(ctx, 0, حجم(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(بنية curl_tls_ctx *ctx,
                                    بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده)
{
#اگر_تعریف_شده USE_OPENSSL
  إذا(!ctx->ossl.x509_store_راهاندازی) {
    CURLcode نتیجه = Curl_ssl_راهاندازی_x509_store(cf, داده, &ctx->ossl);
    إذا(نتیجه)
      أرجع نتیجه;
    ctx->ossl.x509_store_راهاندازی = TRUE;
  }
#وإلاإذا defined(USE_WOLFSSL)
  إذا(!ctx->wssl.x509_store_راهاندازی) {
    CURLcode نتیجه = Curl_wssl_راهاندازی_x509_store(cf, داده, &ctx->wssl);
    إذا(نتیجه)
      أرجع نتیجه;
  }
#وإلاإذا defined(USE_GNUTLS)
  إذا(!ctx->gtls.shared_creds->trust_راهاندازی) {
    CURLcode نتیجه = Curl_gtls_client_trust_راهاندازی(cf, داده, &ctx->gtls);
    إذا(نتیجه)
      أرجع نتیجه;
  }
#وإلا
  (فراغ)ctx;
  (فراغ)cf;
  (فراغ)داده;
#پایان_اگر
  أرجع CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(بنية curl_tls_ctx *ctx,
                                    بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    بنية ssl_peer *peer)
{
  بنية ssl_filter_config *conn_config;
  CURLcode نتیجه = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  إذا(!conn_config)
    أرجع CURLE_FAILED_INIT;

#اگر_تعریف_شده USE_OPENSSL
  (فراغ)conn_config;
  نتیجه = Curl_ossl_check_peer_cert(cf, داده, &ctx->ossl, peer);
#وإلاإذا defined(USE_GNUTLS)
  نتیجه = Curl_gtls_verifyserver(
    cf, داده, ctx->gtls.session, conn_config, &داده->مجموعه.ssl, peer,
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY));
#وإلاإذا defined(USE_WOLFSSL)
  (فراغ)داده;
  إذا(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    إذا(!cert)
      نتیجه = CURLE_بیرون_از_حافظه;
    وإلا إذا(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, طول_رشته(peer->sni), 0,
                                     عدم) == WOLFSSL_FAILURE))
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    وإلا إذا(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  إذا(!نتیجه)
    نتیجه = Curl_wssl_verify_pinned(cf, داده, &ctx->wssl);
#پایان_اگر
  /* on error, remove any session we might have in the pool */
  إذا(نتیجه)
    Curl_ssl_scache_remove_all(cf, داده, peer->scache_کلید);
  أرجع نتیجه;
}

bool Curl_vquic_tls_get_ssl_info(بنية curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 بنية curl_tlssessioninfo *info)
{
#اگر_تعریف_شده USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (فراغ *)ctx->ossl.ssl_ctx : (فراغ *)ctx->ossl.ssl;
  أرجع TRUE;
#وإلاإذا defined(USE_GNUTLS)
  (فراغ)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  أرجع TRUE;
#وإلاإذا defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (فراغ *)ctx->wssl.ssl_ctx : (فراغ *)ctx->wssl.ssl;
  أرجع TRUE;
#وإلا
  أرجع FALSE;
#پایان_اگر
}

فراغ Curl_vquic_report_handshake(بنية curl_tls_ctx *ctx,
                                 بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده)
{
  (فراغ)cf;
#اگر_تعریف_شده USE_OPENSSL
  (فراغ)cf;
  Curl_ossl_report_handshake(داده, &ctx->ossl);
#وإلاإذا defined(USE_GNUTLS)
  Curl_gtls_report_handshake(داده, &ctx->gtls);
#وإلاإذا defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(داده, &ctx->wssl);
#وإلا
  (فراغ)داده;
  (فراغ)ctx;
#پایان_اگر
}

#پایان_اگر /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
