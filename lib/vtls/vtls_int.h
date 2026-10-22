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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_SSL

#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "vtls/vtls.h"

ഘടന Curl_ssl;
ഘടന ssl_connect_data;
ഘടന Curl_ssl_session;

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

ഘടന alpn_spec {
  അക്ഷരം entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t എണ്ണം; /* number of entries */
};

ഘടന alpn_proto_buf {
  unsigned അക്ഷരം data[ALPN_PROTO_BUF_MAX];
  പൂർണ്ണസംഖ്യ len;
};

CURLcode Curl_alpn_to_proto_buf(ഘടന alpn_proto_buf *buf,
                                സ്ഥിരം ഘടന alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(ഘടന alpn_proto_buf *buf,
                                സ്ഥിരം ഘടന alpn_spec *spec);
ശൂന്യം Curl_alpn_restrict_to(ഘടന alpn_spec *spec, സ്ഥിരം അക്ഷരം *proto);
ശൂന്യം Curl_alpn_copy(ഘടന alpn_spec *dest, സ്ഥിരം ഘടന alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന ssl_connect_data *connssl,
                                  സ്ഥിരം unsigned അക്ഷരം *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(സ്ഥിരം ഘടന alpn_spec *spec, സ്ഥിരം അക്ഷരം *proto);

/* enum for the nonblocking SSL connection state machine */
തരനിർവചനം എണ്ണൽ {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

തരനിർവചനം എണ്ണൽ {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

തരനിർവചനം എണ്ണൽ {
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
ഘടന ssl_connect_data {
  സ്ഥിരം ഘടന Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  ഘടന ssl_peer peer;             /* peer the filter talks to */
  സ്ഥിരം ഘടന alpn_spec *alpn;     /* ALPN to use or NULL for none */
  ശൂന്യം *backend;                    /* vtls backend specific props */
  ഘടന cf_call_data call_data;    /* data handle used in current call */
  ഘടന Curl_ssl_session *session; /* TLS session in use or NULL */
  ഘടന curltime handshake_done;   /* time when handshake finished */
  ഘടന {
    അക്ഷരം *alpn;                     /* ALPN value or NULL */
  } negotiated;
  ഘടന bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  പൂർണ്ണസംഖ്യ io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(നിവേശം_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

ഘടന Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned പൂർണ്ണസംഖ്യ supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_data;

  പൂർണ്ണസംഖ്യ (*init)(ശൂന്യം);
  ശൂന്യം (*cleanup)(ശൂന്യം);

  size_t (*version)(അക്ഷരം *buffer, size_t size);
  CURLcode (*shut_down)(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(ഘടന Curl_cfilter *cf, സ്ഥിരം ഘടന Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(ഘടന Curl_easy *data, unsigned അക്ഷരം *entropy,
                     size_t length);
  bool (*cert_status_request)(ശൂന്യം);

  CURLcode (*do_connect)(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                             ഘടന easy_pollset *ps);
  ശൂന്യം *(*get_internals)(ഘടന ssl_connect_data *connssl, CURLINFO info);
  ശൂന്യം (*close)(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data);
  ശൂന്യം (*close_all)(ഘടന Curl_easy *data);

  ഘടന curl_slist *(*engines_list)(ഘടന Curl_easy *data);

  CURLcode (*sha256sum)(സ്ഥിരം unsigned അക്ഷരം *നിവേശം, size_t inputlen,
                        unsigned അക്ഷരം *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                         അക്ഷരം *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                         സ്ഥിരം ശൂന്യം *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(ഘടന Curl_easy *data, int8_t sockindex,
                                  ഘടന dynbuf *binding);
};

ബാഹ്യ സ്ഥിരം ഘടന Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(ഘടന Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന alpn_spec *alpns,
                               ഘടന Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
ഘടന Curl_ssl_session *Curl_ssl_get_cf_session(ഘടന Curl_easy *data,
                                                 സ്ഥിരം ഘടന Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ഘടന ssl_connect_data *)(cf)->ctx)->call_data
#endif
