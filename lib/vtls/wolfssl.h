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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_WOLFSSL

#അന്തര്ഭാവ "urldata.h"

#അന്തര്ഭാവ <wolfssl/options.h>

ഘടന alpn_spec;
ഘടന ssl_peer;
ഘടന Curl_ssl_session;

ഘടന WOLFSSL;
ഘടന WOLFSSL_CTX;
ഘടന WOLFSSL_SESSION;

ബാഹ്യ സ്ഥിരം ഘടന Curl_ssl Curl_ssl_wolfssl;

ഘടന wssl_ctx {
  ഘടന WOLFSSL_CTX *ssl_ctx;
  ഘടന WOLFSSL *ssl;
  CURLcode io_result;      /* result of last BIO cfilter operation */
  CURLcode hs_result;      /* result of handshake */
  പൂർണ്ണസംഖ്യ io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(അക്ഷരം *buffer, size_t size);

തരനിർവചനം CURLcode Curl_wssl_ctx_setup_cb(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ശൂന്യം *user_data);

തരനിർവചനം CURLcode Curl_wssl_init_session_reuse_cb(ഘടന Curl_cfilter *cf,
                                                 ഘടന Curl_easy *data,
                                                 ഘടന alpn_spec *alpns,
                                                 ഘടന Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_wssl_ctx_init(ഘടന wssl_ctx *wctx,
                            ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            ഘടന ssl_peer *peer,
                            സ്ഥിരം ഘടന alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            ശൂന്യം *cb_user_data,
                            ശൂന്യം *ssl_user_data,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന wssl_ctx *wssl);

#ifdef HAVE_EX_DATA
CURLcode Curl_wssl_cache_session(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                 ഘടന WOLFSSL_SESSION *session,
                                 പൂർണ്ണസംഖ്യ ietf_tls_id,
                                 സ്ഥിരം അക്ഷരം *alpn,
                                 unsigned അക്ഷരം *quic_tp,
                                 size_t quic_tp_len,
                                 ഘടന Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന wssl_ctx *wssl);

ശൂന്യം Curl_wssl_report_handshake(ഘടന Curl_easy *data, ഘടന wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
