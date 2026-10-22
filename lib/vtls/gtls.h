#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_GTLS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_GTLS_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GNUTLS

#ಅನ್ತರ್ಭಾವ <gnutls/gnutls.h>

#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

ರಚನೆ Curl_easy;
ರಚನೆ Curl_cfilter;
ರಚನೆ alpn_spec;
ರಚನೆ ssl_filter_config;
ರಚನೆ ssl_easy_config;
ರಚನೆ ssl_peer;
ರಚನೆ ssl_connect_ದತ್ತಾಂಶ;
ರಚನೆ Curl_ssl_session;

ರಚನೆ gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  ಅಕ್ಷರ *CAfile; /* CAfile path used to generate X509 store */
  ರಚನೆ curltime ಸಮಯ; /* when the shared creds was created */
  size_t refcount;
  ಬಿಟ್(trust_ಸಿದ್ಧತೆ); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_ಮೇಲೆ_ref(ರಚನೆ gtls_shared_creds *creds);
ಶೂನ್ಯ Curl_gtls_shared_creds_free(ರಚನೆ gtls_shared_creds **pcreds);

ರಚನೆ gtls_ctx {
  gnutls_session_t session;
  ರಚನೆ gtls_shared_creds *shared_creds;
  CURLcode io_ಫಲಿತಾಂಶ; /* result of last IO cfilter operation */
  ಬಿಟ್(ಕಳುಹಿಸಿದ_shutdown);
};

size_t Curl_gtls_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_gtls_ctx_ಸಿದ್ಧತೆ_cb(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_gtls_init_session_reuse_cb(ರಚನೆ Curl_cfilter *cf,
                                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ರಚನೆ alpn_spec *alpns,
                                                 ರಚನೆ Curl_ssl_session *scs,
                                                 bool *do_early_ದತ್ತಾಂಶ);

CURLcode Curl_gtls_ctx_init(ರಚನೆ gtls_ctx *gctx,
                            ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ssl_peer *peer,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns_requested,
                            Curl_gtls_ctx_ಸಿದ್ಧತೆ_cb *cb_ಸಿದ್ಧತೆ,
                            ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ,
                            ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                gnutls_session_t session,
                                ರಚನೆ ssl_filter_config *config,
                                ರಚನೆ ssl_easy_config *ssl_config,
                                ರಚನೆ ssl_peer *peer,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinned_ಕೀಲಿ);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn,
                                 ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp,
                                 size_t quic_tp_len,
                                 ರಚನೆ Curl_ssl_session **psession);

/* Report properties of a successful handshake */
ಶೂನ್ಯ Curl_gtls_report_handshake(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ gtls_ctx *gctx);

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_gnutls;

#ಆದರೆ_ಅಂತ್ಯ /* USE_GNUTLS */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_GTLS_H */
