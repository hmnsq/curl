#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_GTLS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_GTLS_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GNUTLS

#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/gnutls.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_cfilter;
ਬਣਤਰ alpn_spec;
ਬਣਤਰ ssl_filter_config;
ਬਣਤਰ ssl_easy_config;
ਬਣਤਰ ssl_peer;
ਬਣਤਰ ssl_connect_ਅੰਕੜੇ;
ਬਣਤਰ Curl_ssl_session;

ਬਣਤਰ gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  ਅੱਖਰ *CAfile; /* CAfile path used to generate X509 store */
  ਬਣਤਰ curltime ਸਮਾਂ; /* when the shared creds was created */
  size_t refcount;
  ਦੋਆਧਾਰੀ_ਅੰਕ(trust_ਤਿਆਰੀ); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_ਉੱਪਰ_ref(ਬਣਤਰ gtls_shared_creds *creds);
ਖਾਲੀ Curl_gtls_shared_creds_free(ਬਣਤਰ gtls_shared_creds **pcreds);

ਬਣਤਰ gtls_ctx {
  gnutls_session_t session;
  ਬਣਤਰ gtls_shared_creds *shared_creds;
  CURLcode io_ਨਤੀਜਾ; /* result of last IO cfilter operation */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਭੇਜਿਆ_shutdown);
};

size_t Curl_gtls_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_gtls_ctx_ਤਿਆਰੀ_cb(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਖਾਲੀ *user_ਅੰਕੜੇ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_gtls_init_session_reuse_cb(ਬਣਤਰ Curl_cfilter *cf,
                                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਬਣਤਰ alpn_spec *alpns,
                                                 ਬਣਤਰ Curl_ssl_session *scs,
                                                 bool *do_early_ਅੰਕੜੇ);

CURLcode Curl_gtls_ctx_init(ਬਣਤਰ gtls_ctx *gctx,
                            ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ssl_peer *peer,
                            ਅਚਲ ਬਣਤਰ alpn_spec *alpns_requested,
                            Curl_gtls_ctx_ਤਿਆਰੀ_cb *cb_ਤਿਆਰੀ,
                            ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                            ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                gnutls_session_t session,
                                ਬਣਤਰ ssl_filter_config *config,
                                ਬਣਤਰ ssl_easy_config *ssl_config,
                                ਬਣਤਰ ssl_peer *peer,
                                ਅਚਲ ਅੱਖਰ *pinned_ਕੁੰਜੀ);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ਅਚਲ ਅੱਖਰ *alpn,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp,
                                 size_t quic_tp_len,
                                 ਬਣਤਰ Curl_ssl_session **psession);

/* Report properties of a successful handshake */
ਖਾਲੀ Curl_gtls_report_handshake(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ gtls_ctx *gctx);

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_gnutls;

#ਜੇ_ਅੰਤ /* USE_GNUTLS */
#ਜੇ_ਅੰਤ /* HEADER_CURL_GTLS_H */
