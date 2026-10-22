#اگر_غیر_تعریف_شدہ سرخی_CURL_VQUIC_TLS_H
#تعریف_کرو سرخی_CURL_VQUIC_TLS_H
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
#شامل "curl_setup.h"

#اگر defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#شامل "bufq.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"

#شامل "vtls/openssl.h"
#شامل "vtls/wolfssl.h"

ساخت ssl_peer;
ساخت Curl_ssl_session;
ساخت curl_tlssessioninfo;

ساخت curl_tls_ctx {
#اگر_تعریف_شدہ USE_OPENSSL
  ساخت ossl_ctx ossl;
#وگرنہ_اگر defined(USE_GNUTLS)
  ساخت gtls_ctx gtls;
#وگرنہ_اگر defined(USE_WOLFSSL)
  ساخت wssl_ctx wssl;
#اگر_ختم
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
قسمتعریف CURLcode Curl_vquic_tls_ctx_تیاری(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          خالی *cb_user_ڈیٹا);

قسمتعریف CURLcode Curl_vquic_session_reuse_cb(ساخت Curl_cfilter *cf,
                                             ساخت Curl_easy *ڈیٹا,
                                             ساخت alpn_spec *alpns,
                                             ساخت Curl_ssl_session *scs,
                                             bool *do_early_ڈیٹا);

CURLcode Curl_vquic_tls_peer_init(ساخت Curl_peer *origin,
                                  ساخت Curl_peer *peer,
                                  ساخت ssl_filter_config *sslc,
                                  ساخت ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(ساخت curl_tls_ctx *ctx,
                             ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت ssl_peer *ssl_peer,
                             ثابت ساخت alpn_spec *alpns,
                             Curl_vquic_tls_ctx_تیاری *cb_تیاری,
                             خالی *cb_user_ڈیٹا,
                             خالی *ssl_user_ڈیٹا,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
خالی Curl_vquic_tls_cleanup(ساخت curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(ساخت curl_tls_ctx *ctx,
                                    ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(ساخت curl_tls_ctx *ctx,
                                    ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(ساخت curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ساخت curl_tlssessioninfo *info);

خالی Curl_vquic_report_handshake(ساخت curl_tls_ctx *ctx,
                                 ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا);

#اگر_ختم /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#اگر_ختم /* HEADER_CURL_VQUIC_TLS_H */
