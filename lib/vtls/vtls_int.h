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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_SSL

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"

संरचना Curl_ssl;
संरचना ssl_connect_data;
संरचना Curl_ssl_session;

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

संरचना alpn_spec {
  वर्ण entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  माप_प्रकार गिनती; /* number of entries */
};

संरचना alpn_proto_buf {
  अचिन्हित वर्ण data[ALPN_PROTO_BUF_MAX];
  पूर्णाङ्क len;
};

CURLcode Curl_alpn_to_proto_buf(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec);
शून्य Curl_alpn_restrict_to(संरचना alpn_spec *spec, ध्रुव वर्ण *proto);
शून्य Curl_alpn_copy(संरचना alpn_spec *dest, ध्रुव संरचना alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना ssl_connect_data *connssl,
                                  ध्रुव अचिन्हित वर्ण *proto,
                                  माप_प्रकार proto_len);

bool Curl_alpn_contains_proto(ध्रुव संरचना alpn_spec *spec, ध्रुव वर्ण *proto);

/* enum for the nonblocking SSL connection state machine */
प्रकारनिर्देश गणना {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

प्रकारनिर्देश गणना {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

प्रकारनिर्देश गणना {
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
संरचना ssl_connect_data {
  ध्रुव संरचना Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  संरचना ssl_peer peer;             /* peer the filter talks to */
  ध्रुव संरचना alpn_spec *alpn;     /* ALPN to use or NULL for none */
  शून्य *backend;                    /* vtls backend specific props */
  संरचना cf_call_data call_data;    /* data handle used in current call */
  संरचना Curl_ssl_session *session; /* TLS session in use or NULL */
  संरचना curltime handshake_done;   /* time when handshake finished */
  संरचना {
    वर्ण *alpn;                     /* ALPN value or NULL */
  } negotiated;
  संरचना bufq earlydata;            /* earlydata to be send to peer */
  माप_प्रकार earlydata_max;             /* max earlydata allowed by peer */
  माप_प्रकार earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  पूर्णाङ्क io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(निवेश_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

संरचना Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  अचिन्हित पूर्णाङ्क supports; /* bitfield, see above */
  माप_प्रकार sizeof_ssl_backend_data;

  पूर्णाङ्क (*init)(शून्य);
  शून्य (*cleanup)(शून्य);

  माप_प्रकार (*version)(वर्ण *buffer, माप_प्रकार size);
  CURLcode (*shut_down)(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(संरचना Curl_cfilter *cf, ध्रुव संरचना Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(संरचना Curl_easy *data, अचिन्हित वर्ण *entropy,
                     माप_प्रकार length);
  bool (*cert_status_request)(शून्य);

  CURLcode (*do_connect)(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                             संरचना easy_pollset *ps);
  शून्य *(*get_internals)(संरचना ssl_connect_data *connssl, CURLINFO info);
  शून्य (*बंद)(संरचना Curl_cfilter *cf, संरचना Curl_easy *data);
  शून्य (*close_all)(संरचना Curl_easy *data);

  संरचना curl_slist *(*engines_list)(संरचना Curl_easy *data);

  CURLcode (*sha256sum)(ध्रुव अचिन्हित वर्ण *निवेश, माप_प्रकार inputlen,
                        अचिन्हित वर्ण *sha256sum, माप_प्रकार sha256sumlen);
  CURLcode (*recv_plain)(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                         वर्ण *buf, माप_प्रकार len, माप_प्रकार *pnread);
  CURLcode (*send_plain)(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                         ध्रुव शून्य *mem, माप_प्रकार len, माप_प्रकार *pnwritten);

  CURLcode (*get_channel_binding)(संरचना Curl_easy *data, int8_t sockindex,
                                  संरचना dynbuf *binding);
};

बाह्य ध्रुव संरचना Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(संरचना Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना alpn_spec *alpns,
                               संरचना Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
संरचना Curl_ssl_session *Curl_ssl_get_cf_session(संरचना Curl_easy *data,
                                                 ध्रुव संरचना Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((संरचना ssl_connect_data *)(cf)->ctx)->call_data
#endif
