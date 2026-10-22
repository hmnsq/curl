#ifndef HEADER_CURL_WOLFSSL_H
#define HEADER_CURL_WOLFSSL_H
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

#ifdef USE_WOLFSSL

#ಅನ್ತರ್ಭಾವ "urldata.h"

#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>

ಸಂರಚನಾ alpn_spec;
ಸಂರಚನಾ ssl_peer;
ಸಂರಚನಾ Curl_ssl_session;

ಸಂರಚನಾ WOLFSSL;
ಸಂರಚನಾ WOLFSSL_CTX;
ಸಂರಚನಾ WOLFSSL_SESSION;

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ Curl_ssl Curl_ssl_wolfssl;

ಸಂರಚನಾ wssl_ctx {
  ಸಂರಚನಾ WOLFSSL_CTX *ssl_ctx;
  ಸಂರಚನಾ WOLFSSL *ssl;
  CURLcode io_result;      /* result of last BIO cfilter operation */
  CURLcode hs_result;      /* result of handshake */
  ಪೂರ್ಣಾಙ್ಕ io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(ವರ್ಣ *buffer, size_t size);

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode Curl_wssl_ctx_setup_cb(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಶೂನ್ಯ *user_data);

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode Curl_wssl_init_session_reuse_cb(ಸಂರಚನಾ Curl_cfilter *cf,
                                                 ಸಂರಚನಾ Curl_easy *data,
                                                 ಸಂರಚನಾ alpn_spec *alpns,
                                                 ಸಂರಚನಾ Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_wssl_ctx_init(ಸಂರಚನಾ wssl_ctx *wctx,
                            ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ssl_peer *peer,
                            ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            ಶೂನ್ಯ *cb_user_data,
                            ಶೂನ್ಯ *ssl_user_data,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ wssl_ctx *wssl);

#ifdef HAVE_EX_DATA
CURLcode Curl_wssl_cache_session(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                 ಸಂರಚನಾ WOLFSSL_SESSION *session,
                                 ಪೂರ್ಣಾಙ್ಕ ietf_tls_id,
                                 ಧ್ರುವ ವರ್ಣ *alpn,
                                 unsigned ವರ್ಣ *quic_tp,
                                 size_t quic_tp_len,
                                 ಸಂರಚನಾ Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ wssl_ctx *wssl);

ಶೂನ್ಯ Curl_wssl_report_handshake(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
