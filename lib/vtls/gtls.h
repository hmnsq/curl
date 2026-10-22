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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_GNUTLS

#അന്തര്ഭാവ <gnutls/gnutls.h>

#അന്തര്ഭാവ "curlx/timeval.h"

ഘടന Curl_easy;
ഘടന Curl_cfilter;
ഘടന alpn_spec;
ഘടന ssl_filter_config;
ഘടന ssl_easy_config;
ഘടന ssl_peer;
ഘടന ssl_connect_data;
ഘടന Curl_ssl_session;

ഘടന gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  അക്ഷരം *CAfile; /* CAfile path used to generate X509 store */
  ഘടന curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(ഘടന Curl_easy *data,
                                       ഘടന gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(ഘടന gtls_shared_creds *creds);
ശൂന്യം Curl_gtls_shared_creds_free(ഘടന gtls_shared_creds **pcreds);

ഘടന gtls_ctx {
  gnutls_session_t session;
  ഘടന gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(അക്ഷരം *buffer, size_t size);

തരനിർവചനം CURLcode Curl_gtls_ctx_setup_cb(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ശൂന്യം *user_data);

തരനിർവചനം CURLcode Curl_gtls_init_session_reuse_cb(ഘടന Curl_cfilter *cf,
                                                 ഘടന Curl_easy *data,
                                                 ഘടന alpn_spec *alpns,
                                                 ഘടന Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(ഘടന gtls_ctx *gctx,
                            ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            ഘടന ssl_peer *peer,
                            സ്ഥിരം ഘടന alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            ശൂന്യം *cb_user_data,
                            ശൂന്യം *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      ഘടന gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                gnutls_session_t session,
                                ഘടന ssl_filter_config *config,
                                ഘടന ssl_easy_config *ssl_config,
                                ഘടന ssl_peer *peer,
                                സ്ഥിരം അക്ഷരം *pinned_താക്കോൽ);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 സ്ഥിരം അക്ഷരം *alpn,
                                 unsigned അക്ഷരം *quic_tp,
                                 size_t quic_tp_len,
                                 ഘടന Curl_ssl_session **psession);

/* Report properties of a successful handshake */
ശൂന്യം Curl_gtls_report_handshake(ഘടന Curl_easy *data, ഘടന gtls_ctx *gctx);

ബാഹ്യ സ്ഥിരം ഘടന Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
