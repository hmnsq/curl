#अगर_अघोषित HEADER_CURL_WOLFSSL_H
#घोषणा HEADER_CURL_WOLFSSL_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_WOLFSSL

#समावेश "urldata.h"

#समावेश <wolfssl/options.h>

काष्ठा alpn_spec;
काष्ठा ssl_peer;
काष्ठा Curl_ssl_session;

काष्ठा WOLFSSL;
काष्ठा WOLFSSL_CTX;
काष्ठा WOLFSSL_SESSION;

बाह्य स्थिर काष्ठा Curl_ssl Curl_ssl_wolfssl;

काष्ठा wssl_ctx {
  काष्ठा WOLFSSL_CTX *ssl_ctx;
  काष्ठा WOLFSSL *ssl;
  CURLcode io_परिणाम;      /* result of last BIO cfilter operation */
  CURLcode hs_परिणाम;      /* result of handshake */
  पूर्णांक io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

माप_प्रकार Curl_wssl_version(अक्षर *बफर, माप_प्रकार आकार);

प्रकार CURLcode Curl_wssl_ctx_setup_cb(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        व्योम *user_तथ्य);

प्रकार CURLcode Curl_wssl_init_session_reuse_cb(काष्ठा Curl_cfilter *cf,
                                                 काष्ठा Curl_easy *तथ्य,
                                                 काष्ठा alpn_spec *alpns,
                                                 काष्ठा Curl_ssl_session *scs,
                                                 bool *do_early_तथ्य);

CURLcode Curl_wssl_ctx_init(काष्ठा wssl_ctx *wctx,
                            काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा ssl_peer *peer,
                            स्थिर काष्ठा alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            व्योम *cb_user_तथ्य,
                            व्योम *ssl_user_तथ्य,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा wssl_ctx *wssl);

#अगर_घोषित HAVE_EX_तथ्य
CURLcode Curl_wssl_cache_session(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *ssl_peer_कुंजी,
                                 काष्ठा WOLFSSL_SESSION *session,
                                 पूर्णांक ietf_tls_id,
                                 स्थिर अक्षर *alpn,
                                 अचिन्हित अक्षर *quic_tp,
                                 माप_प्रकार quic_tp_len,
                                 काष्ठा Curl_ssl_session **pscs);
#पूर्ण_अगर

CURLcode Curl_wssl_verify_pinned(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा wssl_ctx *wssl);

व्योम Curl_wssl_report_handshake(काष्ठा Curl_easy *तथ्य, काष्ठा wssl_ctx *wssl);

#पूर्ण_अगर /* USE_WOLFSSL */
#पूर्ण_अगर /* HEADER_CURL_WOLFSSL_H */
