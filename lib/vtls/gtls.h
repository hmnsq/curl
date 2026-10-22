#यदि_अपरिभाषितम् शीर्षकम्_CURL_GTLS_H
#परिभाषय शीर्षकम्_CURL_GTLS_H
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

#यदि_परिभाषितम् USE_GNUTLS

#अन्तर्भाव <gnutls/gnutls.h>

#अन्तर्भाव "curlx/timeval.h"

संरचना Curl_easy;
संरचना Curl_cfilter;
संरचना alpn_spec;
संरचना ssl_filter_config;
संरचना ssl_easy_config;
संरचना ssl_peer;
संरचना ssl_connect_दत्त;
संरचना Curl_ssl_session;

संरचना gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  वर्ण *CAfile; /* CAfile path used to generate X509 store */
  संरचना curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_सज्जता); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(संरचना Curl_easy *दत्त,
                                       संरचना gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_ऊर्ध्वम्_ref(संरचना gtls_shared_creds *creds);
शून्य Curl_gtls_shared_creds_free(संरचना gtls_shared_creds **pcreds);

संरचना gtls_ctx {
  gnutls_session_t session;
  संरचना gtls_shared_creds *shared_creds;
  CURLcode io_परिणाम; /* result of last IO cfilter operation */
  BIT(प्रेषितम्_shutdown);
};

size_t Curl_gtls_संस्करणम्(वर्ण *बफर, size_t आकार);

प्रकारनिर्देश CURLcode Curl_gtls_ctx_सज्जता_cb(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        शून्य *user_दत्त);

प्रकारनिर्देश CURLcode Curl_gtls_init_session_reuse_cb(संरचना Curl_cfilter *cf,
                                                 संरचना Curl_easy *दत्त,
                                                 संरचना alpn_spec *alpns,
                                                 संरचना Curl_ssl_session *scs,
                                                 bool *do_early_दत्त);

CURLcode Curl_gtls_ctx_init(संरचना gtls_ctx *gctx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            संरचना ssl_peer *peer,
                            ध्रुव संरचना alpn_spec *alpns_requested,
                            Curl_gtls_ctx_सज्जता_cb *cb_सज्जता,
                            शून्य *cb_user_दत्त,
                            शून्य *ssl_user_दत्त,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_सज्जता(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                gnutls_session_t session,
                                संरचना ssl_filter_config *config,
                                संरचना ssl_easy_config *ssl_config,
                                संरचना ssl_peer *peer,
                                ध्रुव वर्ण *pinned_कुञ्चिका);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ध्रुव वर्ण *alpn,
                                 अचिह्नित वर्ण *quic_tp,
                                 size_t quic_tp_len,
                                 संरचना Curl_ssl_session **psession);

/* Report properties of a successful handshake */
शून्य Curl_gtls_report_handshake(संरचना Curl_easy *दत्त, संरचना gtls_ctx *gctx);

बाह्य ध्रुव संरचना Curl_ssl Curl_ssl_gnutls;

#यदि_अन्तः /* USE_GNUTLS */
#यदि_अन्तः /* HEADER_CURL_GTLS_H */
