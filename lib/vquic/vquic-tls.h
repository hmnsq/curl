#अगर_अघोषित HEADER_CURL_VQUIC_TLS_H
#घोषणा HEADER_CURL_VQUIC_TLS_H
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

#समावेश "bufq.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_int.h"

#समावेश "vtls/openssl.h"
#समावेश "vtls/wolfssl.h"

काष्ठा ssl_peer;
काष्ठा Curl_ssl_session;
काष्ठा curl_tlssessioninfo;

काष्ठा curl_tls_ctx {
#अगर_घोषित USE_OPENSSL
  काष्ठा ossl_ctx ossl;
#या_अगर defined(USE_GNUTLS)
  काष्ठा gtls_ctx gtls;
#या_अगर defined(USE_WOLFSSL)
  काष्ठा wssl_ctx wssl;
#पूर्ण_अगर
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
प्रकार CURLcode Curl_vquic_tls_ctx_setup(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          व्योम *cb_user_तथ्य);

प्रकार CURLcode Curl_vquic_session_reuse_cb(काष्ठा Curl_cfilter *cf,
                                             काष्ठा Curl_easy *तथ्य,
                                             काष्ठा alpn_spec *alpns,
                                             काष्ठा Curl_ssl_session *scs,
                                             bool *do_early_तथ्य);

CURLcode Curl_vquic_tls_peer_init(काष्ठा Curl_peer *origin,
                                  काष्ठा Curl_peer *peer,
                                  काष्ठा ssl_filter_config *sslc,
                                  काष्ठा ssl_peer *ssl_peer);

/**
 * Initialize the QUIC TLS instances based of the SSL configurations
 * for the connection filter, transfer and peer.
 * @param ctx              the TLS context to initialize
 * @param cf               the connection filter involved
 * @param data             the transfer involved
 * @param ssl_peer         the SSL peer to be connected to
 * @param alpns            the ALPN specifications to negotiate, may be NULL
 * @param cb_setup         optional callback for early TLS config
 * @param cb_user_data     user_data param for callback
 * @param ssl_user_data    optional pointer to set in TLS application context
 * @param session_reuse_cb callback to handle session reuse, signal early data
 */
CURLcode Curl_vquic_tls_init(काष्ठा curl_tls_ctx *ctx,
                             काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssl_peer *ssl_peer,
                             स्थिर काष्ठा alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             व्योम *cb_user_तथ्य,
                             व्योम *ssl_user_तथ्य,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
व्योम Curl_vquic_tls_cleanup(काष्ठा curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(काष्ठा curl_tls_ctx *ctx,
                                    काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(काष्ठा curl_tls_ctx *ctx,
                                    काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(काष्ठा curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 काष्ठा curl_tlssessioninfo *info);

व्योम Curl_vquic_report_handshake(काष्ठा curl_tls_ctx *ctx,
                                 काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य);

#पूर्ण_अगर /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#पूर्ण_अगर /* HEADER_CURL_VQUIC_TLS_H */
