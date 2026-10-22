#నిర్వచితం_కాకపోతే శీర్షిక_CURL_GTLS_H
#నిర్వచించు శీర్షిక_CURL_GTLS_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_GNUTLS

#అన్తర్భావ <gnutls/gnutls.h>

#అన్తర్భావ "curlx/timeval.h"

నిర్మాణం Curl_easy;
నిర్మాణం Curl_cfilter;
నిర్మాణం alpn_spec;
నిర్మాణం ssl_filter_config;
నిర్మాణం ssl_easy_config;
నిర్మాణం ssl_peer;
నిర్మాణం ssl_connect_దత్తాంశం;
నిర్మాణం Curl_ssl_session;

నిర్మాణం gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  అక్షరం *CAfile; /* CAfile path used to generate X509 store */
  నిర్మాణం curltime సమయం; /* when the shared creds was created */
  size_t refcount;
  బిట్టు(trust_అమరిక); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_పైకి_ref(నిర్మాణం gtls_shared_creds *creds);
శూన్య Curl_gtls_shared_creds_free(నిర్మాణం gtls_shared_creds **pcreds);

నిర్మాణం gtls_ctx {
  gnutls_session_t session;
  నిర్మాణం gtls_shared_creds *shared_creds;
  CURLcode io_ఫలితం; /* result of last IO cfilter operation */
  బిట్టు(పంపిన_shutdown);
};

size_t Curl_gtls_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత);

రకనిర్వచనం CURLcode Curl_gtls_ctx_అమరిక_cb(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        శూన్య *user_దత్తాంశం);

రకనిర్వచనం CURLcode Curl_gtls_init_session_reuse_cb(నిర్మాణం Curl_cfilter *cf,
                                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                                 నిర్మాణం alpn_spec *alpns,
                                                 నిర్మాణం Curl_ssl_session *scs,
                                                 bool *do_early_దత్తాంశం);

CURLcode Curl_gtls_ctx_init(నిర్మాణం gtls_ctx *gctx,
                            నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ssl_peer *peer,
                            స్థిరరాశి నిర్మాణం alpn_spec *alpns_requested,
                            Curl_gtls_ctx_అమరిక_cb *cb_అమరిక,
                            శూన్య *cb_user_దత్తాంశం,
                            శూన్య *ssl_user_దత్తాంశం,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                gnutls_session_t session,
                                నిర్మాణం ssl_filter_config *config,
                                నిర్మాణం ssl_easy_config *ssl_config,
                                నిర్మాణం ssl_peer *peer,
                                స్థిరరాశి అక్షరం *pinned_కీలకం);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 స్థిరరాశి అక్షరం *alpn,
                                 గుర్తులేని అక్షరం *quic_tp,
                                 size_t quic_tp_len,
                                 నిర్మాణం Curl_ssl_session **psession);

/* Report properties of a successful handshake */
శూన్య Curl_gtls_report_handshake(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం gtls_ctx *gctx);

బాహ్య స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_gnutls;

#అయితే_ముగింపు /* USE_GNUTLS */
#అయితే_ముగింపు /* HEADER_CURL_GTLS_H */
