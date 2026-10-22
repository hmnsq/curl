#اگر_غیر_تعریف_شدہ سرخی_CURL_WOLFSSL_H
#تعریف_کرو سرخی_CURL_WOLFSSL_H
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

#اگر_تعریف_شدہ USE_WOLFSSL

#شامل "urldata.h"

#شامل <wolfssl/options.h>

ساخت alpn_spec;
ساخت ssl_peer;
ساخت Curl_ssl_session;

ساخت WOLFSSL;
ساخت WOLFSSL_CTX;
ساخت WOLFSSL_SESSION;

خارجی ثابت ساخت Curl_ssl Curl_ssl_wolfssl;

ساخت wssl_ctx {
  ساخت WOLFSSL_CTX *ssl_ctx;
  ساخت WOLFSSL *ssl;
  CURLcode io_نتیجہ;      /* result of last BIO cfilter operation */
  CURLcode hs_نتیجہ;      /* result of handshake */
  صحیح io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  ثنائی_ہندسہ(x509_store_تیاری);   /* x509 store has been set up */
  ثنائی_ہندسہ(shutting_نیچے_کو);      /* TLS is being shut down */
};

size_t Curl_wssl_نسخہ(حرف *بفر, size_t سائز);

قسمتعریف CURLcode Curl_wssl_ctx_تیاری_cb(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        خالی *user_ڈیٹا);

قسمتعریف CURLcode Curl_wssl_init_session_reuse_cb(ساخت Curl_cfilter *cf,
                                                 ساخت Curl_easy *ڈیٹا,
                                                 ساخت alpn_spec *alpns,
                                                 ساخت Curl_ssl_session *scs,
                                                 bool *do_early_ڈیٹا);

CURLcode Curl_wssl_ctx_init(ساخت wssl_ctx *wctx,
                            ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت ssl_peer *peer,
                            ثابت ساخت alpn_spec *alpns_requested,
                            Curl_wssl_ctx_تیاری_cb *cb_تیاری,
                            خالی *cb_user_ڈیٹا,
                            خالی *ssl_user_ڈیٹا,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_تیاری_x509_store(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت wssl_ctx *wssl);

#اگر_تعریف_شدہ HAVE_EX_ڈیٹا
CURLcode Curl_wssl_cache_session(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *ssl_peer_کلید,
                                 ساخت WOLFSSL_SESSION *session,
                                 صحیح ietf_tls_id,
                                 ثابت حرف *alpn,
                                 غیرعلامتی حرف *quic_tp,
                                 size_t quic_tp_len,
                                 ساخت Curl_ssl_session **pscs);
#اگر_ختم

CURLcode Curl_wssl_verify_pinned(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت wssl_ctx *wssl);

خالی Curl_wssl_report_handshake(ساخت Curl_easy *ڈیٹا, ساخت wssl_ctx *wssl);

#اگر_ختم /* USE_WOLFSSL */
#اگر_ختم /* HEADER_CURL_WOLFSSL_H */
