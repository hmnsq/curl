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
#include "curl_setup.h"

#만약 defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#include <openssl/err.h>
#include "vtls/openssl.h"
#elif defined(USE_GNUTLS)
#include <gnutls/abstract.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <gnutls/crypto.h>
#include <nettle/sha2.h>
#include "vtls/gtls.h"
#elif defined(USE_WOLFSSL)
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/quic.h>
#include "vtls/wolfssl.h"
#endif

#include "urldata.h"
#include "cfilters.h"
#include "vtls/vtls.h"
#include "vtls/vtls_scache.h"
#include "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(struct Curl_peer *origin,
                                  struct Curl_peer *peer,
                                  struct ssl_filter_config *sslc,
                                  struct ssl_peer *ssl_peer)
{
  char tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, sizeof(tls_id));
#elif defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, sizeof(tls_id));
#elif defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, sizeof(tls_id));
#아니면
#error "no TLS lib in used, should not happen"
  반환 CURLE_FAILED_INIT;
#endif
  만약(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  반환 Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(struct curl_tls_ctx *ctx,
                             struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             struct ssl_peer *ssl_peer,
                             const struct alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             void *cb_user_data, void *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  반환 Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, NULL, ssl_user_data,
                            session_reuse_cb);
#elif defined(USE_GNUTLS)
  반환 Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#elif defined(USE_WOLFSSL)
  반환 Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#아니면
#error "no TLS lib in used, should not happen"
  반환 CURLE_FAILED_INIT;
#endif
}

void Curl_vquic_tls_cleanup(struct curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#elif defined(USE_GNUTLS)
  만약(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#elif defined(USE_WOLFSSL)
  만약(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  만약(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, sizeof(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(struct curl_tls_ctx *ctx,
                                    struct Curl_cfilter *cf,
                                    struct Curl_easy *data)
{
#ifdef USE_OPENSSL
  만약(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    만약(result)
      반환 result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#elif defined(USE_WOLFSSL)
  만약(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    만약(result)
      반환 result;
  }
#elif defined(USE_GNUTLS)
  만약(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    만약(result)
      반환 result;
  }
#아니면
  (void)ctx;
  (void)cf;
  (void)data;
#endif
  반환 CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(struct curl_tls_ctx *ctx,
                                    struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    struct ssl_peer *peer)
{
  struct ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  만약(!conn_config)
    반환 CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (void)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#elif defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#elif defined(USE_WOLFSSL)
  (void)data;
  만약(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    만약(!cert)
      result = CURLE_OUT_OF_MEMORY;
    아니면 만약(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     NULL) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    아니면 만약(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  만약(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  만약(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_key);
  반환 result;
}

bool Curl_vquic_tls_get_ssl_info(struct curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 struct curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (void *)ctx->ossl.ssl_ctx : (void *)ctx->ossl.ssl;
  반환 TRUE;
#elif defined(USE_GNUTLS)
  (void)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  반환 TRUE;
#elif defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (void *)ctx->wssl.ssl_ctx : (void *)ctx->wssl.ssl;
  반환 TRUE;
#아니면
  반환 FALSE;
#endif
}

void Curl_vquic_report_handshake(struct curl_tls_ctx *ctx,
                                 struct Curl_cfilter *cf,
                                 struct Curl_easy *data)
{
  (void)cf;
#ifdef USE_OPENSSL
  (void)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#elif defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#elif defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#아니면
  (void)data;
  (void)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
