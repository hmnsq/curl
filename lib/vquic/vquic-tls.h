#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VQUIC_TLS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VQUIC_TLS_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/openssl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/wolfssl.h"

ਬਣਤਰ ssl_peer;
ਬਣਤਰ Curl_ssl_session;
ਬਣਤਰ curl_tlssessioninfo;

ਬਣਤਰ curl_tls_ctx {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  ਬਣਤਰ ossl_ctx ossl;
#ਜਾਂਜੇ defined(USE_GNUTLS)
  ਬਣਤਰ gtls_ctx gtls;
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  ਬਣਤਰ wssl_ctx wssl;
#ਜੇ_ਅੰਤ
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_vquic_tls_ctx_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਖਾਲੀ *cb_user_ਅੰਕੜੇ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_vquic_session_reuse_cb(ਬਣਤਰ Curl_cfilter *cf,
                                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ alpn_spec *alpns,
                                             ਬਣਤਰ Curl_ssl_session *scs,
                                             bool *do_early_ਅੰਕੜੇ);

CURLcode Curl_vquic_tls_peer_init(ਬਣਤਰ Curl_peer *origin,
                                  ਬਣਤਰ Curl_peer *peer,
                                  ਬਣਤਰ ssl_filter_config *sslc,
                                  ਬਣਤਰ ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(ਬਣਤਰ curl_tls_ctx *ctx,
                             ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssl_peer *ssl_peer,
                             ਅਚਲ ਬਣਤਰ alpn_spec *alpns,
                             Curl_vquic_tls_ctx_ਤਿਆਰੀ *cb_ਤਿਆਰੀ,
                             ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                             ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
ਖਾਲੀ Curl_vquic_tls_cleanup(ਬਣਤਰ curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(ਬਣਤਰ curl_tls_ctx *ctx,
                                    ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(ਬਣਤਰ curl_tls_ctx *ctx,
                                    ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(ਬਣਤਰ curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ਬਣਤਰ curl_tlssessioninfo *info);

ਖਾਲੀ Curl_vquic_report_handshake(ਬਣਤਰ curl_tls_ctx *ctx,
                                 ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VQUIC_TLS_H */
