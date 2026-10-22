#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VTLS_INT_H
#నిర్వచించు శీర్షిక_CURL_VTLS_INT_H
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

#నిర్వచితమైతే USE_SSL

#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "vtls/vtls.h"

నిర్మాణం Curl_ssl;
నిర్మాణం ssl_connect_దత్తాంశం;
నిర్మాణం Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#నిర్వచించు ALPN_HTTP_1_0_నిడివి 8
#నిర్వచించు ALPN_HTTP_1_0        "http/1.0"
#నిర్వచించు ALPN_HTTP_1_1_నిడివి 8
#నిర్వచించు ALPN_HTTP_1_1        "http/1.1"
#నిర్వచించు ALPN_H2_నిడివి       2
#నిర్వచించు ALPN_H2              "h2"
#నిర్వచించు ALPN_H3_నిడివి       2
#నిర్వచించు ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#నిర్వచించు ALPN_NAME_MAX      10
#నిర్వచించు ALPN_ENTRIES_MAX   3
#నిర్వచించు ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

నిర్మాణం alpn_spec {
  అక్షరం entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t లెక్క; /* number of entries */
};

నిర్మాణం alpn_proto_buf {
  గుర్తులేని అక్షరం దత్తాంశం[ALPN_PROTO_BUF_MAX];
  పూర్ణాంకం len;
};

CURLcode Curl_alpn_గమ్యం_proto_buf(నిర్మాణం alpn_proto_buf *buf,
                                స్థిరరాశి నిర్మాణం alpn_spec *spec);
CURLcode Curl_alpn_గమ్యం_proto_str(నిర్మాణం alpn_proto_buf *buf,
                                స్థిరరాశి నిర్మాణం alpn_spec *spec);
శూన్య Curl_alpn_restrict_గమ్యం(నిర్మాణం alpn_spec *spec, స్థిరరాశి అక్షరం *proto);
శూన్య Curl_alpn_copy(నిర్మాణం alpn_spec *dest, స్థిరరాశి నిర్మాణం alpn_spec *src);

CURLcode Curl_alpn_సమితి_negotiated(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                                  స్థిరరాశి గుర్తులేని అక్షరం *proto,
                                  size_t proto_len);

bool Curl_alpn_కలిగియుంది_proto(స్థిరరాశి నిర్మాణం alpn_spec *spec, స్థిరరాశి అక్షరం *proto);

/* enum for the nonblocking SSL connection state machine */
రకనిర్వచనం గణన {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_పూర్తి
} ssl_connect_state;

రకనిర్వచనం గణన {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

రకనిర్వచనం గణన {
  ssl_earlydata_none,
  ssl_earlydata_await,
  ssl_earlydata_sending,
  ssl_earlydata_పంపిన,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#నిర్వచించు CURL_SSL_IO_NEED_NONE   0
#నిర్వచించు CURL_SSL_IO_NEED_RECV   (1 << 0)
#నిర్వచించు CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#నిర్వచించు CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
నిర్మాణం ssl_connect_దత్తాంశం {
  స్థిరరాశి నిర్మాణం Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  నిర్మాణం ssl_peer peer;             /* peer the filter talks to */
  స్థిరరాశి నిర్మాణం alpn_spec *alpn;     /* ALPN to use or NULL for none */
  శూన్య *backend;                    /* vtls backend specific props */
  నిర్మాణం cf_call_దత్తాంశం call_దత్తాంశం;    /* data handle used in current call */
  నిర్మాణం Curl_ssl_session *session; /* TLS session in use or NULL */
  నిర్మాణం curltime handshake_పూర్తి;   /* time when handshake finished */
  నిర్మాణం {
    అక్షరం *alpn;                     /* ALPN value or NULL */
  } negotiated;
  నిర్మాణం bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  పూర్ణాంకం io_need;                      /* TLS signals special SEND/RECV needs */
  బిట్టు(peer_closed);                 /* peer has closed connection */
  బిట్టు(prefs_checked);               /* SSL preferences have been checked */
  బిట్టు(ఆదానం_pending);               /* data for SSL_read() may be available */
  బిట్టు(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

నిర్మాణం Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  గుర్తులేని పూర్ణాంకం supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_దత్తాంశం;

  పూర్ణాంకం (*init)(శూన్య);
  శూన్య (*cleanup)(శూన్య);

  size_t (*సంస్కరణ_2)(అక్షరం *బఫరు, size_t కొలత);
  CURLcode (*shut_కిందికి)(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                        bool send_shutdown, bool *పూర్తి);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*దత్తాంశం_pending)(నిర్మాణం Curl_cfilter *cf, స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని అక్షరం *entropy,
                     size_t నిడివి);
  bool (*cert_స్థితి_request)(శూన్య);

  CURLcode (*do_connect)(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                         bool *పూర్తి);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps);
  శూన్య *(*get_internals)(నిర్మాణం ssl_connect_దత్తాంశం *connssl, CURLINFO info);
  శూన్య (*close)(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం);
  శూన్య (*close_all)(నిర్మాణం Curl_easy *దత్తాంశం);

  నిర్మాణం curl_slist *(*engines_list)(నిర్మాణం Curl_easy *దత్తాంశం);

  CURLcode (*sha256sum)(స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, size_t inputlen,
                        గుర్తులేని అక్షరం *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                         అక్షరం *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                         స్థిరరాశి శూన్య *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                                  నిర్మాణం dynbuf *binding);
};

బాహ్య స్థిరరాశి నిర్మాణం Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(నిర్మాణం Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం alpn_spec *alpns,
                               నిర్మాణం Curl_ssl_session *scs,
                               bool *do_early_దత్తాంశం, bool early_దత్తాంశం_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
నిర్మాణం Curl_ssl_session *Curl_ssl_get_cf_session(నిర్మాణం Curl_easy *దత్తాంశం,
                                                 స్థిరరాశి నిర్మాణం Curl_cftype *cft,
                                                 int8_t sockindex);

#అయితే_ముగింపు /* USE_SSL */

#అయితే_ముగింపు /* HEADER_CURL_VTLS_INT_H */

#నిర్వచితమైతే USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#నిర్వచనం_రద్దు CF_CTX_CALL_దత్తాంశం
#నిర్వచించు CF_CTX_CALL_దత్తాంశం(cf) ((నిర్మాణం ssl_connect_దత్తాంశం *)(cf)->ctx)->call_దత్తాంశం
#అయితే_ముగింపు
