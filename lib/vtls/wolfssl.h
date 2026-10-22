#यदि_अपरिभाषितम् शीर्षकम्_CURL_WOLFSSL_H
#परिभाषय शीर्षकम्_CURL_WOLFSSL_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_WOLFSSL

#अन्तर्भाव "urldata.h"

#अन्तर्भाव <wolfssl/options.h>

संरचना alpn_spec;
संरचना ssl_peer;
संरचना Curl_ssl_session;

संरचना WOLFSSL;
संरचना WOLFSSL_CTX;
संरचना WOLFSSL_SESSION;

बाह्य ध्रुव संरचना Curl_ssl Curl_ssl_wolfssl;

संरचना wssl_ctx {
  संरचना WOLFSSL_CTX *ssl_ctx;
  संरचना WOLFSSL *ssl;
  CURLcode io_परिणाम;      /* result of last BIO cfilter operation */
  CURLcode hs_परिणाम;      /* result of handshake */
  पूर्णाङ्क io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_सज्जता);   /* x509 store has been set up */
  BIT(shutting_अधस्तात्);      /* TLS is being shut down */
};

size_t Curl_wssl_संस्करणम्(वर्ण *बफर, size_t आकार);

प्रकारनिर्देश CURLcode Curl_wssl_ctx_सज्जता_cb(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        शून्य *user_दत्त);

प्रकारनिर्देश CURLcode Curl_wssl_init_session_reuse_cb(संरचना Curl_cfilter *cf,
                                                 संरचना Curl_easy *दत्त,
                                                 संरचना alpn_spec *alpns,
                                                 संरचना Curl_ssl_session *scs,
                                                 bool *do_early_दत्त);

CURLcode Curl_wssl_ctx_init(संरचना wssl_ctx *wctx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            संरचना ssl_peer *peer,
                            ध्रुव संरचना alpn_spec *alpns_requested,
                            Curl_wssl_ctx_सज्जता_cb *cb_सज्जता,
                            शून्य *cb_user_दत्त,
                            शून्य *ssl_user_दत्त,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_सज्जता_x509_store(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना wssl_ctx *wssl);

#यदि_परिभाषितम् HAVE_EX_दत्त
CURLcode Curl_wssl_cache_session(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 संरचना WOLFSSL_SESSION *session,
                                 पूर्णाङ्क ietf_tls_id,
                                 ध्रुव वर्ण *alpn,
                                 अचिह्नित वर्ण *quic_tp,
                                 size_t quic_tp_len,
                                 संरचना Curl_ssl_session **pscs);
#यदि_अन्तः

CURLcode Curl_wssl_verify_pinned(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना wssl_ctx *wssl);

शून्य Curl_wssl_report_handshake(संरचना Curl_easy *दत्त, संरचना wssl_ctx *wssl);

#यदि_अन्तः /* USE_WOLFSSL */
#यदि_अन्तः /* HEADER_CURL_WOLFSSL_H */
