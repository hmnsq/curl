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
#অন্তর্ভাৱ "curl_setup.h"

#যদি defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#অন্তর্ভাৱ "bufq.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "vtls/vtls_int.h"

#অন্তর্ভাৱ "vtls/openssl.h"
#অন্তর্ভাৱ "vtls/wolfssl.h"

সংরচনা ssl_peer;
সংরচনা Curl_ssl_session;
সংরচনা curl_tlssessioninfo;

সংরচনা curl_tls_ctx {
#ifdef USE_OPENSSL
  সংরচনা ossl_ctx ossl;
#নোচেদ্যদি defined(USE_GNUTLS)
  সংরচনা gtls_ctx gtls;
#নোচেদ্যদি defined(USE_WOLFSSL)
  সংরচনা wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
প্রকারনির্দেশ CURLcode Curl_vquic_tls_ctx_setup(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          শূন্য *cb_user_data);

প্রকারনির্দেশ CURLcode Curl_vquic_session_reuse_cb(সংরচনা Curl_cfilter *cf,
                                             সংরচনা Curl_easy *data,
                                             সংরচনা alpn_spec *alpns,
                                             সংরচনা Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(সংরচনা Curl_peer *origin,
                                  সংরচনা Curl_peer *peer,
                                  সংরচনা ssl_filter_config *sslc,
                                  সংরচনা ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(সংরচনা curl_tls_ctx *ctx,
                             সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             সংরচনা ssl_peer *ssl_peer,
                             ধ্রুৱ সংরচনা alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             শূন্য *cb_user_data,
                             শূন্য *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
শূন্য Curl_vquic_tls_cleanup(সংরচনা curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(সংরচনা curl_tls_ctx *ctx,
                                    সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(সংরচনা curl_tls_ctx *ctx,
                                    সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(সংরচনা curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 সংরচনা curl_tlssessioninfo *info);

শূন্য Curl_vquic_report_handshake(সংরচনা curl_tls_ctx *ctx,
                                 সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
