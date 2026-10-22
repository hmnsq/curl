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
#அந்தர்பாவ "curl_setup.h"

#எனில் defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_int.h"

#அந்தர்பாவ "vtls/openssl.h"
#அந்தர்பாவ "vtls/wolfssl.h"

கட்டமைப்பு ssl_peer;
கட்டமைப்பு Curl_ssl_session;
கட்டமைப்பு curl_tlssessioninfo;

கட்டமைப்பு curl_tls_ctx {
#ifdef USE_OPENSSL
  கட்டமைப்பு ossl_ctx ossl;
#அல்லதெனில் defined(USE_GNUTLS)
  கட்டமைப்பு gtls_ctx gtls;
#அல்லதெனில் defined(USE_WOLFSSL)
  கட்டமைப்பு wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
வகைவரை CURLcode Curl_vquic_tls_ctx_setup(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          வெறுமை *cb_user_data);

வகைவரை CURLcode Curl_vquic_session_reuse_cb(கட்டமைப்பு Curl_cfilter *cf,
                                             கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு alpn_spec *alpns,
                                             கட்டமைப்பு Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(கட்டமைப்பு Curl_peer *origin,
                                  கட்டமைப்பு Curl_peer *peer,
                                  கட்டமைப்பு ssl_filter_config *sslc,
                                  கட்டமைப்பு ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(கட்டமைப்பு curl_tls_ctx *ctx,
                             கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssl_peer *ssl_peer,
                             மாறிலி கட்டமைப்பு alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             வெறுமை *cb_user_data,
                             வெறுமை *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
வெறுமை Curl_vquic_tls_cleanup(கட்டமைப்பு curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(கட்டமைப்பு curl_tls_ctx *ctx,
                                    கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(கட்டமைப்பு curl_tls_ctx *ctx,
                                    கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(கட்டமைப்பு curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 கட்டமைப்பு curl_tlssessioninfo *info);

வெறுமை Curl_vquic_report_handshake(கட்டமைப்பு curl_tls_ctx *ctx,
                                 கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
