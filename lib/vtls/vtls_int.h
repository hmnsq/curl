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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_SSL

#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "vtls/vtls.h"

கட்டமைப்பு Curl_ssl;
கட்டமைப்பு ssl_connect_data;
கட்டமைப்பு Curl_ssl_session;

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

கட்டமைப்பு alpn_spec {
  எழுத்து entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t மொத்தம்; /* number of entries */
};

கட்டமைப்பு alpn_proto_buf {
  unsigned எழுத்து data[ALPN_PROTO_BUF_MAX];
  முழு len;
};

CURLcode Curl_alpn_to_proto_buf(கட்டமைப்பு alpn_proto_buf *buf,
                                மாறிலி கட்டமைப்பு alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(கட்டமைப்பு alpn_proto_buf *buf,
                                மாறிலி கட்டமைப்பு alpn_spec *spec);
வெறுமை Curl_alpn_restrict_to(கட்டமைப்பு alpn_spec *spec, மாறிலி எழுத்து *proto);
வெறுமை Curl_alpn_copy(கட்டமைப்பு alpn_spec *dest, மாறிலி கட்டமைப்பு alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssl_connect_data *connssl,
                                  மாறிலி unsigned எழுத்து *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(மாறிலி கட்டமைப்பு alpn_spec *spec, மாறிலி எழுத்து *proto);

/* enum for the nonblocking SSL connection state machine */
வகைவரை எண்ணிக்கை {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

வகைவரை எண்ணிக்கை {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

வகைவரை எண்ணிக்கை {
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
கட்டமைப்பு ssl_connect_data {
  மாறிலி கட்டமைப்பு Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  கட்டமைப்பு ssl_peer peer;             /* peer the filter talks to */
  மாறிலி கட்டமைப்பு alpn_spec *alpn;     /* ALPN to use or NULL for none */
  வெறுமை *backend;                    /* vtls backend specific props */
  கட்டமைப்பு cf_call_data call_data;    /* data handle used in current call */
  கட்டமைப்பு Curl_ssl_session *session; /* TLS session in use or NULL */
  கட்டமைப்பு curltime handshake_done;   /* time when handshake finished */
  கட்டமைப்பு {
    எழுத்து *alpn;                     /* ALPN value or NULL */
  } negotiated;
  கட்டமைப்பு bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  முழு io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(உள்ளீடு_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

கட்டமைப்பு Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned முழு supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_data;

  முழு (*init)(வெறுமை);
  வெறுமை (*cleanup)(வெறுமை);

  size_t (*version)(எழுத்து *buffer, size_t size);
  CURLcode (*shut_down)(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(கட்டமைப்பு Curl_cfilter *cf, மாறிலி கட்டமைப்பு Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(கட்டமைப்பு Curl_easy *data, unsigned எழுத்து *entropy,
                     size_t length);
  bool (*cert_status_request)(வெறுமை);

  CURLcode (*do_connect)(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு easy_pollset *ps);
  வெறுமை *(*get_internals)(கட்டமைப்பு ssl_connect_data *connssl, CURLINFO info);
  வெறுமை (*close)(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data);
  வெறுமை (*close_all)(கட்டமைப்பு Curl_easy *data);

  கட்டமைப்பு curl_slist *(*engines_list)(கட்டமைப்பு Curl_easy *data);

  CURLcode (*sha256sum)(மாறிலி unsigned எழுத்து *உள்ளீடு, size_t inputlen,
                        unsigned எழுத்து *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                         எழுத்து *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                         மாறிலி வெறுமை *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                                  கட்டமைப்பு dynbuf *binding);
};

வெளி மாறிலி கட்டமைப்பு Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(கட்டமைப்பு Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு alpn_spec *alpns,
                               கட்டமைப்பு Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
கட்டமைப்பு Curl_ssl_session *Curl_ssl_get_cf_session(கட்டமைப்பு Curl_easy *data,
                                                 மாறிலி கட்டமைப்பு Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((கட்டமைப்பு ssl_connect_data *)(cf)->ctx)->call_data
#endif
