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
#ضمن "curl_setup.h"

#ifdef USE_GNUTLS

#ضمن <gnutls/gnutls.h>

#ضمن "curlx/timeval.h"

بنية Curl_easy;
بنية Curl_cfilter;
بنية alpn_spec;
بنية ssl_filter_config;
بنية ssl_easy_config;
بنية ssl_peer;
بنية ssl_connect_data;
بنية Curl_ssl_session;

بنية gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  حرف *CAfile; /* CAfile path used to generate X509 store */
  بنية curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(بنية Curl_easy *data,
                                       بنية gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(بنية gtls_shared_creds *creds);
فراغ Curl_gtls_shared_creds_free(بنية gtls_shared_creds **pcreds);

بنية gtls_ctx {
  gnutls_session_t session;
  بنية gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(حرف *buffer, size_t size);

تعريفنوع CURLcode Curl_gtls_ctx_setup_cb(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        فراغ *user_data);

تعريفنوع CURLcode Curl_gtls_init_session_reuse_cb(بنية Curl_cfilter *cf,
                                                 بنية Curl_easy *data,
                                                 بنية alpn_spec *alpns,
                                                 بنية Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(بنية gtls_ctx *gctx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            فراغ *cb_user_data,
                            فراغ *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      بنية gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                gnutls_session_t session,
                                بنية ssl_filter_config *config,
                                بنية ssl_easy_config *ssl_config,
                                بنية ssl_peer *peer,
                                ثابت حرف *pinned_کلید);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 ثابت حرف *ssl_peer_کلید,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت حرف *alpn,
                                 unsigned حرف *quic_tp,
                                 size_t quic_tp_len,
                                 بنية Curl_ssl_session **psession);

/* Report properties of a successful handshake */
فراغ Curl_gtls_report_handshake(بنية Curl_easy *data, بنية gtls_ctx *gctx);

خارجي ثابت بنية Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
