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
#অন্তর্ভাৱ "curl_setup.h"

#যদি defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#অন্তর্ভাৱ <openssl/err.h>
#অন্তর্ভাৱ "vtls/openssl.h"
#নোচেদ্যদি defined(USE_GNUTLS)
#অন্তর্ভাৱ <gnutls/abstract.h>
#অন্তর্ভাৱ <gnutls/gnutls.h>
#অন্তর্ভাৱ <gnutls/x509.h>
#অন্তর্ভাৱ <gnutls/crypto.h>
#অন্তর্ভাৱ <nettle/sha2.h>
#অন্তর্ভাৱ "vtls/gtls.h"
#নোচেদ্যদি defined(USE_WOLFSSL)
#অন্তর্ভাৱ <wolfssl/options.h>
#অন্তর্ভাৱ <wolfssl/ssl.h>
#অন্তর্ভাৱ <wolfssl/quic.h>
#অন্তর্ভাৱ "vtls/wolfssl.h"
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "vtls/vtls_scache.h"
#অন্তর্ভাৱ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(সংরচনা Curl_peer *origin,
                                  সংরচনা Curl_peer *peer,
                                  সংরচনা ssl_filter_config *sslc,
                                  সংরচনা ssl_peer *ssl_peer)
{
  ৱর্ণ tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, পরিমাণ(tls_id));
#নোচেদ্যদি defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, পরিমাণ(tls_id));
#নোচেদ্যদি defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, পরিমাণ(tls_id));
#অন্যথা
#error "no TLS lib in used, should not happen"
  ঘূৰাই CURLE_FAILED_INIT;
#endif
  যদি(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  ঘূৰাই Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(সংরচনা curl_tls_ctx *ctx,
                             সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             সংরচনা ssl_peer *ssl_peer,
                             ধ্রুৱ সংরচনা alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             শূন্য *cb_user_data, শূন্য *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  ঘূৰাই Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, রিক্ত, ssl_user_data,
                            session_reuse_cb);
#নোচেদ্যদি defined(USE_GNUTLS)
  ঘূৰাই Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#নোচেদ্যদি defined(USE_WOLFSSL)
  ঘূৰাই Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#অন্যথা
#error "no TLS lib in used, should not happen"
  ঘূৰাই CURLE_FAILED_INIT;
#endif
}

শূন্য Curl_vquic_tls_cleanup(সংরচনা curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#নোচেদ্যদি defined(USE_GNUTLS)
  যদি(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#নোচেদ্যদি defined(USE_WOLFSSL)
  যদি(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  যদি(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, পরিমাণ(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(সংরচনা curl_tls_ctx *ctx,
                                    সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data)
{
#ifdef USE_OPENSSL
  যদি(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    যদি(result)
      ঘূৰাই result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#নোচেদ্যদি defined(USE_WOLFSSL)
  যদি(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    যদি(result)
      ঘূৰাই result;
  }
#নোচেদ্যদি defined(USE_GNUTLS)
  যদি(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    যদি(result)
      ঘূৰাই result;
  }
#অন্যথা
  (শূন্য)ctx;
  (শূন্য)cf;
  (শূন্য)data;
#endif
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(সংরচনা curl_tls_ctx *ctx,
                                    সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা ssl_peer *peer)
{
  সংরচনা ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  যদি(!conn_config)
    ঘূৰাই CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (শূন্য)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#নোচেদ্যদি defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#নোচেদ্যদি defined(USE_WOLFSSL)
  (শূন্য)data;
  যদি(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    যদি(!cert)
      result = CURLE_OUT_OF_MEMORY;
    অন্যথা যদি(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     রিক্ত) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    অন্যথা যদি(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  যদি(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  যদি(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_চাবি);
  ঘূৰাই result;
}

bool Curl_vquic_tls_get_ssl_info(সংরচনা curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 সংরচনা curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (শূন্য *)ctx->ossl.ssl_ctx : (শূন্য *)ctx->ossl.ssl;
  ঘূৰাই TRUE;
#নোচেদ্যদি defined(USE_GNUTLS)
  (শূন্য)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  ঘূৰাই TRUE;
#নোচেদ্যদি defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (শূন্য *)ctx->wssl.ssl_ctx : (শূন্য *)ctx->wssl.ssl;
  ঘূৰাই TRUE;
#অন্যথা
  ঘূৰাই FALSE;
#endif
}

শূন্য Curl_vquic_report_handshake(সংরচনা curl_tls_ctx *ctx,
                                 সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data)
{
  (শূন্য)cf;
#ifdef USE_OPENSSL
  (শূন্য)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#নোচেদ্যদি defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#নোচেদ্যদি defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#অন্যথা
  (শূন্য)data;
  (শূন্য)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
