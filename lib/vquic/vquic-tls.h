#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VQUIC_TLS_H
#وصف_ڪريو سرو_CURL_VQUIC_TLS_H
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
#ضمن "curl_setup.h"

#إذا defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ضمن "bufq.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"

#ضمن "vtls/openssl.h"
#ضمن "vtls/wolfssl.h"

بنية ssl_peer;
بنية Curl_ssl_session;
بنية curl_tlssessioninfo;

بنية curl_tls_ctx {
#جيڪڏهن_وصف_ٿيل USE_OPENSSL
  بنية ossl_ctx ossl;
#وإلاإذا defined(USE_GNUTLS)
  بنية gtls_ctx gtls;
#وإلاإذا defined(USE_WOLFSSL)
  بنية wssl_ctx wssl;
#جيڪڏهن_پورو
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
تعريفنوع CURLcode Curl_vquic_tls_ctx_تياري(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          فراغ *cb_user_معلومات);

تعريفنوع CURLcode Curl_vquic_session_reuse_cb(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *معلومات,
                                             بنية alpn_spec *alpns,
                                             بنية Curl_ssl_session *scs,
                                             bool *do_early_معلومات);

CURLcode Curl_vquic_tls_peer_init(بنية Curl_peer *origin,
                                  بنية Curl_peer *peer,
                                  بنية ssl_filter_config *sslc,
                                  بنية ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(بنية curl_tls_ctx *ctx,
                             بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             بنية ssl_peer *ssl_peer,
                             ثابت بنية alpn_spec *alpns,
                             Curl_vquic_tls_ctx_تياري *cb_تياري,
                             فراغ *cb_user_معلومات,
                             فراغ *ssl_user_معلومات,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
فراغ Curl_vquic_tls_cleanup(بنية curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(بنية curl_tls_ctx *ctx,
                                    بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(بنية curl_tls_ctx *ctx,
                                    بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(بنية curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 بنية curl_tlssessioninfo *info);

فراغ Curl_vquic_report_handshake(بنية curl_tls_ctx *ctx,
                                 بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات);

#جيڪڏهن_پورو /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#جيڪڏهن_پورو /* HEADER_CURL_VQUIC_TLS_H */
