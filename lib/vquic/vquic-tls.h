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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_int.h"

#ଅନ୍ତର୍ଭାଵ "vtls/openssl.h"
#ଅନ୍ତର୍ଭାଵ "vtls/wolfssl.h"

ସଂରଚନା ssl_peer;
ସଂରଚନା Curl_ssl_session;
ସଂରଚନା curl_tlssessioninfo;

ସଂରଚନା curl_tls_ctx {
#ifdef USE_OPENSSL
  ସଂରଚନା ossl_ctx ossl;
#ନଚେତଯଦି defined(USE_GNUTLS)
  ସଂରଚନା gtls_ctx gtls;
#ନଚେତଯଦି defined(USE_WOLFSSL)
  ସଂରଚନା wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode Curl_vquic_tls_ctx_setup(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          ଶୂନ୍ୟ *cb_user_data);

ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode Curl_vquic_session_reuse_cb(ସଂରଚନା Curl_cfilter *cf,
                                             ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା alpn_spec *alpns,
                                             ସଂରଚନା Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(ସଂରଚନା Curl_peer *origin,
                                  ସଂରଚନା Curl_peer *peer,
                                  ସଂରଚନା ssl_filter_config *sslc,
                                  ସଂରଚନା ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(ସଂରଚନା curl_tls_ctx *ctx,
                             ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssl_peer *ssl_peer,
                             ଅଚଳ ସଂରଚନା alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             ଶୂନ୍ୟ *cb_user_data,
                             ଶୂନ୍ୟ *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
ଶୂନ୍ୟ Curl_vquic_tls_cleanup(ସଂରଚନା curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(ସଂରଚନା curl_tls_ctx *ctx,
                                    ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(ସଂରଚନା curl_tls_ctx *ctx,
                                    ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(ସଂରଚନା curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 ସଂରଚନା curl_tlssessioninfo *info);

ଶୂନ୍ୟ Curl_vquic_report_handshake(ସଂରଚନା curl_tls_ctx *ctx,
                                 ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
