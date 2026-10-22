#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VTLS_INT_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VTLS_INT_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL

#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"

ರಚನೆ Curl_ssl;
ರಚನೆ ssl_connect_ದತ್ತಾಂಶ;
ರಚನೆ Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_HTTP_1_0_ಉದ್ದಳತೆ 8
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_HTTP_1_0        "http/1.0"
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_HTTP_1_1_ಉದ್ದಳತೆ 8
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_HTTP_1_1        "http/1.1"
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_H2_ಉದ್ದಳತೆ       2
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_H2              "h2"
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_H3_ಉದ್ದಳತೆ       2
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_NAME_MAX      10
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_ENTRIES_MAX   3
#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

ರಚನೆ alpn_spec {
  ಅಕ್ಷರ entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t ಎಣಿಕೆ_2; /* number of entries */
};

ರಚನೆ alpn_proto_buf {
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ದತ್ತಾಂಶ[ALPN_PROTO_BUF_MAX];
  ಪೂರ್ಣಾಂಕ len;
};

CURLcode Curl_alpn_ಗಮ್ಯ_proto_buf(ರಚನೆ alpn_proto_buf *buf,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *spec);
CURLcode Curl_alpn_ಗಮ್ಯ_proto_str(ರಚನೆ alpn_proto_buf *buf,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *spec);
ಶೂನ್ಯ Curl_alpn_restrict_ಗಮ್ಯ(ರಚನೆ alpn_spec *spec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proto);
ಶೂನ್ಯ Curl_alpn_copy(ರಚನೆ alpn_spec *dest, ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *src);

CURLcode Curl_alpn_ಗಣ_negotiated(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *proto,
                                  size_t proto_len);

bool Curl_alpn_ಒಳಗೊಂಡಿದೆ_proto(ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *spec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proto);

/* enum for the nonblocking SSL connection state machine */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_ಮುಗಿದ
} ssl_connect_state;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  ssl_earlydata_none,
  ssl_earlydata_await,
  ssl_earlydata_sending,
  ssl_earlydata_ಕಳುಹಿಸಿದ,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SSL_IO_NEED_NONE   0
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SSL_IO_NEED_RECV   (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
ರಚನೆ ssl_connect_ದತ್ತಾಂಶ {
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  ರಚನೆ ssl_peer peer;             /* peer the filter talks to */
  ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpn;     /* ALPN to use or NULL for none */
  ಶೂನ್ಯ *backend;                    /* vtls backend specific props */
  ರಚನೆ cf_call_ದತ್ತಾಂಶ call_ದತ್ತಾಂಶ;    /* data handle used in current call */
  ರಚನೆ Curl_ssl_session *session; /* TLS session in use or NULL */
  ರಚನೆ curltime handshake_ಮುಗಿದ;   /* time when handshake finished */
  ರಚನೆ {
    ಅಕ್ಷರ *alpn;                     /* ALPN value or NULL */
  } negotiated;
  ರಚನೆ bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  ಪೂರ್ಣಾಂಕ io_need;                      /* TLS signals special SEND/RECV needs */
  ಬಿಟ್(peer_closed);                 /* peer has closed connection */
  ಬಿಟ್(prefs_checked);               /* SSL preferences have been checked */
  ಬಿಟ್(ಒಳಹರಿವು_pending);               /* data for SSL_read() may be available */
  ಬಿಟ್(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

ರಚನೆ Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_ದತ್ತಾಂಶ;

  ಪೂರ್ಣಾಂಕ (*init)(ಶೂನ್ಯ);
  ಶೂನ್ಯ (*cleanup)(ಶೂನ್ಯ);

  size_t (*ಆವೃತ್ತಿ_2)(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2);
  CURLcode (*shut_ಕೆಳಕ್ಕೆ)(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        bool send_shutdown, bool *ಮುಗಿದ);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*ದತ್ತಾಂಶ_pending)(ರಚನೆ Curl_cfilter *cf, ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy,
                     size_t ಉದ್ದಳತೆ);
  bool (*cert_ಸ್ಥಿತಿ_request)(ಶೂನ್ಯ);

  CURLcode (*do_connect)(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         bool *ಮುಗಿದ);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps);
  ಶೂನ್ಯ *(*get_internals)(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl, CURLINFO info);
  ಶೂನ್ಯ (*close)(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
  ಶೂನ್ಯ (*close_all)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

  ರಚನೆ curl_slist *(*engines_list)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

  CURLcode (*sha256sum)(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, size_t inputlen,
                        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಅಕ್ಷರ *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                                  ರಚನೆ dynbuf *binding);
};

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(ರಚನೆ Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ alpn_spec *alpns,
                               ರಚನೆ Curl_ssl_session *scs,
                               bool *do_early_ದತ್ತಾಂಶ, bool early_ದತ್ತಾಂಶ_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
ರಚನೆ Curl_ssl_session *Curl_ssl_get_cf_session(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cftype *cft,
                                                 int8_t sockindex);

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VTLS_INT_H */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CF_CTX_CALL_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTX_CALL_ದತ್ತಾಂಶ(cf) ((ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *)(cf)->ctx)->call_ದತ್ತಾಂಶ
#ಆದರೆ_ಅಂತ್ಯ
