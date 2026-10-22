#నిర్వచితం_కాకపోతే శీర్షిక_CURL_WOLFSSL_H
#నిర్వచించు శీర్షిక_CURL_WOLFSSL_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_WOLFSSL

#అన్తర్భావ "urldata.h"

#అన్తర్భావ <wolfssl/options.h>

నిర్మాణం alpn_spec;
నిర్మాణం ssl_peer;
నిర్మాణం Curl_ssl_session;

నిర్మాణం WOLFSSL;
నిర్మాణం WOLFSSL_CTX;
నిర్మాణం WOLFSSL_SESSION;

బాహ్య స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_wolfssl;

నిర్మాణం wssl_ctx {
  నిర్మాణం WOLFSSL_CTX *ssl_ctx;
  నిర్మాణం WOLFSSL *ssl;
  CURLcode io_ఫలితం;      /* result of last BIO cfilter operation */
  CURLcode hs_ఫలితం;      /* result of handshake */
  పూర్ణాంకం io_send_blocked_len; /* length of last BIO write that EAGAIN-ed */
  బిట్టు(x509_store_అమరిక);   /* x509 store has been set up */
  బిట్టు(shutting_కిందికి);      /* TLS is being shut down */
};

size_t Curl_wssl_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత);

రకనిర్వచనం CURLcode Curl_wssl_ctx_అమరిక_cb(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        శూన్య *user_దత్తాంశం);

రకనిర్వచనం CURLcode Curl_wssl_init_session_reuse_cb(నిర్మాణం Curl_cfilter *cf,
                                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                                 నిర్మాణం alpn_spec *alpns,
                                                 నిర్మాణం Curl_ssl_session *scs,
                                                 bool *do_early_దత్తాంశం);

CURLcode Curl_wssl_ctx_init(నిర్మాణం wssl_ctx *wctx,
                            నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ssl_peer *peer,
                            స్థిరరాశి నిర్మాణం alpn_spec *alpns_requested,
                            Curl_wssl_ctx_అమరిక_cb *cb_అమరిక,
                            శూన్య *cb_user_దత్తాంశం,
                            శూన్య *ssl_user_దత్తాంశం,
                            Curl_wssl_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_wssl_అమరిక_x509_store(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం wssl_ctx *wssl);

#నిర్వచితమైతే HAVE_EX_దత్తాంశం
CURLcode Curl_wssl_cache_session(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                 నిర్మాణం WOLFSSL_SESSION *session,
                                 పూర్ణాంకం ietf_tls_id,
                                 స్థిరరాశి అక్షరం *alpn,
                                 గుర్తులేని అక్షరం *quic_tp,
                                 size_t quic_tp_len,
                                 నిర్మాణం Curl_ssl_session **pscs);
#అయితే_ముగింపు

CURLcode Curl_wssl_verify_pinned(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం wssl_ctx *wssl);

శూన్య Curl_wssl_report_handshake(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం wssl_ctx *wssl);

#అయితే_ముగింపు /* USE_WOLFSSL */
#అయితే_ముగింపు /* HEADER_CURL_WOLFSSL_H */
