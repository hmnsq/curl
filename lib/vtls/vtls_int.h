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
#شامل "curl_setup.h"

#ifdef USE_SSL

#شامل "cfilters.h"
#شامل "select.h"
#شامل "urldata.h"
#شامل "vtls/vtls.h"

ساختار Curl_ssl;
ساختار ssl_connect_data;
ساختار Curl_ssl_session;

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

ساختار alpn_spec {
  نویسه entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t شمار; /* number of entries */
};

ساختار alpn_proto_buf {
  unsigned نویسه data[ALPN_PROTO_BUF_MAX];
  صحیح len;
};

CURLcode Curl_alpn_to_proto_buf(ساختار alpn_proto_buf *buf,
                                ثابت ساختار alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(ساختار alpn_proto_buf *buf,
                                ثابت ساختار alpn_spec *spec);
تهی Curl_alpn_restrict_to(ساختار alpn_spec *spec, ثابت نویسه *proto);
تهی Curl_alpn_copy(ساختار alpn_spec *dest, ثابت ساختار alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار ssl_connect_data *connssl,
                                  ثابت unsigned نویسه *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(ثابت ساختار alpn_spec *spec, ثابت نویسه *proto);

/* enum for the nonblocking SSL connection state machine */
تعریفنوع شمارش {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

تعریفنوع شمارش {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

تعریفنوع شمارش {
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
ساختار ssl_connect_data {
  ثابت ساختار Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  ساختار ssl_peer peer;             /* peer the filter talks to */
  ثابت ساختار alpn_spec *alpn;     /* ALPN to use or NULL for none */
  تهی *backend;                    /* vtls backend specific props */
  ساختار cf_call_data call_data;    /* data handle used in current call */
  ساختار Curl_ssl_session *session; /* TLS session in use or NULL */
  ساختار curltime handshake_done;   /* time when handshake finished */
  ساختار {
    نویسه *alpn;                     /* ALPN value or NULL */
  } negotiated;
  ساختار bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  صحیح io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(درون_داد_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

ساختار Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned صحیح supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_data;

  صحیح (*init)(تهی);
  تهی (*cleanup)(تهی);

  size_t (*version)(نویسه *buffer, size_t size);
  CURLcode (*shut_down)(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(ساختار Curl_cfilter *cf, ثابت ساختار Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(ساختار Curl_easy *data, unsigned نویسه *entropy,
                     size_t length);
  bool (*cert_status_request)(تهی);

  CURLcode (*do_connect)(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                             ساختار easy_pollset *ps);
  تهی *(*get_internals)(ساختار ssl_connect_data *connssl, CURLINFO info);
  تهی (*close)(ساختار Curl_cfilter *cf, ساختار Curl_easy *data);
  تهی (*close_all)(ساختار Curl_easy *data);

  ساختار curl_slist *(*engines_list)(ساختار Curl_easy *data);

  CURLcode (*sha256sum)(ثابت unsigned نویسه *درون_داد, size_t inputlen,
                        unsigned نویسه *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                         نویسه *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                         ثابت تهی *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(ساختار Curl_easy *data, int8_t sockindex,
                                  ساختار dynbuf *binding);
};

بیرونی ثابت ساختار Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(ساختار Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار alpn_spec *alpns,
                               ساختار Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
ساختار Curl_ssl_session *Curl_ssl_get_cf_session(ساختار Curl_easy *data,
                                                 ثابت ساختار Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ساختار ssl_connect_data *)(cf)->ctx)->call_data
#endif
