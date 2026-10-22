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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_WOLFSSL

#অন্তর্ভাৱ "urldata.h"

#অন্তর্ভাৱ <wolfssl/options.h>

সংরচনা alpn_spec;
সংরচনা ssl_peer;
সংরচনা Curl_ssl_session;

সংরচনা WOLFSSL;
সংরচনা WOLFSSL_CTX;
সংরচনা WOLFSSL_SESSION;

বাহ্য ধ্রুৱ সংরচনা Curl_ssl Curl_ssl_wolfssl;

সংরচনা wssl_ctx {
  সংরচনা WOLFSSL_CTX *ssl_ctx;
  সংরচনা WOLFSSL *ssl;
  CURLcode io_result;      /* result of last BIO cfilter operation */
  CURLcode hs_result;      /* result of handshake */
  পূর্ণাঙ্ক io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(ৱর্ণ *buffer, size_t size);

প্রকারনির্দেশ CURLcode Curl_wssl_ctx_setup_cb(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        শূন্য *user_data);

প্রকারনির্দেশ CURLcode Curl_wssl_init_session_reuse_cb(সংরচনা Curl_cfilter *cf,
                                                 সংরচনা Curl_easy *data,
                                                 সংরচনা alpn_spec *alpns,
                                                 সংরচনা Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_wssl_ctx_init(সংরচনা wssl_ctx *wctx,
                            সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            সংরচনা ssl_peer *peer,
                            ধ্রুৱ সংরচনা alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_data,
                            শূন্য *ssl_user_data,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা wssl_ctx *wssl);

#ifdef HAVE_EX_DATA
CURLcode Curl_wssl_cache_session(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ধ্রুৱ ৱর্ণ *ssl_peer_চাবি,
                                 সংরচনা WOLFSSL_SESSION *session,
                                 পূর্ণাঙ্ক ietf_tls_id,
                                 ধ্রুৱ ৱর্ণ *alpn,
                                 unsigned ৱর্ণ *quic_tp,
                                 size_t quic_tp_len,
                                 সংরচনা Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা wssl_ctx *wssl);

শূন্য Curl_wssl_report_handshake(সংরচনা Curl_easy *data, সংরচনা wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
