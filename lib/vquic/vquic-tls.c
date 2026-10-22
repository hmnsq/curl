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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#അന്തര്ഭാവ <openssl/err.h>
#അന്തര്ഭാവ "vtls/openssl.h"
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
#അന്തര്ഭാവ <gnutls/abstract.h>
#അന്തര്ഭാവ <gnutls/gnutls.h>
#അന്തര്ഭാവ <gnutls/x509.h>
#അന്തര്ഭാവ <gnutls/crypto.h>
#അന്തര്ഭാവ <nettle/sha2.h>
#അന്തര്ഭാവ "vtls/gtls.h"
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
#അന്തര്ഭാവ <wolfssl/options.h>
#അന്തര്ഭാവ <wolfssl/ssl.h>
#അന്തര്ഭാവ <wolfssl/quic.h>
#അന്തര്ഭാവ "vtls/wolfssl.h"
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(ഘടന Curl_peer *origin,
                                  ഘടന Curl_peer *peer,
                                  ഘടന ssl_filter_config *sslc,
                                  ഘടന ssl_peer *ssl_peer)
{
  അക്ഷരം tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, വലുപ്പം(tls_id));
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, വലുപ്പം(tls_id));
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, വലുപ്പം(tls_id));
#അല്ലെങ്കിൽ
#error "no TLS lib in used, should not happen"
  തിരികെ CURLE_FAILED_INIT;
#endif
  എങ്കിൽ(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  തിരികെ Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(ഘടന curl_tls_ctx *ctx,
                             ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             ഘടന ssl_peer *ssl_peer,
                             സ്ഥിരം ഘടന alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             ശൂന്യം *cb_user_data, ശൂന്യം *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  തിരികെ Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ശൂന്യ, ssl_user_data,
                            session_reuse_cb);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  തിരികെ Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  തിരികെ Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#അല്ലെങ്കിൽ
#error "no TLS lib in used, should not happen"
  തിരികെ CURLE_FAILED_INIT;
#endif
}

ശൂന്യം Curl_vquic_tls_cleanup(ഘടന curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  എങ്കിൽ(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  എങ്കിൽ(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  എങ്കിൽ(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, വലുപ്പം(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(ഘടന curl_tls_ctx *ctx,
                                    ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data)
{
#ifdef USE_OPENSSL
  എങ്കിൽ(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    എങ്കിൽ(result)
      തിരികെ result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  എങ്കിൽ(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    എങ്കിൽ(result)
      തിരികെ result;
  }
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  എങ്കിൽ(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    എങ്കിൽ(result)
      തിരികെ result;
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)ctx;
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
#endif
  തിരികെ CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(ഘടന curl_tls_ctx *ctx,
                                    ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന ssl_peer *peer)
{
  ഘടന ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  എങ്കിൽ(!conn_config)
    തിരികെ CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (ശൂന്യം)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  (ശൂന്യം)data;
  എങ്കിൽ(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    എങ്കിൽ(!cert)
      result = CURLE_OUT_OF_MEMORY;
    അല്ലെങ്കിൽ എങ്കിൽ(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     ശൂന്യ) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    അല്ലെങ്കിൽ എങ്കിൽ(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  എങ്കിൽ(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  എങ്കിൽ(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_താക്കോൽ);
  തിരികെ result;
}

bool Curl_vquic_tls_get_ssl_info(ഘടന curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ഘടന curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (ശൂന്യം *)ctx->ossl.ssl_ctx : (ശൂന്യം *)ctx->ossl.ssl;
  തിരികെ TRUE;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  (ശൂന്യം)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  തിരികെ TRUE;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (ശൂന്യം *)ctx->wssl.ssl_ctx : (ശൂന്യം *)ctx->wssl.ssl;
  തിരികെ TRUE;
#അല്ലെങ്കിൽ
  തിരികെ FALSE;
#endif
}

ശൂന്യം Curl_vquic_report_handshake(ഘടന curl_tls_ctx *ctx,
                                 ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data)
{
  (ശൂന്യം)cf;
#ifdef USE_OPENSSL
  (ശൂന്യം)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
