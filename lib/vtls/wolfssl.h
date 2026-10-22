#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_WOLFSSL_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_WOLFSSL_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WOLFSSL

#ಅನ್ತರ್ಭಾವ "urldata.h"

#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>

ರಚನೆ alpn_spec;
ರಚನೆ ssl_peer;
ರಚನೆ Curl_ssl_session;

ರಚನೆ WOLFSSL;
ರಚನೆ WOLFSSL_CTX;
ರಚನೆ WOLFSSL_SESSION;

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_wolfssl;

ರಚನೆ wssl_ctx {
  ರಚನೆ WOLFSSL_CTX *ssl_ctx;
  ರಚನೆ WOLFSSL *ssl;
  CURLcode io_ಫಲಿತಾಂಶ;      /* result of last BIO cfilter operation */
  CURLcode hs_ಫಲಿತಾಂಶ;      /* result of handshake */
  ಪೂರ್ಣಾಂಕ io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  ಬಿಟ್(x509_store_ಸಿದ್ಧತೆ);   /* x509 store has been set up */
  ಬಿಟ್(shutting_ಕೆಳಕ್ಕೆ);      /* TLS is being shut down */
};

size_t Curl_wssl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_wssl_ctx_ಸಿದ್ಧತೆ_cb(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_wssl_init_session_reuse_cb(ರಚನೆ Curl_cfilter *cf,
                                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ರಚನೆ alpn_spec *alpns,
                                                 ರಚನೆ Curl_ssl_session *scs,
                                                 bool *do_early_ದತ್ತಾಂಶ);

CURLcode Curl_wssl_ctx_init(ರಚನೆ wssl_ctx *wctx,
                            ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ssl_peer *peer,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns_requested,
                            Curl_wssl_ctx_ಸಿದ್ಧತೆ_cb *cb_ಸಿದ್ಧತೆ,
                            ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ,
                            ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_ಸಿದ್ಧತೆ_x509_store(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ wssl_ctx *wssl);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_EX_ದತ್ತಾಂಶ
CURLcode Curl_wssl_cache_session(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                 ರಚನೆ WOLFSSL_SESSION *session,
                                 ಪೂರ್ಣಾಂಕ ietf_tls_id,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn,
                                 ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp,
                                 size_t quic_tp_len,
                                 ರಚನೆ Curl_ssl_session **pscs);
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_wssl_verify_pinned(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ wssl_ctx *wssl);

ಶೂನ್ಯ Curl_wssl_report_handshake(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ wssl_ctx *wssl);

#ಆದರೆ_ಅಂತ್ಯ /* USE_WOLFSSL */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_WOLFSSL_H */
