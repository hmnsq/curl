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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_GNUTLS

#અન્તર્ભાવ <gnutls/gnutls.h>

#અન્તર્ભાવ "curlx/timeval.h"

સંરચના Curl_easy;
સંરચના Curl_cfilter;
સંરચના alpn_spec;
સંરચના ssl_filter_config;
સંરચના ssl_easy_config;
સંરચના ssl_peer;
સંરચના ssl_connect_data;
સંરચના Curl_ssl_session;

સંરચના gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  અક્ષર *CAfile; /* CAfile path used to generate X509 store */
  સંરચના curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(સંરચના Curl_easy *data,
                                       સંરચના gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(સંરચના gtls_shared_creds *creds);
શૂન્ય Curl_gtls_shared_creds_free(સંરચના gtls_shared_creds **pcreds);

સંરચના gtls_ctx {
  gnutls_session_t session;
  સંરચના gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(અક્ષર *buffer, size_t size);

પ્રકારવ્યાખ્યા CURLcode Curl_gtls_ctx_setup_cb(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        શૂન્ય *user_data);

પ્રકારવ્યાખ્યા CURLcode Curl_gtls_init_session_reuse_cb(સંરચના Curl_cfilter *cf,
                                                 સંરચના Curl_easy *data,
                                                 સંરચના alpn_spec *alpns,
                                                 સંરચના Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(સંરચના gtls_ctx *gctx,
                            સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            સંરચના ssl_peer *peer,
                            અચલ સંરચના alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            શૂન્ય *cb_user_data,
                            શૂન્ય *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                gnutls_session_t session,
                                સંરચના ssl_filter_config *config,
                                સંરચના ssl_easy_config *ssl_config,
                                સંરચના ssl_peer *peer,
                                અચલ અક્ષર *pinned_ચાવી);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અચલ અક્ષર *ssl_peer_ચાવી,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 અચલ અક્ષર *alpn,
                                 unsigned અક્ષર *quic_tp,
                                 size_t quic_tp_len,
                                 સંરચના Curl_ssl_session **psession);

/* Report properties of a successful handshake */
શૂન્ય Curl_gtls_report_handshake(સંરચના Curl_easy *data, સંરચના gtls_ctx *gctx);

બાહ્ય અચલ સંરચના Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
