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

#यदि_परिभाषितम् USE_OPENSSL
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
#यदि_अन्तः

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

#यदि_परिभाषितम् USE_OPENSSL
  Curl_ossl_संस्करणम्(tls_id, परिमाण(tls_id));
#नोचेद्यदि defined(USE_GNUTLS)
  Curl_gtls_संस्करणम्(tls_id, परिमाण(tls_id));
#नोचेद्यदि defined(USE_WOLFSSL)
  Curl_wssl_संस्करणम्(tls_id, परिमाण(tls_id));
#अन्यथा
#दोषः "no TLS lib in used, should not happen"
  प्रत्यागम CURLE_FAILED_INIT;
#यदि_अन्तः
  यदि(ssl_peer->origin || ssl_peer->peer)
    Curl_ssl_peer_cleanup(ssl_peer);
  प्रत्यागम Curl_ssl_peer_init(ssl_peer, origin, peer, sslc,
                            tls_id, TRNSPRT_QUIC);
}

CURLcode Curl_vquic_tls_init(संरचना curl_tls_ctx *ctx,
                             संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             संरचना ssl_peer *ssl_peer,
                             ध्रुव संरचना alpn_spec *alpns,
                             Curl_vquic_tls_ctx_सज्जता *cb_सज्जता,
                             शून्य *cb_user_दत्त, शून्य *ssl_user_दत्त,
                             Curl_vquic_session_reuse_cb *session_reuse_cb)
{
#यदि_परिभाषितम् USE_OPENSSL
  प्रत्यागम Curl_ossl_ctx_init(&ctx->ossl, cf, दत्त, ssl_peer, alpns,
                            cb_सज्जता, cb_user_दत्त, रिक्त, ssl_user_दत्त,
                            session_reuse_cb);
#नोचेद्यदि defined(USE_GNUTLS)
  प्रत्यागम Curl_gtls_ctx_init(&ctx->gtls, cf, दत्त, ssl_peer, alpns,
                            cb_सज्जता, cb_user_दत्त, ssl_user_दत्त,
                            session_reuse_cb);
#नोचेद्यदि defined(USE_WOLFSSL)
  प्रत्यागम Curl_wssl_ctx_init(&ctx->wssl, cf, दत्त, ssl_peer, alpns,
                            cb_सज्जता, cb_user_दत्त,
                            ssl_user_दत्त, session_reuse_cb);
#अन्यथा
#दोषः "no TLS lib in used, should not happen"
  प्रत्यागम CURLE_FAILED_INIT;
#यदि_अन्तः
}

शून्य Curl_vquic_tls_cleanup(संरचना curl_tls_ctx *ctx)
{
#यदि_परिभाषितम् USE_OPENSSL
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
#यदि_अन्तः
  स्मृतिं_पूरय(ctx, 0, परिमाण(*ctx));
}

CURLcode Curl_vquic_tls_before_recv(संरचना curl_tls_ctx *ctx,
                                    संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त)
{
#यदि_परिभाषितम् USE_OPENSSL
  यदि(!ctx->ossl.x509_store_सज्जता) {
    CURLcode परिणाम = Curl_ssl_सज्जता_x509_store(cf, दत्त, &ctx->ossl);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    ctx->ossl.x509_store_सज्जता = TRUE;
  }
#नोचेद्यदि defined(USE_WOLFSSL)
  यदि(!ctx->wssl.x509_store_सज्जता) {
    CURLcode परिणाम = Curl_wssl_सज्जता_x509_store(cf, दत्त, &ctx->wssl);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
#नोचेद्यदि defined(USE_GNUTLS)
  यदि(!ctx->gtls.shared_creds->trust_सज्जता) {
    CURLcode परिणाम = Curl_gtls_client_trust_सज्जता(cf, दत्त, &ctx->gtls);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
#अन्यथा
  (शून्य)ctx;
  (शून्य)cf;
  (शून्य)दत्त;
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_vquic_tls_verify_peer(संरचना curl_tls_ctx *ctx,
                                    संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना ssl_peer *peer)
{
  संरचना ssl_filter_config *conn_config;
  CURLcode परिणाम = CURLE_OK;

  conn_config = Curl_ssl_cf_get_filter_config(cf);
  यदि(!conn_config)
    प्रत्यागम CURLE_FAILED_INIT;

#यदि_परिभाषितम् USE_OPENSSL
  (शून्य)conn_config;
  परिणाम = Curl_ossl_check_peer_cert(cf, दत्त, &ctx->ossl, peer);
#नोचेद्यदि defined(USE_GNUTLS)
  परिणाम = Curl_gtls_verifyserver(
    cf, दत्त, ctx->gtls.session, conn_config, &दत्त->समुच्चयः.ssl, peer,
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY));
#नोचेद्यदि defined(USE_WOLFSSL)
  (शून्य)दत्त;
  यदि(conn_config->verifyhost) {
    WOLFSSL_X509 *cert = wolfSSL_get_peer_certificate(ctx->wssl.ssl);
    यदि(!cert)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    अन्यथा यदि(peer->sni &&
            (wolfSSL_X509_check_host(cert, peer->sni, वाक्यदैर्घ्यम्(peer->sni), 0,
                                     रिक्त) == WOLFSSL_FAILURE))
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    अन्यथा यदि(!peer->sni &&
            (wolfSSL_X509_check_ip_asc(cert, peer->origin->hostname,
                                       0) == WOLFSSL_FAILURE))
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    wolfSSL_X509_free(cert);
  }
  यदि(!परिणाम)
    परिणाम = Curl_wssl_verify_pinned(cf, दत्त, &ctx->wssl);
#यदि_अन्तः
  /* on error, remove any session we might have in the pool */
  यदि(परिणाम)
    Curl_ssl_scache_remove_all(cf, दत्त, peer->scache_कुञ्चिका);
  प्रत्यागम परिणाम;
}

bool Curl_vquic_tls_get_ssl_info(संरचना curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 संरचना curl_tlssessioninfo *info)
{
#यदि_परिभाषितम् USE_OPENSSL
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
#यदि_अन्तः
}

शून्य Curl_vquic_report_handshake(संरचना curl_tls_ctx *ctx,
                                 संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त)
{
  (शून्य)cf;
#यदि_परिभाषितम् USE_OPENSSL
  (शून्य)cf;
  Curl_ossl_report_handshake(दत्त, &ctx->ossl);
#नोचेद्यदि defined(USE_GNUTLS)
  Curl_gtls_report_handshake(दत्त, &ctx->gtls);
#नोचेद्यदि defined(USE_WOLFSSL)
  Curl_wssl_report_handshake(दत्त, &ctx->wssl);
#अन्यथा
  (शून्य)दत्त;
  (शून्य)ctx;
#यदि_अन्तः
}

#यदि_अन्तः /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */
