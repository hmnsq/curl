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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#నిర్వచితమైతే USE_OPENSSL
#అన్తర్భావ <openssl/err.h>
#అన్తర్భావ "vtls/openssl.h"
#లేదావేళ defined(USE_GNUTLS)
#అన్తర్భావ <gnutls/abstract.h>
#అన్తర్భావ <gnutls/gnutls.h>
#అన్తర్భావ <gnutls/x509.h>
#అన్తర్భావ <gnutls/crypto.h>
#అన్తర్భావ <nettle/sha2.h>
#అన్తర్భావ "vtls/gtls.h"
#లేదావేళ defined(USE_WOLFSSL)
#అన్తర్భావ <wolfssl/options.h>
#అన్తర్భావ <wolfssl/ssl.h>
#అన్తర్భావ <wolfssl/quic.h>
#అన్తర్భావ "vtls/wolfssl.h"
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(నిర్మాణం Curl_peer *origin,
                                  నిర్మాణం Curl_peer *peer,
                                  నిర్మాణం ssl_filter_config *sslc,
                                  నిర్మాణం ssl_peer *ssl_peer)
{
  అక్షరం tls_id[80];

#నిర్వచితమైతే USE_OPENSSL
  Curl_ossl_సంస్కరణ(tls_id, పరిమాణం(tls_id));
#లేదావేళ defined(USE_GNUTLS)
  Curl_gtls_సంస్కరణ(tls_id, పరిమాణం(tls_id));
#లేదావేళ defined(USE_WOLFSSL)
  Curl_wssl_సంస్కరణ(tls_id, పరిమాణం(tls_id));
#లేకపోతే
#దోషం "no TLS lib in used, should not happen"
  తిరిగి CURLE_FAILED_INIT;
#అయితే_ముగింపు
  ఒకవేళ(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  తిరిగి Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(నిర్మాణం curl_tls_ctx *ctx,
                             నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssl_peer *ssl_peer,
                             స్థిరరాశి నిర్మాణం alpn_spec *alpns,
                             Curl_vquic_tls_ctx_అమరిక *cb_అమరిక,
                             శూన్య *cb_user_దత్తాంశం, శూన్య *ssl_user_దత్తాంశం,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#నిర్వచితమైతే USE_OPENSSL
  తిరిగి Curl_ossl_ctx_init(&ctx->ossl, cf, దత్తాంశం, ssl_peer, alpns,
                            cb_అమరిక, cb_user_దత్తాంశం, ఖాళీ, ssl_user_దత్తాంశం,
                            session_reuse_cb);
#లేదావేళ defined(USE_GNUTLS)
  తిరిగి Curl_gtls_ctx_init(&ctx->gtls, cf, దత్తాంశం, ssl_peer, alpns,
                            cb_అమరిక, cb_user_దత్తాంశం, ssl_user_దత్తాంశం,
                            session_reuse_cb);
#లేదావేళ defined(USE_WOLFSSL)
  తిరిగి Curl_wssl_ctx_init(&ctx->wssl, cf, దత్తాంశం, ssl_peer, alpns,
                            cb_అమరిక, cb_user_దత్తాంశం,
                            ssl_user_దత్తాంశం, session_reuse_cb);
#లేకపోతే
#దోషం "no TLS lib in used, should not happen"
  తిరిగి CURLE_FAILED_INIT;
#అయితే_ముగింపు
}

శూన్య Curl_vquic_tls_cleanup(నిర్మాణం curl_tls_ctx *ctx)
{
#నిర్వచితమైతే USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#లేదావేళ defined(USE_GNUTLS)
  ఒకవేళ(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#లేదావేళ defined(USE_WOLFSSL)
  ఒకవేళ(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  ఒకవేళ(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#అయితే_ముగింపు
  memset(ctx, 0, పరిమాణం(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(నిర్మాణం curl_tls_ctx *ctx,
                                    నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
#నిర్వచితమైతే USE_OPENSSL
  ఒకవేళ(!ctx->ossl.x509_store_అమరిక) {
    CURLcode ఫలితం = Curl_ssl_అమరిక_x509_store(cf, దత్తాంశం, &ctx->ossl);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ctx->ossl.x509_store_అమరిక = TRUE;
  }
#లేదావేళ defined(USE_WOLFSSL)
  ఒకవేళ(!ctx->wssl.x509_store_అమరిక) {
    CURLcode ఫలితం = Curl_wssl_అమరిక_x509_store(cf, దత్తాంశం, &ctx->wssl);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
#లేదావేళ defined(USE_GNUTLS)
  ఒకవేళ(!ctx->gtls.shared_creds->trust_అమరిక) {
    CURLcode ఫలితం = Curl_gtls_client_trust_అమరిక(cf, దత్తాంశం, &ctx->gtls);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
#లేకపోతే
  (శూన్య)ctx;
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(నిర్మాణం curl_tls_ctx *ctx,
                                    నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssl_peer *peer)
{
  నిర్మాణం ssl_filter_config *conn_config;
  CURLcode ఫలితం = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  ఒకవేళ(!conn_config)
    తిరిగి CURLE_FAILED_INIT;

#నిర్వచితమైతే USE_OPENSSL
  (శూన్య)conn_config;
  ఫలితం = Curl_ossl_check_peer_cert(cf, దత్తాంశం, &ctx->ossl, peer);
#లేదావేళ defined(USE_GNUTLS)
  ఫలితం = Curl_gtls_verifyserver(
    cf, దత్తాంశం, ctx->gtls.session, conn_config, &దత్తాంశం->సమితి.ssl, peer,
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY));
#లేదావేళ defined(USE_WOLFSSL)
  (శూన్య)దత్తాంశం;
  ఒకవేళ(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    ఒకవేళ(!cert)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    లేకపోతే ఒకవేళ(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, అక్షరశ్రేణి_పొడవు(peer->sni), 0,
                                     ఖాళీ) == WOLFSSL_FAILURE))
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    లేకపోతే ఒకవేళ(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_wssl_verify_pinned(cf, దత్తాంశం, &ctx->wssl);
#అయితే_ముగింపు
  /* on error, remove any session we might have in the pool */
  ఒకవేళ(ఫలితం)
    Curl_ssl_scache_remove_all(cf, దత్తాంశం, peer->scache_కీలకం);
  తిరిగి ఫలితం;
}

bool Curl_vquic_tls_get_ssl_info(నిర్మాణం curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 నిర్మాణం curl_tlssessioninfo *info)
{
#నిర్వచితమైతే USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (శూన్య *)ctx->ossl.ssl_ctx : (శూన్య *)ctx->ossl.ssl;
  తిరిగి TRUE;
#లేదావేళ defined(USE_GNUTLS)
  (శూన్య)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  తిరిగి TRUE;
#లేదావేళ defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (శూన్య *)ctx->wssl.ssl_ctx : (శూన్య *)ctx->wssl.ssl;
  తిరిగి TRUE;
#లేకపోతే
  తిరిగి FALSE;
#అయితే_ముగింపు
}

శూన్య Curl_vquic_report_handshake(నిర్మాణం curl_tls_ctx *ctx,
                                 నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  (శూన్య)cf;
#నిర్వచితమైతే USE_OPENSSL
  (శూన్య)cf;
  Curl_ossl_report_handshake(దత్తాంశం, &ctx->ossl);
#లేదావేళ defined(USE_GNUTLS)
  Curl_gtls_report_handshake(దత్తాంశం, &ctx->gtls);
#లేదావేళ defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(దత్తాంశం, &ctx->wssl);
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)ctx;
#అయితే_ముగింపు
}

#అయితే_ముగింపు /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
