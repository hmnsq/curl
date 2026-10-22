#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VTLS_INT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VTLS_INT_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL

#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"

ਬਣਤਰ Curl_ssl;
ਬਣਤਰ ssl_connect_ਅੰਕੜੇ;
ਬਣਤਰ Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_HTTP_1_0_ਲੰਮਾਈ 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_HTTP_1_0        "http/1.0"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_HTTP_1_1_ਲੰਮਾਈ 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_HTTP_1_1        "http/1.1"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_H2_ਲੰਮਾਈ       2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_H2              "h2"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_H3_ਲੰਮਾਈ       2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_NAME_MAX      10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_ENTRIES_MAX   3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

ਬਣਤਰ alpn_spec {
  ਅੱਖਰ entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t ਗਿਣਤੀ_2; /* number of entries */
};

ਬਣਤਰ alpn_proto_buf {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਅੰਕੜੇ[ALPN_PROTO_BUF_MAX];
  ਪੂਰਨ len;
};

CURLcode Curl_alpn_ਮੰਜ਼ਿਲ_proto_buf(ਬਣਤਰ alpn_proto_buf *buf,
                                ਅਚਲ ਬਣਤਰ alpn_spec *spec);
CURLcode Curl_alpn_ਮੰਜ਼ਿਲ_proto_str(ਬਣਤਰ alpn_proto_buf *buf,
                                ਅਚਲ ਬਣਤਰ alpn_spec *spec);
ਖਾਲੀ Curl_alpn_restrict_ਮੰਜ਼ਿਲ(ਬਣਤਰ alpn_spec *spec, ਅਚਲ ਅੱਖਰ *proto);
ਖਾਲੀ Curl_alpn_copy(ਬਣਤਰ alpn_spec *dest, ਅਚਲ ਬਣਤਰ alpn_spec *src);

CURLcode Curl_alpn_ਸੈੱਟ_negotiated(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *proto,
                                  size_t proto_len);

bool Curl_alpn_ਸ਼ਾਮਲ_ਹੈ_proto(ਅਚਲ ਬਣਤਰ alpn_spec *spec, ਅਚਲ ਅੱਖਰ *proto);

/* enum for the nonblocking SSL connection state machine */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_ਪੂਰਾ
} ssl_connect_state;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  ssl_earlydata_none,
  ssl_earlydata_await,
  ssl_earlydata_sending,
  ssl_earlydata_ਭੇਜਿਆ,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SSL_IO_NEED_NONE   0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SSL_IO_NEED_RECV   (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
ਬਣਤਰ ssl_connect_ਅੰਕੜੇ {
  ਅਚਲ ਬਣਤਰ Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  ਬਣਤਰ ssl_peer peer;             /* peer the filter talks to */
  ਅਚਲ ਬਣਤਰ alpn_spec *alpn;     /* ALPN to use or NULL for none */
  ਖਾਲੀ *backend;                    /* vtls backend specific props */
  ਬਣਤਰ cf_call_ਅੰਕੜੇ call_ਅੰਕੜੇ;    /* data handle used in current call */
  ਬਣਤਰ Curl_ssl_session *session; /* TLS session in use or NULL */
  ਬਣਤਰ curltime handshake_ਪੂਰਾ;   /* time when handshake finished */
  ਬਣਤਰ {
    ਅੱਖਰ *alpn;                     /* ALPN value or NULL */
  } negotiated;
  ਬਣਤਰ bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  ਪੂਰਨ io_need;                      /* TLS signals special SEND/RECV needs */
  ਦੋਆਧਾਰੀ_ਅੰਕ(peer_closed);                 /* peer has closed connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(prefs_checked);               /* SSL preferences have been checked */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਨਿਵੇਸ਼_pending);               /* data for SSL_read() may be available */
  ਦੋਆਧਾਰੀ_ਅੰਕ(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

ਬਣਤਰ Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_ਅੰਕੜੇ;

  ਪੂਰਨ (*init)(ਖਾਲੀ);
  ਖਾਲੀ (*cleanup)(ਖਾਲੀ);

  size_t (*ਸੰਸਕਰਣ_2)(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ);
  CURLcode (*shut_ਥੱਲੇ)(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        bool send_shutdown, bool *ਪੂਰਾ);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*ਅੰਕੜੇ_pending)(ਬਣਤਰ Curl_cfilter *cf, ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy,
                     size_t ਲੰਮਾਈ);
  bool (*cert_ਸਥਿਤੀ_request)(ਖਾਲੀ);

  CURLcode (*do_connect)(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         bool *ਪੂਰਾ);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps);
  ਖਾਲੀ *(*get_internals)(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl, CURLINFO info);
  ਖਾਲੀ (*close)(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
  ਖਾਲੀ (*close_all)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

  ਬਣਤਰ curl_slist *(*engines_list)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

  CURLcode (*sha256sum)(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, size_t inputlen,
                        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਅੱਖਰ *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਅਚਲ ਖਾਲੀ *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                                  ਬਣਤਰ dynbuf *binding);
};

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(ਬਣਤਰ Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ alpn_spec *alpns,
                               ਬਣਤਰ Curl_ssl_session *scs,
                               bool *do_early_ਅੰਕੜੇ, bool early_ਅੰਕੜੇ_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
ਬਣਤਰ Curl_ssl_session *Curl_ssl_get_cf_session(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਅਚਲ ਬਣਤਰ Curl_cftype *cft,
                                                 int8_t sockindex);

#ਜੇ_ਅੰਤ /* USE_SSL */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VTLS_INT_H */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CF_CTX_CALL_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_CTX_CALL_ਅੰਕੜੇ(cf) ((ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *)(cf)->ctx)->call_ਅੰਕੜੇ
#ਜੇ_ਅੰਤ
