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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_SSL

#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "vtls/vtls.h"

সংরচনা Curl_ssl;
সংরচনা ssl_connect_data;
সংরচনা Curl_ssl_session;

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

সংরচনা alpn_spec {
  ৱর্ণ entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t গুনতি; /* number of entries */
};

সংরচনা alpn_proto_buf {
  unsigned ৱর্ণ data[ALPN_PROTO_BUF_MAX];
  পূর্ণাঙ্ক len;
};

CURLcode Curl_alpn_to_proto_buf(সংরচনা alpn_proto_buf *buf,
                                ধ্রুৱ সংরচনা alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(সংরচনা alpn_proto_buf *buf,
                                ধ্রুৱ সংরচনা alpn_spec *spec);
শূন্য Curl_alpn_restrict_to(সংরচনা alpn_spec *spec, ধ্রুৱ ৱর্ণ *proto);
শূন্য Curl_alpn_copy(সংরচনা alpn_spec *dest, ধ্রুৱ সংরচনা alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা ssl_connect_data *connssl,
                                  ধ্রুৱ unsigned ৱর্ণ *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(ধ্রুৱ সংরচনা alpn_spec *spec, ধ্রুৱ ৱর্ণ *proto);

/* enum for the nonblocking SSL connection state machine */
প্রকারনির্দেশ গণনা {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

প্রকারনির্দেশ গণনা {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

প্রকারনির্দেশ গণনা {
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
সংরচনা ssl_connect_data {
  ধ্রুৱ সংরচনা Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  সংরচনা ssl_peer peer;             /* peer the filter talks to */
  ধ্রুৱ সংরচনা alpn_spec *alpn;     /* ALPN to use or NULL for none */
  শূন্য *backend;                    /* vtls backend specific props */
  সংরচনা cf_call_data call_data;    /* data handle used in current call */
  সংরচনা Curl_ssl_session *session; /* TLS session in use or NULL */
  সংরচনা curltime handshake_done;   /* time when handshake finished */
  সংরচনা {
    ৱর্ণ *alpn;                     /* ALPN value or NULL */
  } negotiated;
  সংরচনা bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  পূর্ণাঙ্ক io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(প্ৰবেশ_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

সংরচনা Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned পূর্ণাঙ্ক supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_data;

  পূর্ণাঙ্ক (*init)(শূন্য);
  শূন্য (*cleanup)(শূন্য);

  size_t (*version)(ৱর্ণ *buffer, size_t size);
  CURLcode (*shut_down)(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*data_pending)(সংরচনা Curl_cfilter *cf, ধ্রুৱ সংরচনা Curl_easy *data);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(সংরচনা Curl_easy *data, unsigned ৱর্ণ *entropy,
                     size_t length);
  bool (*cert_status_request)(শূন্য);

  CURLcode (*do_connect)(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps);
  শূন্য *(*get_internals)(সংরচনা ssl_connect_data *connssl, CURLINFO info);
  শূন্য (*close)(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data);
  শূন্য (*close_all)(সংরচনা Curl_easy *data);

  সংরচনা curl_slist *(*engines_list)(সংরচনা Curl_easy *data);

  CURLcode (*sha256sum)(ধ্রুৱ unsigned ৱর্ণ *প্ৰবেশ, size_t inputlen,
                        unsigned ৱর্ণ *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                         ৱর্ণ *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                         ধ্রুৱ শূন্য *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(সংরচনা Curl_easy *data, int8_t sockindex,
                                  সংরচনা dynbuf *binding);
};

বাহ্য ধ্রুৱ সংরচনা Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(সংরচনা Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা alpn_spec *alpns,
                               সংরচনা Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
সংরচনা Curl_ssl_session *Curl_ssl_get_cf_session(সংরচনা Curl_easy *data,
                                                 ধ্রুৱ সংরচনা Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((সংরচনা ssl_connect_data *)(cf)->ctx)->call_data
#endif
