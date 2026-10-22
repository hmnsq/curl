#ifndef HEADER_CURL_GTLS_H
#define HEADER_CURL_GTLS_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_GNUTLS

#ಅನ್ತರ್ಭಾವ <gnutls/gnutls.h>

#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ Curl_cfilter;
ಸಂರಚನಾ alpn_spec;
ಸಂರಚನಾ ssl_filter_config;
ಸಂರಚನಾ ssl_easy_config;
ಸಂರಚನಾ ssl_peer;
ಸಂರಚನಾ ssl_connect_data;
ಸಂರಚನಾ Curl_ssl_session;

ಸಂರಚನಾ gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  ವರ್ಣ *CAfile; /* CAfile path used to generate X509 store */
  ಸಂರಚನಾ curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(ಸಂರಚನಾ gtls_shared_creds *creds);
ಶೂನ್ಯ Curl_gtls_shared_creds_free(ಸಂರಚನಾ gtls_shared_creds **pcreds);

ಸಂರಚನಾ gtls_ctx {
  gnutls_session_t session;
  ಸಂರಚನಾ gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(ವರ್ಣ *buffer, size_t size);

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode Curl_gtls_ctx_setup_cb(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಶೂನ್ಯ *user_data);

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode Curl_gtls_init_session_reuse_cb(ಸಂರಚನಾ Curl_cfilter *cf,
                                                 ಸಂರಚನಾ Curl_easy *data,
                                                 ಸಂರಚನಾ alpn_spec *alpns,
                                                 ಸಂರಚನಾ Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(ಸಂರಚನಾ gtls_ctx *gctx,
                            ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ssl_peer *peer,
                            ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            ಶೂನ್ಯ *cb_user_data,
                            ಶೂನ್ಯ *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                gnutls_session_t session,
                                ಸಂರಚನಾ ssl_filter_config *config,
                                ಸಂರಚನಾ ssl_easy_config *ssl_config,
                                ಸಂರಚನಾ ssl_peer *peer,
                                ಧ್ರುವ ವರ್ಣ *pinned_ಕೀಲಿ);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ಧ್ರುವ ವರ್ಣ *alpn,
                                 unsigned ವರ್ಣ *quic_tp,
                                 size_t quic_tp_len,
                                 ಸಂರಚನಾ Curl_ssl_session **psession);

/* Report properties of a successful handshake */
ಶೂನ್ಯ Curl_gtls_report_handshake(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ gtls_ctx *gctx);

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
