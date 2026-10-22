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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_WOLFSSL

#অন্তর্ভাব "urldata.h"

#অন্তর্ভাব <wolfssl/options.h>

কাঠামো alpn_spec;
কাঠামো ssl_peer;
কাঠামো Curl_ssl_session;

কাঠামো WOLFSSL;
কাঠামো WOLFSSL_CTX;
কাঠামো WOLFSSL_SESSION;

বাহ্যিক ধ্রুবক কাঠামো Curl_ssl Curl_ssl_wolfssl;

কাঠামো wssl_ctx {
  কাঠামো WOLFSSL_CTX *ssl_ctx;
  কাঠামো WOLFSSL *ssl;
  CURLcode io_ফলাফল;      /* result of last BIO cfilter operation */
  CURLcode hs_ফলাফল;      /* result of handshake */
  পূর্ণসংখ্যা io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  BIT(x509_store_setup);   /* x509 store has been set up */
  BIT(shutting_down);      /* TLS is being shut down */
};

size_t Curl_wssl_version(অক্ষর *বাফার, size_t আকার_2);

প্রকারলেখ CURLcode Curl_wssl_ctx_setup_cb(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        শূন্য *user_তথ্য);

প্রকারলেখ CURLcode Curl_wssl_init_session_reuse_cb(কাঠামো Curl_cfilter *cf,
                                                 কাঠামো Curl_easy *তথ্য,
                                                 কাঠামো alpn_spec *alpns,
                                                 কাঠামো Curl_ssl_session *scs,
                                                 bool *do_early_তথ্য);

CURLcode Curl_wssl_ctx_init(কাঠামো wssl_ctx *wctx,
                            কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো ssl_peer *peer,
                            ধ্রুবক কাঠামো alpn_spec *alpns_requested,
                            Curl_wssl_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_তথ্য,
                            শূন্য *ssl_user_তথ্য,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_setup_x509_store(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো wssl_ctx *wssl);

#ifdef HAVE_EX_তথ্য
CURLcode Curl_wssl_cache_session(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                 কাঠামো WOLFSSL_SESSION *session,
                                 পূর্ণসংখ্যা ietf_tls_id,
                                 ধ্রুবক অক্ষর *alpn,
                                 unsigned অক্ষর *quic_tp,
                                 size_t quic_tp_len,
                                 কাঠামো Curl_ssl_session **pscs);
#endif

CURLcode Curl_wssl_verify_pinned(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো wssl_ctx *wssl);

শূন্য Curl_wssl_report_handshake(কাঠামো Curl_easy *তথ্য, কাঠামো wssl_ctx *wssl);

#endif /* USE_WOLFSSL */
#endif /* HEADER_CURL_WOLFSSL_H */
