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
#अन्तर्भाव "curl_setup.h"

#जर defined(USE_HTTP3) && \
  (defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL))

#अन्तर्भाव "bufq.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"

#अन्तर्भाव "vtls/openssl.h"
#अन्तर्भाव "vtls/wolfssl.h"

संरचना ssl_peer;
संरचना Curl_ssl_session;
संरचना curl_tlssessioninfo;

संरचना curl_tls_ctx {
#ifdef USE_OPENSSL
  संरचना ossl_ctx ossl;
#किंवाजर defined(USE_GNUTLS)
  संरचना gtls_ctx gtls;
#किंवाजर defined(USE_WOLFSSL)
  संरचना wssl_ctx wssl;
#endif
};

/**
 * Callback passed to `Curl_vquic_tls_init()` that can
 * do early initializations on the not otherwise configured TLS
 * instances created. This varies by TLS backend:
 * - openssl/wolfssl: SSL_CTX* has been created
 * - gnutls: gtls_client_init() has run
 */
प्रकारव्याख्या CURLcode Curl_vquic_tls_ctx_setup(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          शून्य *cb_user_data);

प्रकारव्याख्या CURLcode Curl_vquic_session_reuse_cb(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data,
                                             संरचना alpn_spec *alpns,
                                             संरचना Curl_ssl_session *scs,
                                             bool *do_early_data);

CURLcode Curl_vquic_tls_peer_init(संरचना Curl_peer *origin,
                                  संरचना Curl_peer *peer,
                                  संरचना ssl_filter_config *sslc,
                                  संरचना ssl_peer *ssl_peer);

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
CURLcode Curl_vquic_tls_init(संरचना curl_tls_ctx *ctx,
                             संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             संरचना ssl_peer *ssl_peer,
                             स्थिरांक संरचना alpn_spec *alpns,
                             Curl_vquic_tls_ctx_setup *cb_setup,
                             शून्य *cb_user_data,
                             शून्य *ssl_user_data,
                             Curl_vquic_session_reuse_cb *session_reuse_cb);

/**
 * Cleanup all data that has been initialized.
 */
शून्य Curl_vquic_tls_cleanup(संरचना curl_tls_ctx *ctx);

CURLcode Curl_vquic_tls_before_recv(संरचना curl_tls_ctx *ctx,
                                    संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data);

/**
 * After the QUIC basic handshake has been, verify that the peer
 * (and its certificate) fulfill our requirements.
 */
CURLcode Curl_vquic_tls_verify_peer(संरचना curl_tls_ctx *ctx,
                                    संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना ssl_peer *peer);

bool Curl_vquic_tls_get_ssl_info(संरचना curl_tls_ctx *ctx,
                                 bool give_ssl_ctx,
                                 संरचना curl_tlssessioninfo *info);

शून्य Curl_vquic_report_handshake(संरचना curl_tls_ctx *ctx,
                                 संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data);

#endif /* !USE_HTTP3 && (USE_OPENSSL || USE_GNUTLS || USE_WOLFSSL) */

#endif /* HEADER_CURL_VQUIC_TLS_H */
