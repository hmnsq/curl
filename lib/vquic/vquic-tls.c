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
#અન્તર્ભાવ "curl_setup.h"

#જો defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#અન્તર્ભાવ <openssl/err.h>
#અન્તર્ભાવ "vtls/openssl.h"
#અથવાજો defined(USE_GNUTLS)
#અન્તર્ભાવ <gnutls/abstract.h>
#અન્તર્ભાવ <gnutls/gnutls.h>
#અન્તર્ભાવ <gnutls/x509.h>
#અન્તર્ભાવ <gnutls/crypto.h>
#અન્તર્ભાવ <nettle/sha2.h>
#અન્તર્ભાવ "vtls/gtls.h"
#અથવાજો defined(USE_WOLFSSL)
#અન્તર્ભાવ <wolfssl/options.h>
#અન્તર્ભાવ <wolfssl/ssl.h>
#અન્તર્ભાવ <wolfssl/quic.h>
#અન્તર્ભાવ "vtls/wolfssl.h"
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(સંરચના Curl_peer *origin,
                                  સંરચના Curl_peer *peer,
                                  સંરચના ssl_filter_config *sslc,
                                  સંરચના ssl_peer *ssl_peer)
{
  અક્ષર tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, કદ(tls_id));
#અથવાજો defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, કદ(tls_id));
#અથવાજો defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, કદ(tls_id));
#નહીંતર
#error "no TLS lib in used, should not happen"
  પાછા CURLE_FAILED_INIT;
#endif
  જો(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  પાછા Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(સંરચના curl_tls_ctx *ctx,
                             સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             સંરચના ssl_peer *ssl_peer,
                             અચલ સંરચના alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             શૂન્ય *cb_user_data, શૂન્ય *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  પાછા Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ખાલી, ssl_user_data,
                            session_reuse_cb);
#અથવાજો defined(USE_GNUTLS)
  પાછા Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#અથવાજો defined(USE_WOLFSSL)
  પાછા Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#નહીંતર
#error "no TLS lib in used, should not happen"
  પાછા CURLE_FAILED_INIT;
#endif
}

શૂન્ય Curl_vquic_tls_cleanup(સંરચના curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#અથવાજો defined(USE_GNUTLS)
  જો(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#અથવાજો defined(USE_WOLFSSL)
  જો(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  જો(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, કદ(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(સંરચના curl_tls_ctx *ctx,
                                    સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data)
{
#ifdef USE_OPENSSL
  જો(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    જો(result)
      પાછા result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#અથવાજો defined(USE_WOLFSSL)
  જો(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    જો(result)
      પાછા result;
  }
#અથવાજો defined(USE_GNUTLS)
  જો(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    જો(result)
      પાછા result;
  }
#નહીંતર
  (શૂન્ય)ctx;
  (શૂન્ય)cf;
  (શૂન્ય)data;
#endif
  પાછા CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(સંરચના curl_tls_ctx *ctx,
                                    સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના ssl_peer *peer)
{
  સંરચના ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  જો(!conn_config)
    પાછા CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (શૂન્ય)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#અથવાજો defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#અથવાજો defined(USE_WOLFSSL)
  (શૂન્ય)data;
  જો(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    જો(!cert)
      result = CURLE_OUT_OF_MEMORY;
    નહીંતર જો(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     ખાલી) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    નહીંતર જો(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  જો(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  જો(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_ચાવી);
  પાછા result;
}

bool Curl_vquic_tls_get_ssl_info(સંરચના curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 સંરચના curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (શૂન્ય *)ctx->ossl.ssl_ctx : (શૂન્ય *)ctx->ossl.ssl;
  પાછા TRUE;
#અથવાજો defined(USE_GNUTLS)
  (શૂન્ય)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  પાછા TRUE;
#અથવાજો defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (શૂન્ય *)ctx->wssl.ssl_ctx : (શૂન્ય *)ctx->wssl.ssl;
  પાછા TRUE;
#નહીંતર
  પાછા FALSE;
#endif
}

શૂન્ય Curl_vquic_report_handshake(સંરચના curl_tls_ctx *ctx,
                                 સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data)
{
  (શૂન્ય)cf;
#ifdef USE_OPENSSL
  (શૂન્ય)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#અથવાજો defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#અથવાજો defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
