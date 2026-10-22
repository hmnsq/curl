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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#অন্তর্ভাব <openssl/err.h>
#অন্তর্ভাব "vtls/openssl.h"
#নতুবা_যদি defined(USE_GNUTLS)
#অন্তর্ভাব <gnutls/abstract.h>
#অন্তর্ভাব <gnutls/gnutls.h>
#অন্তর্ভাব <gnutls/x509.h>
#অন্তর্ভাব <gnutls/crypto.h>
#অন্তর্ভাব <nettle/sha2.h>
#অন্তর্ভাব "vtls/gtls.h"
#নতুবা_যদি defined(USE_WOLFSSL)
#অন্তর্ভাব <wolfssl/options.h>
#অন্তর্ভাব <wolfssl/ssl.h>
#অন্তর্ভাব <wolfssl/quic.h>
#অন্তর্ভাব "vtls/wolfssl.h"
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(কাঠামো Curl_peer *origin,
                                  কাঠামো Curl_peer *peer,
                                  কাঠামো ssl_filter_config *sslc,
                                  কাঠামো ssl_peer *ssl_peer)
{
  অক্ষর tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, আকার(tls_id));
#নতুবা_যদি defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, আকার(tls_id));
#নতুবা_যদি defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, আকার(tls_id));
#অন্যথা
#error "no TLS lib in used, should not happen"
  ফেরত CURLE_FAILED_INIT;
#endif
  যদি(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  ফেরত Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(কাঠামো curl_tls_ctx *ctx,
                             কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো ssl_peer *ssl_peer,
                             ধ্রুবক কাঠামো alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             শূন্য *cb_user_তথ্য, শূন্য *ssl_user_তথ্য,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  ফেরত Curl_ossl_ctx_init(&ctx->ossl, cf, তথ্য, ssl_peer, alpns,
                            cb_setup, cb_user_তথ্য, ফাঁকা, ssl_user_তথ্য,
                            session_reuse_cb);
#নতুবা_যদি defined(USE_GNUTLS)
  ফেরত Curl_gtls_ctx_init(&ctx->gtls, cf, তথ্য, ssl_peer, alpns,
                            cb_setup, cb_user_তথ্য, ssl_user_তথ্য,
                            session_reuse_cb);
#নতুবা_যদি defined(USE_WOLFSSL)
  ফেরত Curl_wssl_ctx_init(&ctx->wssl, cf, তথ্য, ssl_peer, alpns,
                            cb_setup, cb_user_তথ্য,
                            ssl_user_তথ্য, session_reuse_cb);
#অন্যথা
#error "no TLS lib in used, should not happen"
  ফেরত CURLE_FAILED_INIT;
#endif
}

শূন্য Curl_vquic_tls_cleanup(কাঠামো curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#নতুবা_যদি defined(USE_GNUTLS)
  যদি(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#নতুবা_যদি defined(USE_WOLFSSL)
  যদি(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  যদি(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, আকার(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(কাঠামো curl_tls_ctx *ctx,
                                    কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য)
{
#ifdef USE_OPENSSL
  যদি(!ctx->ossl.x509_store_setup) {
    CURLcode ফলাফল = Curl_ssl_setup_x509_store(cf, তথ্য, &ctx->ossl);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ctx->ossl.x509_store_setup = TRUE;
  }
#নতুবা_যদি defined(USE_WOLFSSL)
  যদি(!ctx->wssl.x509_store_setup) {
    CURLcode ফলাফল = Curl_wssl_setup_x509_store(cf, তথ্য, &ctx->wssl);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
#নতুবা_যদি defined(USE_GNUTLS)
  যদি(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode ফলাফল = Curl_gtls_client_trust_setup(cf, তথ্য, &ctx->gtls);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
#অন্যথা
  (শূন্য)ctx;
  (শূন্য)cf;
  (শূন্য)তথ্য;
#endif
  ফেরত CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(কাঠামো curl_tls_ctx *ctx,
                                    কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssl_peer *peer)
{
  কাঠামো ssl_filter_config *conn_config;
  CURLcode ফলাফল = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  যদি(!conn_config)
    ফেরত CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (শূন্য)conn_config;
  ফলাফল = Curl_ossl_check_peer_cert(cf, তথ্য, &ctx->ossl, peer);
#নতুবা_যদি defined(USE_GNUTLS)
  ফলাফল = Curl_gtls_verifyserver(
    cf, তথ্য, ctx->gtls.session, conn_config, &তথ্য->set.ssl, peer,
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY));
#নতুবা_যদি defined(USE_WOLFSSL)
  (শূন্য)তথ্য;
  যদি(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    যদি(!cert)
      ফলাফল = CURLE_OUT_OF_MEMORY;
    অন্যথা যদি(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     ফাঁকা) == WOLFSSL_FAILURE))
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    অন্যথা যদি(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  যদি(!ফলাফল)
    ফলাফল = Curl_wssl_verify_pinned(cf, তথ্য, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  যদি(ফলাফল)
    Curl_ssl_scache_remove_all(cf, তথ্য, peer->scache_চাবি);
  ফেরত ফলাফল;
}

bool Curl_vquic_tls_get_ssl_info(কাঠামো curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 কাঠামো curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (শূন্য *)ctx->ossl.ssl_ctx : (শূন্য *)ctx->ossl.ssl;
  ফেরত TRUE;
#নতুবা_যদি defined(USE_GNUTLS)
  (শূন্য)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  ফেরত TRUE;
#নতুবা_যদি defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (শূন্য *)ctx->wssl.ssl_ctx : (শূন্য *)ctx->wssl.ssl;
  ফেরত TRUE;
#অন্যথা
  ফেরত FALSE;
#endif
}

শূন্য Curl_vquic_report_handshake(কাঠামো curl_tls_ctx *ctx,
                                 কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য)
{
  (শূন্য)cf;
#ifdef USE_OPENSSL
  (শূন্য)cf;
  Curl_ossl_report_handshake(তথ্য, &ctx->ossl);
#নতুবা_যদি defined(USE_GNUTLS)
  Curl_gtls_report_handshake(তথ্য, &ctx->gtls);
#নতুবা_যদি defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(তথ্য, &ctx->wssl);
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
