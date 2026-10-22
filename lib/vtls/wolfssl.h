#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_WOLFSSL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_WOLFSSL_H
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WOLFSSL

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>

ਬਣਤਰ alpn_spec;
ਬਣਤਰ ssl_peer;
ਬਣਤਰ Curl_ssl_session;

ਬਣਤਰ WOLFSSL;
ਬਣਤਰ WOLFSSL_CTX;
ਬਣਤਰ WOLFSSL_SESSION;

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_wolfssl;

ਬਣਤਰ wssl_ctx {
  ਬਣਤਰ WOLFSSL_CTX *ssl_ctx;
  ਬਣਤਰ WOLFSSL *ssl;
  CURLcode io_ਨਤੀਜਾ;      /* result of last BIO cfilter operation */
  CURLcode hs_ਨਤੀਜਾ;      /* result of handshake */
  ਪੂਰਨ io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(x509_store_ਤਿਆਰੀ);   /* x509 store has been set up */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutting_ਥੱਲੇ);      /* TLS is being shut down */
};

size_t Curl_wssl_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_wssl_ctx_ਤਿਆਰੀ_cb(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਖਾਲੀ *user_ਅੰਕੜੇ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_wssl_init_session_reuse_cb(ਬਣਤਰ Curl_cfilter *cf,
                                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਬਣਤਰ alpn_spec *alpns,
                                                 ਬਣਤਰ Curl_ssl_session *scs,
                                                 bool *do_early_ਅੰਕੜੇ);

CURLcode Curl_wssl_ctx_init(ਬਣਤਰ wssl_ctx *wctx,
                            ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ssl_peer *peer,
                            ਅਚਲ ਬਣਤਰ alpn_spec *alpns_requested,
                            Curl_wssl_ctx_ਤਿਆਰੀ_cb *cb_ਤਿਆਰੀ,
                            ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                            ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_ਤਿਆਰੀ_x509_store(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ wssl_ctx *wssl);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_EX_ਅੰਕੜੇ
CURLcode Curl_wssl_cache_session(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                 ਬਣਤਰ WOLFSSL_SESSION *session,
                                 ਪੂਰਨ ietf_tls_id,
                                 ਅਚਲ ਅੱਖਰ *alpn,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp,
                                 size_t quic_tp_len,
                                 ਬਣਤਰ Curl_ssl_session **pscs);
#ਜੇ_ਅੰਤ

CURLcode Curl_wssl_verify_pinned(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ wssl_ctx *wssl);

ਖਾਲੀ Curl_wssl_report_handshake(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ wssl_ctx *wssl);

#ਜੇ_ਅੰਤ /* USE_WOLFSSL */
#ਜੇ_ਅੰਤ /* HEADER_CURL_WOLFSSL_H */
