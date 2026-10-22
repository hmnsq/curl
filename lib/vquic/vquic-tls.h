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
#અન્તર્ભાવ "curl_setup.h"

#જો defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_int.h"

#અન્તર્ભાવ "vtls/openssl.h"
#અન્તર્ભાવ "vtls/wolfssl.h"

સંરચના ssl_peer;
સંરચના Curl_ssl_session;
સંરચના curl_tlssessioninfo;

સંરચના curl_tls_ctx {
#ifdef USE_OPENSSL
  સંરચના ossl_ctx ossl;
#અથવાજો defined(USE_GNUTLS)
  સંરચના gtls_ctx gtls;
#અથવાજો defined(USE_WOLFSSL)
  સંરચના wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
પ્રકારવ્યાખ્યા CURLcode Curl_vquic_tls_ctx_setup(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          શૂન્ય *cb_user_data);

પ્રકારવ્યાખ્યા CURLcode Curl_vquic_session_reuse_cb(સંરચના Curl_cfilter *cf,
                                             સંરચના Curl_easy *data,
                                             સંરચના alpn_spec *alpns,
                                             સંરચના Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(સંરચના Curl_peer *origin,
                                  સંરચના Curl_peer *peer,
                                  સંરચના ssl_filter_config *sslc,
                                  સંરચના ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(સંરચના curl_tls_ctx *ctx,
                             સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             સંરચના ssl_peer *ssl_peer,
                             અચલ સંરચના alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             શૂન્ય *cb_user_data,
                             શૂન્ય *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
શૂન્ય Curl_vquic_tls_cleanup(સંરચના curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(સંરચના curl_tls_ctx *ctx,
                                    સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(સંરચના curl_tls_ctx *ctx,
                                    સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(સંરચના curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 સંરચના curl_tlssessioninfo *info);

શૂન્ય Curl_vquic_report_handshake(સંરચના curl_tls_ctx *ctx,
                                 સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
