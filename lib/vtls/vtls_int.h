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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_SSL

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"

ಸಂರಚನಾ Curl_ssl;
ಸಂರಚನಾ ssl_connect_data;
ಸಂರಚನಾ Curl_ssl_session;

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

ಸಂರಚನಾ alpn_spec {
  ವರ್ಣ entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t ಎಣಿಕೆ; /* number of entries */
};

ಸಂರಚನಾ alpn_proto_buf {
  unsigned ವರ್ಣ data[ALPN_PROTO_BUF_MAX];
  ಪೂರ್ಣಾಙ್ಕ len;
};

CURLcode Curl_alpn_to_proto_buf(ಸಂರಚನಾ alpn_proto_buf *buf,
                                ಧ್ರುವ ಸಂರಚನಾ alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(ಸಂರಚನಾ alpn_proto_buf *buf,
                                ಧ್ರುವ ಸಂರಚನಾ alpn_spec *spec);
ಶೂನ್ಯ Curl_alpn_restrict_to(ಸಂರಚನಾ alpn_spec *spec, ಧ್ರುವ ವರ್ಣ *proto);
ಶೂನ್ಯ Curl_alpn_copy(ಸಂರಚನಾ alpn_spec *dest, ಧ್ರುವ ಸಂರಚನಾ alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssl_connect_data *connssl,
                                  ಧ್ರುವ unsigned ವರ್ಣ *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(ಧ್ರುವ ಸಂರಚನಾ alpn_spec *spec, ಧ್ರುವ ವರ್ಣ *proto);

/* enum for the nonblocking SSL connection state machine */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
ಸಂರಚನಾ ssl_connect_data {
  ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  ಸಂರಚನಾ ssl_peer peer;             /* peer the filter talks to */
  ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpn;     /* ALPN to use or NULL for none */
  ಶೂನ್ಯ *backend;                    /* vtls backend specific props */
  ಸಂರಚನಾ cf_call_data call_data;    /* data handle used in current call */
  ಸಂರಚನಾ Curl_ssl_session *session; /* TLS session in use or NULL */
  ಸಂರಚನಾ curltime handshake_done;   /* time when handshake finished */
  ಸಂರಚನಾ {
    ವರ್ಣ *alpn;                     /* ALPN value or NULL */
  } negotiated;
  ಸಂರಚನಾ bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  ಪೂರ್ಣಾಙ್ಕ io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(ಒಳಹರಿವು_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

ಸಂರಚನಾ Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned ಪೂರ್ಣಾಙ್ಕ supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_data;

  ಪೂರ್ಣಾಙ್ಕ (*init)(ಶೂನ್ಯ);
  ಶೂನ್ಯ (*cleanup)(ಶೂನ್ಯ);

  size_t (*version)(ವರ್ಣ *buffer, size_t size);
  CURLcode (*shut_down)(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(ಸಂರಚನಾ Curl_cfilter *cf, ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(ಸಂರಚನಾ Curl_easy *data, unsigned ವರ್ಣ *entropy,
                     size_t length);
  bool (*cert_status_request)(ಶೂನ್ಯ);

  CURLcode (*do_connect)(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps);
  ಶೂನ್ಯ *(*get_internals)(ಸಂರಚನಾ ssl_connect_data *connssl, CURLINFO info);
  ಶೂನ್ಯ (*close)(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data);
  ಶೂನ್ಯ (*close_all)(ಸಂರಚನಾ Curl_easy *data);

  ಸಂರಚನಾ curl_slist *(*engines_list)(ಸಂರಚನಾ Curl_easy *data);

  CURLcode (*sha256sum)(ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು, size_t inputlen,
                        unsigned ವರ್ಣ *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                         ವರ್ಣ *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                         ಧ್ರುವ ಶೂನ್ಯ *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                                  ಸಂರಚನಾ dynbuf *binding);
};

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(ಸಂರಚನಾ Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ alpn_spec *alpns,
                               ಸಂರಚನಾ Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
ಸಂರಚನಾ Curl_ssl_session *Curl_ssl_get_cf_session(ಸಂರಚನಾ Curl_easy *data,
                                                 ಧ್ರುವ ಸಂರಚನಾ Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ಸಂರಚನಾ ssl_connect_data *)(cf)->ctx)->call_data
#endif
