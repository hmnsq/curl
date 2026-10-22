#अगर_अघोषित HEADER_CURL_GTLS_H
#घोषणा HEADER_CURL_GTLS_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_GNUTLS

#समावेश <gnutls/gnutls.h>

#समावेश "curlx/timeval.h"

काष्ठा Curl_easy;
काष्ठा Curl_cfilter;
काष्ठा alpn_spec;
काष्ठा ssl_filter_config;
काष्ठा ssl_easy_config;
काष्ठा ssl_peer;
काष्ठा ssl_connect_तथ्य;
काष्ठा Curl_ssl_session;

काष्ठा gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  अक्षर *CAfile; /* CAfile path used to generate X509 store */
  काष्ठा curltime समय; /* when the shared creds was created */
  माप_प्रकार refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(काष्ठा gtls_shared_creds *creds);
व्योम Curl_gtls_shared_creds_free(काष्ठा gtls_shared_creds **pcreds);

काष्ठा gtls_ctx {
  gnutls_session_t session;
  काष्ठा gtls_shared_creds *shared_creds;
  CURLcode io_परिणाम; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

माप_प्रकार Curl_gtls_version(अक्षर *बफर, माप_प्रकार आकार);

प्रकार CURLcode Curl_gtls_ctx_setup_cb(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        व्योम *user_तथ्य);

प्रकार CURLcode Curl_gtls_init_session_reuse_cb(काष्ठा Curl_cfilter *cf,
                                                 काष्ठा Curl_easy *तथ्य,
                                                 काष्ठा alpn_spec *alpns,
                                                 काष्ठा Curl_ssl_session *scs,
                                                 bool *do_early_तथ्य);

CURLcode Curl_gtls_ctx_init(काष्ठा gtls_ctx *gctx,
                            काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा ssl_peer *peer,
                            स्थिर काष्ठा alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            व्योम *cb_user_तथ्य,
                            व्योम *ssl_user_तथ्य,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                gnutls_session_t session,
                                काष्ठा ssl_filter_config *config,
                                काष्ठा ssl_easy_config *ssl_config,
                                काष्ठा ssl_peer *peer,
                                स्थिर अक्षर *pinned_कुंजी);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *ssl_peer_कुंजी,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 स्थिर अक्षर *alpn,
                                 अचिन्हित अक्षर *quic_tp,
                                 माप_प्रकार quic_tp_len,
                                 काष्ठा Curl_ssl_session **psession);

/* Report properties of a successful handshake */
व्योम Curl_gtls_report_handshake(काष्ठा Curl_easy *तथ्य, काष्ठा gtls_ctx *gctx);

बाह्य स्थिर काष्ठा Curl_ssl Curl_ssl_gnutls;

#पूर्ण_अगर /* USE_GNUTLS */
#पूर्ण_अगर /* HEADER_CURL_GTLS_H */
