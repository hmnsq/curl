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
#समावेश "curl_setup.h"

#अगर defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#अगर_घोषित USE_OPENSSL
#समावेश <openssl/err.h>
#समावेश "vtls/openssl.h"
#या_अगर defined(USE_GNUTLS)
#समावेश <gnutls/abstract.h>
#समावेश <gnutls/gnutls.h>
#समावेश <gnutls/x509.h>
#समावेश <gnutls/crypto.h>
#समावेश <nettle/sha2.h>
#समावेश "vtls/gtls.h"
#या_अगर defined(USE_WOLFSSL)
#समावेश <wolfssl/options.h>
#समावेश <wolfssl/ssl.h>
#समावेश <wolfssl/quic.h>
#समावेश "vtls/wolfssl.h"
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(काष्ठा Curl_peer *origin,
                                  काष्ठा Curl_peer *peer,
                                  काष्ठा ssl_filter_config *sslc,
                                  काष्ठा ssl_peer *ssl_peer)
{
  अक्षर tls_id[80];

#अगर_घोषित USE_OPENSSL
  Curl_ossl_version(tls_id, माप(tls_id));
#या_अगर defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, माप(tls_id));
#या_अगर defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, माप(tls_id));
#अन्यथा
#त्रुटि "no TLS lib in used, should not happen"
  वापस CURLE_FAILED_INIT;
#पूर्ण_अगर
  अगर(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  वापस Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(काष्ठा curl_tls_ctx *ctx,
                             काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssl_peer *ssl_peer,
                             स्थिर काष्ठा alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             व्योम *cb_user_तथ्य, व्योम *ssl_user_तथ्य,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#अगर_घोषित USE_OPENSSL
  वापस Curl_ossl_ctx_init(&ctx->ossl, cf, तथ्य, ssl_peer, alpns,
                            cb_setup, cb_user_तथ्य, शून्य, ssl_user_तथ्य,
                            session_reuse_cb);
#या_अगर defined(USE_GNUTLS)
  वापस Curl_gtls_ctx_init(&ctx->gtls, cf, तथ्य, ssl_peer, alpns,
                            cb_setup, cb_user_तथ्य, ssl_user_तथ्य,
                            session_reuse_cb);
#या_अगर defined(USE_WOLFSSL)
  वापस Curl_wssl_ctx_init(&ctx->wssl, cf, तथ्य, ssl_peer, alpns,
                            cb_setup, cb_user_तथ्य,
                            ssl_user_तथ्य, session_reuse_cb);
#अन्यथा
#त्रुटि "no TLS lib in used, should not happen"
  वापस CURLE_FAILED_INIT;
#पूर्ण_अगर
}

व्योम Curl_vquic_tls_cleanup(काष्ठा curl_tls_ctx *ctx)
{
#अगर_घोषित USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#या_अगर defined(USE_GNUTLS)
  अगर(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#या_अगर defined(USE_WOLFSSL)
  अगर(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  अगर(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#पूर्ण_अगर
  स_रखो(ctx, 0, माप(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(काष्ठा curl_tls_ctx *ctx,
                                    काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य)
{
#अगर_घोषित USE_OPENSSL
  अगर(!ctx->ossl.x509_store_setup) {
    CURLcode परिणाम = Curl_ssl_setup_x509_store(cf, तथ्य, &ctx->ossl);
    अगर(परिणाम)
      वापस परिणाम;
    ctx->ossl.x509_store_setup = TRUE;
  }
#या_अगर defined(USE_WOLFSSL)
  अगर(!ctx->wssl.x509_store_setup) {
    CURLcode परिणाम = Curl_wssl_setup_x509_store(cf, तथ्य, &ctx->wssl);
    अगर(परिणाम)
      वापस परिणाम;
  }
#या_अगर defined(USE_GNUTLS)
  अगर(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode परिणाम = Curl_gtls_client_trust_setup(cf, तथ्य, &ctx->gtls);
    अगर(परिणाम)
      वापस परिणाम;
  }
#अन्यथा
  (व्योम)ctx;
  (व्योम)cf;
  (व्योम)तथ्य;
#पूर्ण_अगर
  वापस CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(काष्ठा curl_tls_ctx *ctx,
                                    काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssl_peer *peer)
{
  काष्ठा ssl_filter_config *conn_config;
  CURLcode परिणाम = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  अगर(!conn_config)
    वापस CURLE_FAILED_INIT;

#अगर_घोषित USE_OPENSSL
  (व्योम)conn_config;
  परिणाम = Curl_ossl_check_peer_cert(cf, तथ्य, &ctx->ossl, peer);
#या_अगर defined(USE_GNUTLS)
  परिणाम = Curl_gtls_verifyserver(
    cf, तथ्य, ctx->gtls.session, conn_config, &तथ्य->set.ssl, peer,
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY));
#या_अगर defined(USE_WOLFSSL)
  (व्योम)तथ्य;
  अगर(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    अगर(!cert)
      परिणाम = CURLE_OUT_OF_MEMORY;
    अन्यथा अगर(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, म_माप(peer->sni), 0,
                                     शून्य) == WOLFSSL_FAILURE))
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    अन्यथा अगर(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  अगर(!परिणाम)
    परिणाम = Curl_wssl_verify_pinned(cf, तथ्य, &ctx->wssl);
#पूर्ण_अगर
  /* on error, remove any session we might have in the pool */
  अगर(परिणाम)
    Curl_ssl_scache_remove_all(cf, तथ्य, peer->scache_कुंजी);
  वापस परिणाम;
}

bool Curl_vquic_tls_get_ssl_info(काष्ठा curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 काष्ठा curl_tlssessioninfo *info)
{
#अगर_घोषित USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (व्योम *)ctx->ossl.ssl_ctx : (व्योम *)ctx->ossl.ssl;
  वापस TRUE;
#या_अगर defined(USE_GNUTLS)
  (व्योम)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  वापस TRUE;
#या_अगर defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (व्योम *)ctx->wssl.ssl_ctx : (व्योम *)ctx->wssl.ssl;
  वापस TRUE;
#अन्यथा
  वापस FALSE;
#पूर्ण_अगर
}

व्योम Curl_vquic_report_handshake(काष्ठा curl_tls_ctx *ctx,
                                 काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य)
{
  (व्योम)cf;
#अगर_घोषित USE_OPENSSL
  (व्योम)cf;
  Curl_ossl_report_handshake(तथ्य, &ctx->ossl);
#या_अगर defined(USE_GNUTLS)
  Curl_gtls_report_handshake(तथ्य, &ctx->gtls);
#या_अगर defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(तथ्य, &ctx->wssl);
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)ctx;
#पूर्ण_अगर
}

#पूर्ण_अगर /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
