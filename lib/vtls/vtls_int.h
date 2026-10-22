#ifndef HEADER_CURL_VTLS_INT_H
#define HEADER_CURL_VTLS_INT_H
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

#ifdef USE_SSL

#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "vtls/vtls.h"

સંરચના Curl_ssl;
સંરચના ssl_connect_data;
સંરચના Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#define ALPN_HTTP_1_0_LENGTH 8
#define ALPN_HTTP_1_0        "http/1.0"
#define ALPN_HTTP_1_1_LENGTH 8
#define ALPN_HTTP_1_1        "http/1.1"
#define ALPN_H2_LENGTH       2
#define ALPN_H2              "h2"
#define ALPN_H3_LENGTH       2
#define ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#define ALPN_NAME_MAX      10
#define ALPN_ENTRIES_MAX   3
#define ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

સંરચના alpn_spec {
  અક્ષર entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t ગણતરી; /* number of entries */
};

સંરચના alpn_proto_buf {
  unsigned અક્ષર data[ALPN_PROTO_BUF_MAX];
  પૂર્ણાંક len;
};

CURLcode Curl_alpn_to_proto_buf(સંરચના alpn_proto_buf *buf,
                                અચલ સંરચના alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(સંરચના alpn_proto_buf *buf,
                                અચલ સંરચના alpn_spec *spec);
શૂન્ય Curl_alpn_restrict_to(સંરચના alpn_spec *spec, અચલ અક્ષર *proto);
શૂન્ય Curl_alpn_copy(સંરચના alpn_spec *dest, અચલ સંરચના alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના ssl_connect_data *connssl,
                                  અચલ unsigned અક્ષર *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(અચલ સંરચના alpn_spec *spec, અચલ અક્ષર *proto);

/* enum for the nonblocking SSL connection state machine */
પ્રકારવ્યાખ્યા ગણના {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

પ્રકારવ્યાખ્યા ગણના {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

પ્રકારવ્યાખ્યા ગણના {
  ssl_earlydata_none,
  ssl_earlydata_await,
  ssl_earlydata_sending,
  ssl_earlydata_sent,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#define CURL_SSL_IO_NEED_NONE   0
#define CURL_SSL_IO_NEED_RECV   (1 << 0)
#define CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#define CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
સંરચના ssl_connect_data {
  અચલ સંરચના Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  સંરચના ssl_peer peer;             /* peer the filter talks to */
  અચલ સંરચના alpn_spec *alpn;     /* ALPN to use or NULL for none */
  શૂન્ય *backend;                    /* vtls backend specific props */
  સંરચના cf_call_data call_data;    /* data handle used in current call */
  સંરચના Curl_ssl_session *session; /* TLS session in use or NULL */
  સંરચના curltime handshake_done;   /* time when handshake finished */
  સંરચના {
    અક્ષર *alpn;                     /* ALPN value or NULL */
  } negotiated;
  સંરચના bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  પૂર્ણાંક io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(આગત_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

સંરચના Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned પૂર્ણાંક supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_data;

  પૂર્ણાંક (*init)(શૂન્ય);
  શૂન્ય (*cleanup)(શૂન્ય);

  size_t (*version)(અક્ષર *buffer, size_t size);
  CURLcode (*shut_down)(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(સંરચના Curl_cfilter *cf, અચલ સંરચના Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(સંરચના Curl_easy *data, unsigned અક્ષર *entropy,
                     size_t length);
  bool (*cert_status_request)(શૂન્ય);

  CURLcode (*do_connect)(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                             સંરચના easy_pollset *ps);
  શૂન્ય *(*get_internals)(સંરચના ssl_connect_data *connssl, CURLINFO info);
  શૂન્ય (*close)(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data);
  શૂન્ય (*close_all)(સંરચના Curl_easy *data);

  સંરચના curl_slist *(*engines_list)(સંરચના Curl_easy *data);

  CURLcode (*sha256sum)(અચલ unsigned અક્ષર *આગત, size_t inputlen,
                        unsigned અક્ષર *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                         અક્ષર *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                         અચલ શૂન્ય *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(સંરચના Curl_easy *data, int8_t sockindex,
                                  સંરચના dynbuf *binding);
};

બાહ્ય અચલ સંરચના Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(સંરચના Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના alpn_spec *alpns,
                               સંરચના Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
સંરચના Curl_ssl_session *Curl_ssl_get_cf_session(સંરચના Curl_easy *data,
                                                 અચલ સંરચના Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((સંરચના ssl_connect_data *)(cf)->ctx)->call_data
#endif
