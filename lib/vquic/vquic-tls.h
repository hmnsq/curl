#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VQUIC_TLS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VQUIC_TLS_H
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

#ಆದರೆ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"

#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"

ರಚನೆ ssl_peer;
ರಚನೆ Curl_ssl_session;
ರಚನೆ curl_tlssessioninfo;

ರಚನೆ curl_tls_ctx {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  ರಚನೆ ossl_ctx ossl;
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  ರಚನೆ gtls_ctx gtls;
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  ರಚನೆ wssl_ctx wssl;
#ಆದರೆ_ಅಂತ್ಯ
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_vquic_tls_ctx_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_vquic_session_reuse_cb(ರಚನೆ Curl_cfilter *cf,
                                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ alpn_spec *alpns,
                                             ರಚನೆ Curl_ssl_session *scs,
                                             bool *do_early_ದತ್ತಾಂಶ);

CURLcode Curl_vquic_tls_peer_init(ರಚನೆ Curl_peer *origin,
                                  ರಚನೆ Curl_peer *peer,
                                  ರಚನೆ ssl_filter_config *sslc,
                                  ರಚನೆ ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(ರಚನೆ curl_tls_ctx *ctx,
                             ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssl_peer *ssl_peer,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns,
                             Curl_vquic_tls_ctx_ಸಿದ್ಧತೆ *cb_ಸಿದ್ಧತೆ,
                             ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ,
                             ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
ಶೂನ್ಯ Curl_vquic_tls_cleanup(ರಚನೆ curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(ರಚನೆ curl_tls_ctx *ctx,
                                    ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(ರಚನೆ curl_tls_ctx *ctx,
                                    ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(ರಚನೆ curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ರಚನೆ curl_tlssessioninfo *info);

ಶೂನ್ಯ Curl_vquic_report_handshake(ರಚನೆ curl_tls_ctx *ctx,
                                 ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VQUIC_TLS_H */
