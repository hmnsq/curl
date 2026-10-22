#اگر_غیر_تعریف_شدہ سرخی_CURL_VTLS_INT_H
#تعریف_کرو سرخی_CURL_VTLS_INT_H
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

#اگر_تعریف_شدہ USE_SSL

#شامل "cfilters.h"
#شامل "select.h"
#شامل "urldata.h"
#شامل "vtls/vtls.h"

ساخت Curl_ssl;
ساخت ssl_connect_ڈیٹا;
ساخت Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#تعریف_کرو ALPN_HTTP_1_0_لمبائی 8
#تعریف_کرو ALPN_HTTP_1_0        "http/1.0"
#تعریف_کرو ALPN_HTTP_1_1_لمبائی 8
#تعریف_کرو ALPN_HTTP_1_1        "http/1.1"
#تعریف_کرو ALPN_H2_لمبائی       2
#تعریف_کرو ALPN_H2              "h2"
#تعریف_کرو ALPN_H3_لمبائی       2
#تعریف_کرو ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#تعریف_کرو ALPN_NAME_MAX      10
#تعریف_کرو ALPN_ENTRIES_MAX   3
#تعریف_کرو ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

ساخت alpn_spec {
  حرف entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t گنتی; /* number of entries */
};

ساخت alpn_proto_buf {
  غیرعلامتی حرف ڈیٹا[ALPN_PROTO_BUF_MAX];
  صحیح len;
};

CURLcode Curl_alpn_مقصد_proto_buf(ساخت alpn_proto_buf *buf,
                                ثابت ساخت alpn_spec *spec);
CURLcode Curl_alpn_مقصد_proto_str(ساخت alpn_proto_buf *buf,
                                ثابت ساخت alpn_spec *spec);
خالی Curl_alpn_restrict_مقصد(ساخت alpn_spec *spec, ثابت حرف *proto);
خالی Curl_alpn_copy(ساخت alpn_spec *dest, ثابت ساخت alpn_spec *src);

CURLcode Curl_alpn_سیٹ_negotiated(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت ssl_connect_ڈیٹا *connssl,
                                  ثابت غیرعلامتی حرف *proto,
                                  size_t proto_len);

bool Curl_alpn_شامل_ہے_proto(ثابت ساخت alpn_spec *spec, ثابت حرف *proto);

/* enum for the nonblocking SSL connection state machine */
قسمتعریف شمار {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_مکمل
} ssl_connect_state;

قسمتعریف شمار {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

قسمتعریف شمار {
  ssl_earlydata_none,
  ssl_earlydata_await,
  ssl_earlydata_sending,
  ssl_earlydata_بھیجا,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#تعریف_کرو CURL_SSL_IO_NEED_NONE   0
#تعریف_کرو CURL_SSL_IO_NEED_RECV   (1 << 0)
#تعریف_کرو CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#تعریف_کرو CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
ساخت ssl_connect_ڈیٹا {
  ثابت ساخت Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  ساخت ssl_peer peer;             /* peer the filter talks to */
  ثابت ساخت alpn_spec *alpn;     /* ALPN to use or NULL for none */
  خالی *backend;                    /* vtls backend specific props */
  ساخت cf_call_ڈیٹا call_ڈیٹا;    /* data handle used in current call */
  ساخت Curl_ssl_session *session; /* TLS session in use or NULL */
  ساخت curltime handshake_مکمل;   /* time when handshake finished */
  ساخت {
    حرف *alpn;                     /* ALPN value or NULL */
  } negotiated;
  ساخت bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  صحیح io_need;                      /* TLS signals special SEND/RECV needs */
  ثنائی_ہندسہ(peer_closed);                 /* peer has closed connection */
  ثنائی_ہندسہ(prefs_checked);               /* SSL preferences have been checked */
  ثنائی_ہندسہ(داخلہ_pending);               /* data for SSL_read() may be available */
  ثنائی_ہندسہ(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

ساخت Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  غیرعلامتی صحیح supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_ڈیٹا;

  صحیح (*init)(خالی);
  خالی (*cleanup)(خالی);

  size_t (*نسخہ_2)(حرف *بفر, size_t سائز);
  CURLcode (*shut_نیچے_کو)(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                        bool send_shutdown, bool *مکمل);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*ڈیٹا_pending)(ساخت Curl_cfilter *cf, ثابت ساخت Curl_easy *ڈیٹا);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(ساخت Curl_easy *ڈیٹا, غیرعلامتی حرف *entropy,
                     size_t لمبائی);
  bool (*cert_حالت_request)(خالی);

  CURLcode (*do_connect)(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                         bool *مکمل);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps);
  خالی *(*get_internals)(ساخت ssl_connect_ڈیٹا *connssl, CURLINFO info);
  خالی (*close)(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا);
  خالی (*close_all)(ساخت Curl_easy *ڈیٹا);

  ساخت curl_slist *(*engines_list)(ساخت Curl_easy *ڈیٹا);

  CURLcode (*sha256sum)(ثابت غیرعلامتی حرف *داخلہ, size_t inputlen,
                        غیرعلامتی حرف *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                         حرف *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                         ثابت خالی *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                                  ساخت dynbuf *binding);
};

خارجی ثابت ساخت Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(ساخت Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت alpn_spec *alpns,
                               ساخت Curl_ssl_session *scs,
                               bool *do_early_ڈیٹا, bool early_ڈیٹا_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
ساخت Curl_ssl_session *Curl_ssl_get_cf_session(ساخت Curl_easy *ڈیٹا,
                                                 ثابت ساخت Curl_cftype *cft,
                                                 int8_t sockindex);

#اگر_ختم /* USE_SSL */

#اگر_ختم /* HEADER_CURL_VTLS_INT_H */

#اگر_تعریف_شدہ USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#تعریف_منسوخ CF_CTX_CALL_ڈیٹا
#تعریف_کرو CF_CTX_CALL_ڈیٹا(cf) ((ساخت ssl_connect_ڈیٹا *)(cf)->ctx)->call_ڈیٹا
#اگر_ختم
