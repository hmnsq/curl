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
#شامل "curl_setup.h"

#اگر defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#شامل "bufq.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"

#شامل "vtls/openssl.h"
#شامل "vtls/wolfssl.h"

ساختار ssl_peer;
ساختار Curl_ssl_session;
ساختار curl_tlssessioninfo;

ساختار curl_tls_ctx {
#ifdef USE_OPENSSL
  ساختار ossl_ctx ossl;
#وگرنهاگر defined(USE_GNUTLS)
  ساختار gtls_ctx gtls;
#وگرنهاگر defined(USE_WOLFSSL)
  ساختار wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
تعریفنوع CURLcode Curl_vquic_tls_ctx_setup(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          تهی *cb_user_data);

تعریفنوع CURLcode Curl_vquic_session_reuse_cb(ساختار Curl_cfilter *cf,
                                             ساختار Curl_easy *data,
                                             ساختار alpn_spec *alpns,
                                             ساختار Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(ساختار Curl_peer *origin,
                                  ساختار Curl_peer *peer,
                                  ساختار ssl_filter_config *sslc,
                                  ساختار ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(ساختار curl_tls_ctx *ctx,
                             ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             ساختار ssl_peer *ssl_peer,
                             ثابت ساختار alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             تهی *cb_user_data,
                             تهی *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
تهی Curl_vquic_tls_cleanup(ساختار curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(ساختار curl_tls_ctx *ctx,
                                    ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(ساختار curl_tls_ctx *ctx,
                                    ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(ساختار curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ساختار curl_tlssessioninfo *info);

تهی Curl_vquic_report_handshake(ساختار curl_tls_ctx *ctx,
                                 ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
