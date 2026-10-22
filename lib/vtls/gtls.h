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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_GNUTLS

#ଅନ୍ତର୍ଭାଵ <gnutls/gnutls.h>

#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"

ସଂରଚନା Curl_easy;
ସଂରଚନା Curl_cfilter;
ସଂରଚନା alpn_spec;
ସଂରଚନା ssl_filter_config;
ସଂରଚନା ssl_easy_config;
ସଂରଚନା ssl_peer;
ସଂରଚନା ssl_connect_data;
ସଂରଚନା Curl_ssl_session;

ସଂରଚନା gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  ଅକ୍ଷର *CAfile; /* CAfile path used to generate X509 store */
  ସଂରଚନା curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(ସଂରଚନା gtls_shared_creds *creds);
ଶୂନ୍ୟ Curl_gtls_shared_creds_free(ସଂରଚନା gtls_shared_creds **pcreds);

ସଂରଚନା gtls_ctx {
  gnutls_session_t session;
  ସଂରଚନା gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(ଅକ୍ଷର *buffer, size_t size);

ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode Curl_gtls_ctx_setup_cb(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ଶୂନ୍ୟ *user_data);

ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode Curl_gtls_init_session_reuse_cb(ସଂରଚନା Curl_cfilter *cf,
                                                 ସଂରଚନା Curl_easy *data,
                                                 ସଂରଚନା alpn_spec *alpns,
                                                 ସଂରଚନା Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(ସଂରଚନା gtls_ctx *gctx,
                            ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ssl_peer *peer,
                            ଅଚଳ ସଂରଚନା alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            ଶୂନ୍ୟ *cb_user_data,
                            ଶୂନ୍ୟ *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                gnutls_session_t session,
                                ସଂରଚନା ssl_filter_config *config,
                                ସଂରଚନା ssl_easy_config *ssl_config,
                                ସଂରଚନା ssl_peer *peer,
                                ଅଚଳ ଅକ୍ଷର *pinned_ଚାବି);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ଅଚଳ ଅକ୍ଷର *alpn,
                                 unsigned ଅକ୍ଷର *quic_tp,
                                 size_t quic_tp_len,
                                 ସଂରଚନା Curl_ssl_session **psession);

/* Report properties of a successful handshake */
ଶୂନ୍ୟ Curl_gtls_report_handshake(ସଂରଚନା Curl_easy *data, ସଂରଚନା gtls_ctx *gctx);

ବାହ୍ୟ ଅଚଳ ସଂରଚନା Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
