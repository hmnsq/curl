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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_int.h"

#അന്തര്ഭാവ "vtls/openssl.h"
#അന്തര്ഭാവ "vtls/wolfssl.h"

ഘടന ssl_peer;
ഘടന Curl_ssl_session;
ഘടന curl_tlssessioninfo;

ഘടന curl_tls_ctx {
#ifdef USE_OPENSSL
  ഘടന ossl_ctx ossl;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  ഘടന gtls_ctx gtls;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  ഘടന wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
തരനിർവചനം CURLcode Curl_vquic_tls_ctx_setup(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          ശൂന്യം *cb_user_data);

തരനിർവചനം CURLcode Curl_vquic_session_reuse_cb(ഘടന Curl_cfilter *cf,
                                             ഘടന Curl_easy *data,
                                             ഘടന alpn_spec *alpns,
                                             ഘടന Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(ഘടന Curl_peer *origin,
                                  ഘടന Curl_peer *peer,
                                  ഘടന ssl_filter_config *sslc,
                                  ഘടന ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(ഘടന curl_tls_ctx *ctx,
                             ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             ഘടന ssl_peer *ssl_peer,
                             സ്ഥിരം ഘടന alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             ശൂന്യം *cb_user_data,
                             ശൂന്യം *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
ശൂന്യം Curl_vquic_tls_cleanup(ഘടന curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(ഘടന curl_tls_ctx *ctx,
                                    ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(ഘടന curl_tls_ctx *ctx,
                                    ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(ഘടന curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ഘടന curl_tlssessioninfo *info);

ശൂന്യം Curl_vquic_report_handshake(ഘടന curl_tls_ctx *ctx,
                                 ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
