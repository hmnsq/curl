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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_GNUTLS

#অন্তর্ভাব <gnutls/gnutls.h>

#অন্তর্ভাব "curlx/timeval.h"

সংরচনা Curl_easy;
সংরচনা Curl_cfilter;
সংরচনা alpn_spec;
সংরচনা ssl_filter_config;
সংরচনা ssl_easy_config;
সংরচনা ssl_peer;
সংরচনা ssl_connect_data;
সংরচনা Curl_ssl_session;

সংরচনা gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  বর্ণ *CAfile; /* CAfile path used to generate X509 store */
  সংরচনা curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(সংরচনা Curl_easy *data,
                                       সংরচনা gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(সংরচনা gtls_shared_creds *creds);
শূন্য Curl_gtls_shared_creds_free(সংরচনা gtls_shared_creds **pcreds);

সংরচনা gtls_ctx {
  gnutls_session_t session;
  সংরচনা gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(বর্ণ *buffer, size_t size);

প্রকারনির্দেশ CURLcode Curl_gtls_ctx_setup_cb(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        শূন্য *user_data);

প্রকারনির্দেশ CURLcode Curl_gtls_init_session_reuse_cb(সংরচনা Curl_cfilter *cf,
                                                 সংরচনা Curl_easy *data,
                                                 সংরচনা alpn_spec *alpns,
                                                 সংরচনা Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(সংরচনা gtls_ctx *gctx,
                            সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            সংরচনা ssl_peer *peer,
                            ধ্রুব সংরচনা alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_data,
                            শূন্য *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                gnutls_session_t session,
                                সংরচনা ssl_filter_config *config,
                                সংরচনা ssl_easy_config *ssl_config,
                                সংরচনা ssl_peer *peer,
                                ধ্রুব বর্ণ *pinned_চাবি);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ধ্রুব বর্ণ *ssl_peer_চাবি,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ধ্রুব বর্ণ *alpn,
                                 unsigned বর্ণ *quic_tp,
                                 size_t quic_tp_len,
                                 সংরচনা Curl_ssl_session **psession);

/* Report properties of a successful handshake */
শূন্য Curl_gtls_report_handshake(সংরচনা Curl_easy *data, সংরচনা gtls_ctx *gctx);

বাহ্য ধ্রুব সংরচনা Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
