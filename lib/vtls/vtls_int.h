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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_SSL

#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"

ସଂରଚନା Curl_ssl;
ସଂରଚନା ssl_connect_data;
ସଂରଚନା Curl_ssl_session;

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

ସଂରଚନା alpn_spec {
  ଅକ୍ଷର entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t ଗଣନା_2; /* number of entries */
};

ସଂରଚନା alpn_proto_buf {
  unsigned ଅକ୍ଷର data[ALPN_PROTO_BUF_MAX];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
};

CURLcode Curl_alpn_to_proto_buf(ସଂରଚନା alpn_proto_buf *buf,
                                ଅଚଳ ସଂରଚନା alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(ସଂରଚନା alpn_proto_buf *buf,
                                ଅଚଳ ସଂରଚନା alpn_spec *spec);
ଶୂନ୍ୟ Curl_alpn_restrict_to(ସଂରଚନା alpn_spec *spec, ଅଚଳ ଅକ୍ଷର *proto);
ଶୂନ୍ୟ Curl_alpn_copy(ସଂରଚନା alpn_spec *dest, ଅଚଳ ସଂରଚନା alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssl_connect_data *connssl,
                                  ଅଚଳ unsigned ଅକ୍ଷର *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(ଅଚଳ ସଂରଚନା alpn_spec *spec, ଅଚଳ ଅକ୍ଷର *proto);

/* enum for the nonblocking SSL connection state machine */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
ସଂରଚନା ssl_connect_data {
  ଅଚଳ ସଂରଚନା Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  ସଂରଚନା ssl_peer peer;             /* peer the filter talks to */
  ଅଚଳ ସଂରଚନା alpn_spec *alpn;     /* ALPN to use or NULL for none */
  ଶୂନ୍ୟ *backend;                    /* vtls backend specific props */
  ସଂରଚନା cf_call_data call_data;    /* data handle used in current call */
  ସଂରଚନା Curl_ssl_session *session; /* TLS session in use or NULL */
  ସଂରଚନା curltime handshake_done;   /* time when handshake finished */
  ସଂରଚନା {
    ଅକ୍ଷର *alpn;                     /* ALPN value or NULL */
  } negotiated;
  ସଂରଚନା bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(ନିବେଶ_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

ସଂରଚନା Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_data;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*init)(ଶୂନ୍ୟ);
  ଶୂନ୍ୟ (*cleanup)(ଶୂନ୍ୟ);

  size_t (*version)(ଅକ୍ଷର *buffer, size_t size);
  CURLcode (*shut_down)(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(ସଂରଚନା Curl_cfilter *cf, ଅଚଳ ସଂରଚନା Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(ସଂରଚନା Curl_easy *data, unsigned ଅକ୍ଷର *entropy,
                     size_t length);
  bool (*cert_status_request)(ଶୂନ୍ୟ);

  CURLcode (*do_connect)(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା easy_pollset *ps);
  ଶୂନ୍ୟ *(*get_internals)(ସଂରଚନା ssl_connect_data *connssl, CURLINFO info);
  ଶୂନ୍ୟ (*close)(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data);
  ଶୂନ୍ୟ (*close_all)(ସଂରଚନା Curl_easy *data);

  ସଂରଚନା curl_slist *(*engines_list)(ସଂରଚନା Curl_easy *data);

  CURLcode (*sha256sum)(ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ, size_t inputlen,
                        unsigned ଅକ୍ଷର *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                         ଅକ୍ଷର *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                         ଅଚଳ ଶୂନ୍ୟ *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                                  ସଂରଚନା dynbuf *binding);
};

ବାହ୍ୟ ଅଚଳ ସଂରଚନା Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(ସଂରଚନା Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା alpn_spec *alpns,
                               ସଂରଚନା Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
ସଂରଚନା Curl_ssl_session *Curl_ssl_get_cf_session(ସଂରଚନା Curl_easy *data,
                                                 ଅଚଳ ସଂରଚନା Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ସଂରଚନା ssl_connect_data *)(cf)->ctx)->call_data
#endif
