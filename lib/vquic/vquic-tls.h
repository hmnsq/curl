#ifndef HEADER_CURL_VQUIC_TLS_H
#define HEADER_CURL_VQUIC_TLS_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"

#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"

ಸಂರಚನಾ ssl_peer;
ಸಂರಚನಾ Curl_ssl_session;
ಸಂರಚನಾ curl_tlssessioninfo;

ಸಂರಚನಾ curl_tls_ctx {
#ifdef USE_OPENSSL
  ಸಂರಚನಾ ossl_ctx ossl;
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  ಸಂರಚನಾ gtls_ctx gtls;
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  ಸಂರಚನಾ wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode Curl_vquic_tls_ctx_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          ಶೂನ್ಯ *cb_user_data);

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode Curl_vquic_session_reuse_cb(ಸಂರಚನಾ Curl_cfilter *cf,
                                             ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ alpn_spec *alpns,
                                             ಸಂರಚನಾ Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(ಸಂರಚನಾ Curl_peer *origin,
                                  ಸಂರಚನಾ Curl_peer *peer,
                                  ಸಂರಚನಾ ssl_filter_config *sslc,
                                  ಸಂರಚನಾ ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(ಸಂರಚನಾ curl_tls_ctx *ctx,
                             ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssl_peer *ssl_peer,
                             ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             ಶೂನ್ಯ *cb_user_data,
                             ಶೂನ್ಯ *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
ಶೂನ್ಯ Curl_vquic_tls_cleanup(ಸಂರಚನಾ curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                    ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                    ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ಸಂರಚನಾ curl_tlssessioninfo *info);

ಶೂನ್ಯ Curl_vquic_report_handshake(ಸಂರಚನಾ curl_tls_ctx *ctx,
                                 ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
