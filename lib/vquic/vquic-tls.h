#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VQUIC_TLS_H
#నిర్వచించు శీర్షిక_CURL_VQUIC_TLS_H
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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#అన్తర్భావ "bufq.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_int.h"

#అన్తర్భావ "vtls/openssl.h"
#అన్తర్భావ "vtls/wolfssl.h"

నిర్మాణం ssl_peer;
నిర్మాణం Curl_ssl_session;
నిర్మాణం curl_tlssessioninfo;

నిర్మాణం curl_tls_ctx {
#నిర్వచితమైతే USE_OPENSSL
  నిర్మాణం ossl_ctx ossl;
#లేదావేళ defined(USE_GNUTLS)
  నిర్మాణం gtls_ctx gtls;
#లేదావేళ defined(USE_WOLFSSL)
  నిర్మాణం wssl_ctx wssl;
#అయితే_ముగింపు
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
రకనిర్వచనం CURLcode Curl_vquic_tls_ctx_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          శూన్య *cb_user_దత్తాంశం);

రకనిర్వచనం CURLcode Curl_vquic_session_reuse_cb(నిర్మాణం Curl_cfilter *cf,
                                             నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం alpn_spec *alpns,
                                             నిర్మాణం Curl_ssl_session *scs,
                                             bool *do_early_దత్తాంశం);

CURLcode Curl_vquic_tls_peer_init(నిర్మాణం Curl_peer *origin,
                                  నిర్మాణం Curl_peer *peer,
                                  నిర్మాణం ssl_filter_config *sslc,
                                  నిర్మాణం ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(నిర్మాణం curl_tls_ctx *ctx,
                             నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssl_peer *ssl_peer,
                             స్థిరరాశి నిర్మాణం alpn_spec *alpns,
                             Curl_vquic_tls_ctx_అమరిక *cb_అమరిక,
                             శూన్య *cb_user_దత్తాంశం,
                             శూన్య *ssl_user_దత్తాంశం,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
శూన్య Curl_vquic_tls_cleanup(నిర్మాణం curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(నిర్మాణం curl_tls_ctx *ctx,
                                    నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(నిర్మాణం curl_tls_ctx *ctx,
                                    నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(నిర్మాణం curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 నిర్మాణం curl_tlssessioninfo *info);

శూన్య Curl_vquic_report_handshake(నిర్మాణం curl_tls_ctx *ctx,
                                 నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం);

#అయితే_ముగింపు /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#అయితే_ముగింపు /* HEADER_CURL_VQUIC_TLS_H */
