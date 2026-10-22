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
#شامل "curl_setup.h"

#ifdef USE_GNUTLS

#شامل <gnutls/gnutls.h>

#شامل "curlx/timeval.h"

ساختار Curl_easy;
ساختار Curl_cfilter;
ساختار alpn_spec;
ساختار ssl_filter_config;
ساختار ssl_easy_config;
ساختار ssl_peer;
ساختار ssl_connect_data;
ساختار Curl_ssl_session;

ساختار gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  نویسه *CAfile; /* CAfile path used to generate X509 store */
  ساختار curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(ساختار Curl_easy *data,
                                       ساختار gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(ساختار gtls_shared_creds *creds);
تهی Curl_gtls_shared_creds_free(ساختار gtls_shared_creds **pcreds);

ساختار gtls_ctx {
  gnutls_session_t session;
  ساختار gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(نویسه *buffer, size_t size);

تعریفنوع CURLcode Curl_gtls_ctx_setup_cb(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        تهی *user_data);

تعریفنوع CURLcode Curl_gtls_init_session_reuse_cb(ساختار Curl_cfilter *cf,
                                                 ساختار Curl_easy *data,
                                                 ساختار alpn_spec *alpns,
                                                 ساختار Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(ساختار gtls_ctx *gctx,
                            ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            ساختار ssl_peer *peer,
                            ثابت ساختار alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            تهی *cb_user_data,
                            تهی *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                gnutls_session_t session,
                                ساختار ssl_filter_config *config,
                                ساختار ssl_easy_config *ssl_config,
                                ساختار ssl_peer *peer,
                                ثابت نویسه *pinned_کلید);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ثابت نویسه *ssl_peer_کلید,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت نویسه *alpn,
                                 unsigned نویسه *quic_tp,
                                 size_t quic_tp_len,
                                 ساختار Curl_ssl_session **psession);

/* Report properties of a successful handshake */
تهی Curl_gtls_report_handshake(ساختار Curl_easy *data, ساختار gtls_ctx *gctx);

بیرونی ثابت ساختار Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
