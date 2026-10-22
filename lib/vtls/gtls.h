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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_GNUTLS

#அந்தர்பாவ <gnutls/gnutls.h>

#அந்தர்பாவ "curlx/timeval.h"

கட்டமைப்பு Curl_easy;
கட்டமைப்பு Curl_cfilter;
கட்டமைப்பு alpn_spec;
கட்டமைப்பு ssl_filter_config;
கட்டமைப்பு ssl_easy_config;
கட்டமைப்பு ssl_peer;
கட்டமைப்பு ssl_connect_data;
கட்டமைப்பு Curl_ssl_session;

கட்டமைப்பு gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  எழுத்து *CAfile; /* CAfile path used to generate X509 store */
  கட்டமைப்பு curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(கட்டமைப்பு gtls_shared_creds *creds);
வெறுமை Curl_gtls_shared_creds_free(கட்டமைப்பு gtls_shared_creds **pcreds);

கட்டமைப்பு gtls_ctx {
  gnutls_session_t session;
  கட்டமைப்பு gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(எழுத்து *buffer, size_t size);

வகைவரை CURLcode Curl_gtls_ctx_setup_cb(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        வெறுமை *user_data);

வகைவரை CURLcode Curl_gtls_init_session_reuse_cb(கட்டமைப்பு Curl_cfilter *cf,
                                                 கட்டமைப்பு Curl_easy *data,
                                                 கட்டமைப்பு alpn_spec *alpns,
                                                 கட்டமைப்பு Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(கட்டமைப்பு gtls_ctx *gctx,
                            கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு ssl_peer *peer,
                            மாறிலி கட்டமைப்பு alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            வெறுமை *cb_user_data,
                            வெறுமை *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                gnutls_session_t session,
                                கட்டமைப்பு ssl_filter_config *config,
                                கட்டமைப்பு ssl_easy_config *ssl_config,
                                கட்டமைப்பு ssl_peer *peer,
                                மாறிலி எழுத்து *pinned_சாவி);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *ssl_peer_சாவி,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 மாறிலி எழுத்து *alpn,
                                 unsigned எழுத்து *quic_tp,
                                 size_t quic_tp_len,
                                 கட்டமைப்பு Curl_ssl_session **psession);

/* Report properties of a successful handshake */
வெறுமை Curl_gtls_report_handshake(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு gtls_ctx *gctx);

வெளி மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
