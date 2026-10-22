#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VTLS_INT_H
#وصف_ڪريو سرو_CURL_VTLS_INT_H
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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_ٿيل USE_SSL

#ضمن "cfilters.h"
#ضمن "select.h"
#ضمن "urldata.h"
#ضمن "vtls/vtls.h"

بنية Curl_ssl;
بنية ssl_connect_معلومات;
بنية Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#وصف_ڪريو ALPN_HTTP_1_0_ڊيگهائي 8
#وصف_ڪريو ALPN_HTTP_1_0        "http/1.0"
#وصف_ڪريو ALPN_HTTP_1_1_ڊيگهائي 8
#وصف_ڪريو ALPN_HTTP_1_1        "http/1.1"
#وصف_ڪريو ALPN_H2_ڊيگهائي       2
#وصف_ڪريو ALPN_H2              "h2"
#وصف_ڪريو ALPN_H3_ڊيگهائي       2
#وصف_ڪريو ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#وصف_ڪريو ALPN_NAME_MAX      10
#وصف_ڪريو ALPN_ENTRIES_MAX   3
#وصف_ڪريو ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

بنية alpn_spec {
  حرف entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t ڳڻپ; /* number of entries */
};

بنية alpn_proto_buf {
  بنانشان حرف معلومات[ALPN_PROTO_BUF_MAX];
  صحيح len;
};

CURLcode Curl_alpn_منزل_proto_buf(بنية alpn_proto_buf *buf,
                                ثابت بنية alpn_spec *spec);
CURLcode Curl_alpn_منزل_proto_str(بنية alpn_proto_buf *buf,
                                ثابت بنية alpn_spec *spec);
فراغ Curl_alpn_restrict_منزل(بنية alpn_spec *spec, ثابت حرف *proto);
فراغ Curl_alpn_copy(بنية alpn_spec *dest, ثابت بنية alpn_spec *src);

CURLcode Curl_alpn_سيٽ_negotiated(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية ssl_connect_معلومات *connssl,
                                  ثابت بنانشان حرف *proto,
                                  size_t proto_len);

bool Curl_alpn_شامل_آهي_proto(ثابت بنية alpn_spec *spec, ثابت حرف *proto);

/* enum for the nonblocking SSL connection state machine */
تعريفنوع تعداد {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_پورو
} ssl_connect_state;

تعريفنوع تعداد {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

تعريفنوع تعداد {
  ssl_earlydata_none,
  ssl_earlydata_await,
  ssl_earlydata_sending,
  ssl_earlydata_موڪليل,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#وصف_ڪريو CURL_SSL_IO_NEED_NONE   0
#وصف_ڪريو CURL_SSL_IO_NEED_RECV   (1 << 0)
#وصف_ڪريو CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#وصف_ڪريو CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
بنية ssl_connect_معلومات {
  ثابت بنية Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  بنية ssl_peer peer;             /* peer the filter talks to */
  ثابت بنية alpn_spec *alpn;     /* ALPN to use or NULL for none */
  فراغ *backend;                    /* vtls backend specific props */
  بنية cf_call_معلومات call_معلومات;    /* data handle used in current call */
  بنية Curl_ssl_session *session; /* TLS session in use or NULL */
  بنية curltime handshake_پورو;   /* time when handshake finished */
  بنية {
    حرف *alpn;                     /* ALPN value or NULL */
  } negotiated;
  بنية bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  صحيح io_need;                      /* TLS signals special SEND/RECV needs */
  ٻٽو_انگ(peer_closed);                 /* peer has closed connection */
  ٻٽو_انگ(prefs_checked);               /* SSL preferences have been checked */
  ٻٽو_انگ(داخلا_pending);               /* data for SSL_read() may be available */
  ٻٽو_انگ(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

بنية Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  بنانشان صحيح supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_معلومات;

  صحيح (*init)(فراغ);
  فراغ (*cleanup)(فراغ);

  size_t (*نسخو_2)(حرف *بفر, size_t سائيز);
  CURLcode (*shut_هيٺ_ڏانهن)(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                        bool send_shutdown, bool *پورو);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*معلومات_pending)(بنية Curl_cfilter *cf, ثابت بنية Curl_easy *معلومات);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(بنية Curl_easy *معلومات, بنانشان حرف *entropy,
                     size_t ڊيگهائي);
  bool (*cert_حالت_request)(فراغ);

  CURLcode (*do_connect)(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                         bool *پورو);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps);
  فراغ *(*get_internals)(بنية ssl_connect_معلومات *connssl, CURLINFO info);
  فراغ (*close)(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات);
  فراغ (*close_all)(بنية Curl_easy *معلومات);

  بنية curl_slist *(*engines_list)(بنية Curl_easy *معلومات);

  CURLcode (*sha256sum)(ثابت بنانشان حرف *داخلا, size_t inputlen,
                        بنانشان حرف *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                         حرف *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                         ثابت فراغ *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(بنية Curl_easy *معلومات, int8_t sockindex,
                                  بنية dynbuf *binding);
};

خارجي ثابت بنية Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(بنية Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية alpn_spec *alpns,
                               بنية Curl_ssl_session *scs,
                               bool *do_early_معلومات, bool early_معلومات_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
بنية Curl_ssl_session *Curl_ssl_get_cf_session(بنية Curl_easy *معلومات,
                                                 ثابت بنية Curl_cftype *cft,
                                                 int8_t sockindex);

#جيڪڏهن_پورو /* USE_SSL */

#جيڪڏهن_پورو /* HEADER_CURL_VTLS_INT_H */

#جيڪڏهن_وصف_ٿيل USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#وصف_رد CF_CTX_CALL_معلومات
#وصف_ڪريو CF_CTX_CALL_معلومات(cf) ((بنية ssl_connect_معلومات *)(cf)->ctx)->call_معلومات
#جيڪڏهن_پورو
