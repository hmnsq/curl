#اگر_غیر_تعریف_شدہ سرخی_CURL_GTLS_H
#تعریف_کرو سرخی_CURL_GTLS_H
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

#اگر_تعریف_شدہ USE_GNUTLS

#شامل <gnutls/gnutls.h>

#شامل "curlx/timeval.h"

ساخت Curl_easy;
ساخت Curl_cfilter;
ساخت alpn_spec;
ساخت ssl_filter_config;
ساخت ssl_easy_config;
ساخت ssl_peer;
ساخت ssl_connect_ڈیٹا;
ساخت Curl_ssl_session;

ساخت gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  حرف *CAfile; /* CAfile path used to generate X509 store */
  ساخت curltime وقت; /* when the shared creds was created */
  size_t refcount;
  ثنائی_ہندسہ(trust_تیاری); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(ساخت Curl_easy *ڈیٹا,
                                       ساخت gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_اوپر_ref(ساخت gtls_shared_creds *creds);
خالی Curl_gtls_shared_creds_free(ساخت gtls_shared_creds **pcreds);

ساخت gtls_ctx {
  gnutls_session_t session;
  ساخت gtls_shared_creds *shared_creds;
  CURLcode io_نتیجہ; /* result of last IO cfilter operation */
  ثنائی_ہندسہ(بھیجا_shutdown);
};

size_t Curl_gtls_نسخہ(حرف *بفر, size_t سائز);

قسمتعریف CURLcode Curl_gtls_ctx_تیاری_cb(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        خالی *user_ڈیٹا);

قسمتعریف CURLcode Curl_gtls_init_session_reuse_cb(ساخت Curl_cfilter *cf,
                                                 ساخت Curl_easy *ڈیٹا,
                                                 ساخت alpn_spec *alpns,
                                                 ساخت Curl_ssl_session *scs,
                                                 bool *do_early_ڈیٹا);

CURLcode Curl_gtls_ctx_init(ساخت gtls_ctx *gctx,
                            ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت ssl_peer *peer,
                            ثابت ساخت alpn_spec *alpns_requested,
                            Curl_gtls_ctx_تیاری_cb *cb_تیاری,
                            خالی *cb_user_ڈیٹا,
                            خالی *ssl_user_ڈیٹا,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_تیاری(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                gnutls_session_t session,
                                ساخت ssl_filter_config *config,
                                ساخت ssl_easy_config *ssl_config,
                                ساخت ssl_peer *peer,
                                ثابت حرف *pinned_کلید);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *ssl_peer_کلید,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت حرف *alpn,
                                 غیرعلامتی حرف *quic_tp,
                                 size_t quic_tp_len,
                                 ساخت Curl_ssl_session **psession);

/* Report properties of a successful handshake */
خالی Curl_gtls_report_handshake(ساخت Curl_easy *ڈیٹا, ساخت gtls_ctx *gctx);

خارجی ثابت ساخت Curl_ssl Curl_ssl_gnutls;

#اگر_ختم /* USE_GNUTLS */
#اگر_ختم /* HEADER_CURL_GTLS_H */
