#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_WOLFSSL_H
#وصف_ڪريو سرو_CURL_WOLFSSL_H
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

#جيڪڏهن_وصف_ٿيل USE_WOLFSSL

#ضمن "urldata.h"

#ضمن <wolfssl/options.h>

بنية alpn_spec;
بنية ssl_peer;
بنية Curl_ssl_session;

بنية WOLFSSL;
بنية WOLFSSL_CTX;
بنية WOLFSSL_SESSION;

خارجي ثابت بنية Curl_ssl Curl_ssl_wolfssl;

بنية wssl_ctx {
  بنية WOLFSSL_CTX *ssl_ctx;
  بنية WOLFSSL *ssl;
  CURLcode io_نتيجو;      /* result of last BIO cfilter operation */
  CURLcode hs_نتيجو;      /* result of handshake */
  صحيح io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  ٻٽو_انگ(x509_store_تياري);   /* x509 store has been set up */
  ٻٽو_انگ(shutting_هيٺ_ڏانهن);      /* TLS is being shut down */
};

size_t Curl_wssl_نسخو(حرف *بفر, size_t سائيز);

تعريفنوع CURLcode Curl_wssl_ctx_تياري_cb(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        فراغ *user_معلومات);

تعريفنوع CURLcode Curl_wssl_init_session_reuse_cb(بنية Curl_cfilter *cf,
                                                 بنية Curl_easy *معلومات,
                                                 بنية alpn_spec *alpns,
                                                 بنية Curl_ssl_session *scs,
                                                 bool *do_early_معلومات);

CURLcode Curl_wssl_ctx_init(بنية wssl_ctx *wctx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_wssl_ctx_تياري_cb *cb_تياري,
                            فراغ *cb_user_معلومات,
                            فراغ *ssl_user_معلومات,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_تياري_x509_store(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية wssl_ctx *wssl);

#جيڪڏهن_وصف_ٿيل HAVE_EX_معلومات
CURLcode Curl_wssl_cache_session(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 ثابت حرف *ssl_peer_ڪنجي,
                                 بنية WOLFSSL_SESSION *session,
                                 صحيح ietf_tls_id,
                                 ثابت حرف *alpn,
                                 بنانشان حرف *quic_tp,
                                 size_t quic_tp_len,
                                 بنية Curl_ssl_session **pscs);
#جيڪڏهن_پورو

CURLcode Curl_wssl_verify_pinned(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية wssl_ctx *wssl);

فراغ Curl_wssl_report_handshake(بنية Curl_easy *معلومات, بنية wssl_ctx *wssl);

#جيڪڏهن_پورو /* USE_WOLFSSL */
#جيڪڏهن_پورو /* HEADER_CURL_WOLFSSL_H */
