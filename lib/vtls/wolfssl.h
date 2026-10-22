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
#شامل "curl_setup.h"

#ifdef USE_WOLFSSL

#شامل "urldata.h"

#شامل <wolfssl/options.h>

ساختار alpn_spec;
ساختار ssl_peer;
ساختار Curl_ssl_session;

ساختار WOLFSSL;
ساختار WOLFSSL_CTX;
ساختار WOLFSSL_SESSION;

بیرونی ثابت ساختار Curl_ssl Curl_ssl_wolfssl;

ساختار wssl_ctx {
  ساختار WOLFSSL_CTX *ssl_ctx;
  ساختار WOLFSSL *ssl;
  CURLcode io_result;      /* result of last BIO cfilter operation */
  CURLcode hs_result;      /* result of handshake */
  صحیح io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(نویسه *buffer, size_t size);

تعریفنوع CURLcode Curl_wssl_ctx_setup_cb(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        تهی *user_data);

تعریفنوع CURLcode Curl_wssl_init_session_reuse_cb(ساختار Curl_cfilter *cf,
                                                 ساختار Curl_easy *data,
                                                 ساختار alpn_spec *alpns,
                                                 ساختار Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_wssl_ctx_init(ساختار wssl_ctx *wctx,
                            ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            ساختار ssl_peer *peer,
                            ثابت ساختار alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            تهی *cb_user_data,
                            تهی *ssl_user_data,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار wssl_ctx *wssl);

#ifdef HAVE_EX_DATA
CURLcode Curl_wssl_cache_session(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ثابت نویسه *ssl_peer_کلید,
                                 ساختار WOLFSSL_SESSION *session,
                                 صحیح ietf_tls_id,
                                 ثابت نویسه *alpn,
                                 unsigned نویسه *quic_tp,
                                 size_t quic_tp_len,
                                 ساختار Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار wssl_ctx *wssl);

تهی Curl_wssl_report_handshake(ساختار Curl_easy *data, ساختار wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
