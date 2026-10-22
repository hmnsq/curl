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
#include "curl_setup.h"

#ifdef USE_WOLFSSL

#include "urldata.h"

#include <wolfssl/options.h>

sa_nracanaa alpn_spec;
sa_nracanaa ssl_peer;
sa_nracanaa Curl_ssl_session;

sa_nracanaa WOLFSSL;
sa_nracanaa WOLFSSL_CTX;
sa_nracanaa WOLFSSL_SESSION;

extern const sa_nracanaa Curl_ssl Curl_ssl_wolfssl;

sa_nracanaa wssl_ctx {
  sa_nracanaa WOLFSSL_CTX *ssl_ctx;
  sa_nracanaa WOLFSSL *ssl;
  CURLcode io_result;      /* result of last BIO cfilter operation */
  CURLcode hs_result;      /* result of handshake */
  จำนวนเต็ม io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(var_na *buffer, size_t size);

typedef CURLcode Curl_wssl_ctx_setup_cb(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        void *user_data);

typedef CURLcode Curl_wssl_init_session_reuse_cb(sa_nracanaa Curl_cfilter *cf,
                                                 sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa alpn_spec *alpns,
                                                 sa_nracanaa Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_wssl_ctx_init(sa_nracanaa wssl_ctx *wctx,
                            sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa ssl_peer *peer,
                            const sa_nracanaa alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            void *cb_user_data,
                            void *ssl_user_data,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa wssl_ctx *wssl);

#ifdef HAVE_EX_DATA
CURLcode Curl_wssl_cache_session(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 const var_na *ssl_peer_key,
                                 sa_nracanaa WOLFSSL_SESSION *session,
                                 จำนวนเต็ม ietf_tls_id,
                                 const var_na *alpn,
                                 unsigned var_na *quic_tp,
                                 size_t quic_tp_len,
                                 sa_nracanaa Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa wssl_ctx *wssl);

void Curl_wssl_report_handshake(sa_nracanaa Curl_easy *data, sa_nracanaa wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
