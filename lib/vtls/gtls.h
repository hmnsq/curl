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

কাঠামো Curl_easy;
কাঠামো Curl_cfilter;
কাঠামো alpn_spec;
কাঠামো ssl_filter_config;
কাঠামো ssl_easy_config;
কাঠামো ssl_peer;
কাঠামো ssl_connect_তথ্য;
কাঠামো Curl_ssl_session;

কাঠামো gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  অক্ষর *CAfile; /* CAfile path used to generate X509 store */
  কাঠামো curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(কাঠামো gtls_shared_creds *creds);
শূন্য Curl_gtls_shared_creds_free(কাঠামো gtls_shared_creds **pcreds);

কাঠামো gtls_ctx {
  gnutls_session_t session;
  কাঠামো gtls_shared_creds *shared_creds;
  CURLcode io_ফলাফল; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(অক্ষর *বাফার, size_t আকার_2);

প্রকারলেখ CURLcode Curl_gtls_ctx_setup_cb(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        শূন্য *user_তথ্য);

প্রকারলেখ CURLcode Curl_gtls_init_session_reuse_cb(কাঠামো Curl_cfilter *cf,
                                                 কাঠামো Curl_easy *তথ্য,
                                                 কাঠামো alpn_spec *alpns,
                                                 কাঠামো Curl_ssl_session *scs,
                                                 bool *do_early_তথ্য);

CURLcode Curl_gtls_ctx_init(কাঠামো gtls_ctx *gctx,
                            কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো ssl_peer *peer,
                            ধ্রুবক কাঠামো alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_তথ্য,
                            শূন্য *ssl_user_তথ্য,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                gnutls_session_t session,
                                কাঠামো ssl_filter_config *config,
                                কাঠামো ssl_easy_config *ssl_config,
                                কাঠামো ssl_peer *peer,
                                ধ্রুবক অক্ষর *pinned_চাবি);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ধ্রুবক অক্ষর *alpn,
                                 unsigned অক্ষর *quic_tp,
                                 size_t quic_tp_len,
                                 কাঠামো Curl_ssl_session **psession);

/* Report properties of a successful handshake */
শূন্য Curl_gtls_report_handshake(কাঠামো Curl_easy *তথ্য, কাঠামো gtls_ctx *gctx);

বাহ্যিক ধ্রুবক কাঠামো Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
