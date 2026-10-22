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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_WOLFSSL

#અન્તર્ભાવ "urldata.h"

#અન્તર્ભાવ <wolfssl/options.h>

સંરચના alpn_spec;
સંરચના ssl_peer;
સંરચના Curl_ssl_session;

સંરચના WOLFSSL;
સંરચના WOLFSSL_CTX;
સંરચના WOLFSSL_SESSION;

બાહ્ય અચલ સંરચના Curl_ssl Curl_ssl_wolfssl;

સંરચના wssl_ctx {
  સંરચના WOLFSSL_CTX *ssl_ctx;
  સંરચના WOLFSSL *ssl;
  CURLcode io_result;      /* result of last BIO cfilter operation */
  CURLcode hs_result;      /* result of handshake */
  પૂર્ણાંક io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(અક્ષર *buffer, size_t size);

પ્રકારવ્યાખ્યા CURLcode Curl_wssl_ctx_setup_cb(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        શૂન્ય *user_data);

પ્રકારવ્યાખ્યા CURLcode Curl_wssl_init_session_reuse_cb(સંરચના Curl_cfilter *cf,
                                                 સંરચના Curl_easy *data,
                                                 સંરચના alpn_spec *alpns,
                                                 સંરચના Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_wssl_ctx_init(સંરચના wssl_ctx *wctx,
                            સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            સંરચના ssl_peer *peer,
                            અચલ સંરચના alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            શૂન્ય *cb_user_data,
                            શૂન્ય *ssl_user_data,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના wssl_ctx *wssl);

#ifdef HAVE_EX_DATA
CURLcode Curl_wssl_cache_session(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અચલ અક્ષર *ssl_peer_ચાવી,
                                 સંરચના WOLFSSL_SESSION *session,
                                 પૂર્ણાંક ietf_tls_id,
                                 અચલ અક્ષર *alpn,
                                 unsigned અક્ષર *quic_tp,
                                 size_t quic_tp_len,
                                 સંરચના Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના wssl_ctx *wssl);

શૂન્ય Curl_wssl_report_handshake(સંરચના Curl_easy *data, સંરચના wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
