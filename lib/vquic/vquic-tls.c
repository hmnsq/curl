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
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ifdef USE_OPENSSL
#अन्तर्भाव <openssl/err.h>
#अन्तर्भाव "vtls/openssl.h"
#नोचेद्यदि defined(USE_GNUTLS)
#अन्तर्भाव <gnutls/abstract.h>
#अन्तर्भाव <gnutls/gnutls.h>
#अन्तर्भाव <gnutls/x509.h>
#अन्तर्भाव <gnutls/crypto.h>
#अन्तर्भाव <nettle/sha2.h>
#अन्तर्भाव "vtls/gtls.h"
#नोचेद्यदि defined(USE_WOLFSSL)
#अन्तर्भाव <wolfssl/options.h>
#अन्तर्भाव <wolfssl/ssl.h>
#अन्तर्भाव <wolfssl/quic.h>
#अन्तर्भाव "vtls/wolfssl.h"
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vquic/vquic-tls.h"

CURLcode Curl_vquic_tls_peer_init(संरचना Curl_peer *origin,
                                  संरचना Curl_peer *peer,
                                  संरचना ssl_filter_config *sslc,
                                  संरचना ssl_peer *ssl_peer)
{
  वर्ण tls_id[80];

#ifdef USE_OPENSSL
  Curl_ossl_version(tls_id, परिमाण(tls_id));
#नोचेद्यदि defined(USE_GNUTLS)
  Curl_gtls_version(tls_id, परिमाण(tls_id));
#नोचेद्यदि defined(USE_WOLFSSL)
  Curl_wssl_version(tls_id, परिमाण(tls_id));
#अन्यथा
#error "no TLS lib in used, should not happen"
  प्रत्यागम CURLE_FAILED_INIT;
#endif
  यदि(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  प्रत्यागम Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(संरचना curl_tls_ctx *ctx,
                             संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             संरचना ssl_peer *ssl_peer,
                             ध्रुव संरचना alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             शून्य *cb_user_data, शून्य *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#ifdef USE_OPENSSL
  प्रत्यागम Curl_ossl_ctx_init(&ctx->ossl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, रिक्त, ssl_user_data,
                            session_reuse_cb);
#नोचेद्यदि defined(USE_GNUTLS)
  प्रत्यागम Curl_gtls_ctx_init(&ctx->gtls, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data, ssl_user_data,
                            session_reuse_cb);
#नोचेद्यदि defined(USE_WOLFSSL)
  प्रत्यागम Curl_wssl_ctx_init(&ctx->wssl, cf, data, ssl_peer, alpns,
                            cb_setup, cb_user_data,
                            ssl_user_data, session_reuse_cb);
#अन्यथा
#error "no TLS lib in used, should not happen"
  प्रत्यागम CURLE_FAILED_INIT;
#endif
}

शून्य Curl_vquic_tls_cleanup(संरचना curl_tls_ctx *ctx)
{
#ifdef USE_OPENSSL
  Curl_osslctx_cleanup(&ctx->ossl);
#नोचेद्यदि defined(USE_GNUTLS)
  यदि(ctx->gtls.session)
    gnutls_deinit(ctx->gtls.session);
  Curl_gtls_shared_creds_free(&ctx->gtls.shared_creds);
#नोचेद्यदि defined(USE_WOLFSSL)
  यदि(ctx->wssl.ssl)
    wolfSSL_free(ctx->wssl.ssl);
  यदि(ctx->wssl.ssl_ctx)
    wolfSSL_CTX_free(ctx->wssl.ssl_ctx);
#endif
  memset(ctx, 0, परिमाण(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(संरचना curl_tls_ctx *ctx,
                                    संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data)
{
#ifdef USE_OPENSSL
  यदि(!ctx->ossl.x509_store_setup) {
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, &ctx->ossl);
    यदि(result)
      प्रत्यागम result;
    ctx->ossl.x509_store_setup = TRUE;
  }
#नोचेद्यदि defined(USE_WOLFSSL)
  यदि(!ctx->wssl.x509_store_setup) {
    CURLcode result = Curl_wssl_setup_x509_store(cf, data, &ctx->wssl);
    यदि(result)
      प्रत्यागम result;
  }
#नोचेद्यदि defined(USE_GNUTLS)
  यदि(!ctx->gtls.shared_creds->trust_setup) {
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &ctx->gtls);
    यदि(result)
      प्रत्यागम result;
  }
#अन्यथा
  (शून्य)ctx;
  (शून्य)cf;
  (शून्य)data;
#endif
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(संरचना curl_tls_ctx *ctx,
                                    संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना ssl_peer *peer)
{
  संरचना ssl_filter_config *conn_config;
  CURLcode result = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  यदि(!conn_config)
    प्रत्यागम CURLE_FAILED_INIT;

#ifdef USE_OPENSSL
  (शून्य)conn_config;
  result = Curl_ossl_check_peer_cert(cf, data, &ctx->ossl, peer);
#नोचेद्यदि defined(USE_GNUTLS)
  result = Curl_gtls_verifyserver(
    cf, data, ctx->gtls.session, conn_config, &data->set.ssl, peer,
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY));
#नोचेद्यदि defined(USE_WOLFSSL)
  (शून्य)data;
  यदि(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    यदि(!cert)
      result = CURLE_OUT_OF_MEMORY;
    अन्यथा यदि(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, strlen(peer->sni), 0,
                                     रिक्त) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    अन्यथा यदि(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      result = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  यदि(!result)
    result = Curl_wssl_verify_pinned(cf, data, &ctx->wssl);
#endif
  /* on error, remove any session we might have in the pool */
  यदि(result)
    Curl_ssl_scache_remove_all(cf, data, peer->scache_किल्ली);
  प्रत्यागम result;
}

bool Curl_vquic_tls_get_ssl_info(संरचना curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 संरचना curl_tlssessioninfo *info)
{
#ifdef USE_OPENSSL
  info->backend = CURLSSLBACKEND_OPENSSL;
  info->internals = give_ssl_ctx ?
                    (शून्य *)ctx->ossl.ssl_ctx : (शून्य *)ctx->ossl.ssl;
  प्रत्यागम TRUE;
#नोचेद्यदि defined(USE_GNUTLS)
  (शून्य)give_ssl_ctx; /* gnutls always returns its session */
  info->backend = CURLSSLBACKEND_GNUTLS;
  info->internals = ctx->gtls.session;
  प्रत्यागम TRUE;
#नोचेद्यदि defined(USE_WOLFSSL)
  info->backend = CURLSSLBACKEND_WOLFSSL;
  info->internals = give_ssl_ctx ?
                    (शून्य *)ctx->wssl.ssl_ctx : (शून्य *)ctx->wssl.ssl;
  प्रत्यागम TRUE;
#अन्यथा
  प्रत्यागम FALSE;
#endif
}

शून्य Curl_vquic_report_handshake(संरचना curl_tls_ctx *ctx,
                                 संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  (शून्य)cf;
#ifdef USE_OPENSSL
  (शून्य)cf;
  Curl_ossl_report_handshake(data, &ctx->ossl);
#नोचेद्यदि defined(USE_GNUTLS)
  Curl_gtls_report_handshake(data, &ctx->gtls);
#नोचेद्यदि defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(data, &ctx->wssl);
#अन्यथा
  (शून्य)data;
  (शून्य)ctx;
#endif
}

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
