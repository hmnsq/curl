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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_WOLFSSL

#அந்தர்பாவ "urldata.h"

#அந்தர்பாவ <wolfssl/options.h>

கட்டமைப்பு alpn_spec;
கட்டமைப்பு ssl_peer;
கட்டமைப்பு Curl_ssl_session;

கட்டமைப்பு WOLFSSL;
கட்டமைப்பு WOLFSSL_CTX;
கட்டமைப்பு WOLFSSL_SESSION;

வெளி மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_wolfssl;

கட்டமைப்பு wssl_ctx {
  கட்டமைப்பு WOLFSSL_CTX *ssl_ctx;
  கட்டமைப்பு WOLFSSL *ssl;
  CURLcode io_result;      /* result of last BIO cfilter operation */
  CURLcode hs_result;      /* result of handshake */
  முழு io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(எழுத்து *buffer, size_t size);

வகைவரை CURLcode Curl_wssl_ctx_setup_cb(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        வெறுமை *user_data);

வகைவரை CURLcode Curl_wssl_init_session_reuse_cb(கட்டமைப்பு Curl_cfilter *cf,
                                                 கட்டமைப்பு Curl_easy *data,
                                                 கட்டமைப்பு alpn_spec *alpns,
                                                 கட்டமைப்பு Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_wssl_ctx_init(கட்டமைப்பு wssl_ctx *wctx,
                            கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு ssl_peer *peer,
                            மாறிலி கட்டமைப்பு alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            வெறுமை *cb_user_data,
                            வெறுமை *ssl_user_data,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு wssl_ctx *wssl);

#ifdef HAVE_EX_DATA
CURLcode Curl_wssl_cache_session(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *ssl_peer_சாவி,
                                 கட்டமைப்பு WOLFSSL_SESSION *session,
                                 முழு ietf_tls_id,
                                 மாறிலி எழுத்து *alpn,
                                 unsigned எழுத்து *quic_tp,
                                 size_t quic_tp_len,
                                 கட்டமைப்பு Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு wssl_ctx *wssl);

வெறுமை Curl_wssl_report_handshake(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
