#ifndef HEADER_CURL_GTLS_H
#define HEADER_CURL_GTLS_H
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

#ifdef USE_GNUTLS

#अन्तर्भाव <gnutls/gnutls.h>

#अन्तर्भाव "curlx/timeval.h"

संरचना Curl_easy;
संरचना Curl_cfilter;
संरचना alpn_spec;
संरचना ssl_filter_config;
संरचना ssl_easy_config;
संरचना ssl_peer;
संरचना ssl_connect_data;
संरचना Curl_ssl_session;

संरचना gtls_shared_creds {
  gnutls_certificate_credentials_t creds;
  वर्ण *CAfile; /* CAfile path used to generate X509 store */
  संरचना curltime time; /* when the shared creds was created */
  size_t refcount;
  BIT(trust_setup); /* x509 anchors + CRLs have been set up */
};

CURLcode Curl_gtls_shared_creds_create(संरचना Curl_easy *data,
                                       संरचना gtls_shared_creds **pcreds);
CURLcode Curl_gtls_shared_creds_up_ref(संरचना gtls_shared_creds *creds);
शून्य Curl_gtls_shared_creds_free(संरचना gtls_shared_creds **pcreds);

संरचना gtls_ctx {
  gnutls_session_t session;
  संरचना gtls_shared_creds *shared_creds;
  CURLcode io_result; /* result of last IO cfilter operation */
  BIT(sent_shutdown);
};

size_t Curl_gtls_version(वर्ण *buffer, size_t size);

प्रकारनिर्देश CURLcode Curl_gtls_ctx_setup_cb(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        शून्य *user_data);

प्रकारनिर्देश CURLcode Curl_gtls_init_session_reuse_cb(संरचना Curl_cfilter *cf,
                                                 संरचना Curl_easy *data,
                                                 संरचना alpn_spec *alpns,
                                                 संरचना Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_gtls_ctx_init(संरचना gtls_ctx *gctx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            संरचना ssl_peer *peer,
                            ध्रुव संरचना alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            शून्य *cb_user_data,
                            शून्य *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb);

CURLcode Curl_gtls_client_trust_setup(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना gtls_ctx *gtls);

CURLcode Curl_gtls_verifyserver(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                gnutls_session_t session,
                                संरचना ssl_filter_config *config,
                                संरचना ssl_easy_config *ssl_config,
                                संरचना ssl_peer *peer,
                                ध्रुव वर्ण *pinned_किल्ली);

/* Extract TLS session and place in cache, if configured. Return
 * a copy of the session if desired. */
CURLcode Curl_gtls_cache_session(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 ध्रुव वर्ण *ssl_peer_किल्ली,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ध्रुव वर्ण *alpn,
                                 unsigned वर्ण *quic_tp,
                                 size_t quic_tp_len,
                                 संरचना Curl_ssl_session **psession);

/* Report properties of a successful handshake */
शून्य Curl_gtls_report_handshake(संरचना Curl_easy *data, संरचना gtls_ctx *gctx);

बाह्य ध्रुव संरचना Curl_ssl Curl_ssl_gnutls;

#endif /* USE_GNUTLS */
#endif /* HEADER_CURL_GTLS_H */
