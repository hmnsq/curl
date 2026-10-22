#अगर_अघोषित HEADER_CURL_VTLS_INT_H
#घोषणा HEADER_CURL_VTLS_INT_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_SSL

#समावेश "cfilters.h"
#समावेश "select.h"
#समावेश "urldata.h"
#समावेश "vtls/vtls.h"

काष्ठा Curl_ssl;
काष्ठा ssl_connect_तथ्य;
काष्ठा Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#घोषणा ALPN_HTTP_1_0_लंबाई 8
#घोषणा ALPN_HTTP_1_0        "http/1.0"
#घोषणा ALPN_HTTP_1_1_लंबाई 8
#घोषणा ALPN_HTTP_1_1        "http/1.1"
#घोषणा ALPN_H2_लंबाई       2
#घोषणा ALPN_H2              "h2"
#घोषणा ALPN_H3_लंबाई       2
#घोषणा ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#घोषणा ALPN_NAME_MAX      10
#घोषणा ALPN_ENTRIES_MAX   3
#घोषणा ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

काष्ठा alpn_spec {
  अक्षर entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  माप_प्रकार गिनती; /* number of entries */
};

काष्ठा alpn_proto_buf {
  अचिन्हित अक्षर तथ्य[ALPN_PROTO_BUF_MAX];
  पूर्णांक len;
};

CURLcode Curl_alpn_to_proto_buf(काष्ठा alpn_proto_buf *buf,
                                स्थिर काष्ठा alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(काष्ठा alpn_proto_buf *buf,
                                स्थिर काष्ठा alpn_spec *spec);
व्योम Curl_alpn_restrict_to(काष्ठा alpn_spec *spec, स्थिर अक्षर *proto);
व्योम Curl_alpn_copy(काष्ठा alpn_spec *dest, स्थिर काष्ठा alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssl_connect_तथ्य *connssl,
                                  स्थिर अचिन्हित अक्षर *proto,
                                  माप_प्रकार proto_len);

bool Curl_alpn_contains_proto(स्थिर काष्ठा alpn_spec *spec, स्थिर अक्षर *proto);

/* enum for the nonblocking SSL connection state machine */
प्रकार क्रमागत {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

प्रकार क्रमागत {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

प्रकार क्रमागत {
  ssl_earlydata_none,
  ssl_earlydata_await,
  ssl_earlydata_sending,
  ssl_earlydata_sent,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#घोषणा CURL_SSL_IO_NEED_NONE   0
#घोषणा CURL_SSL_IO_NEED_RECV   (1 << 0)
#घोषणा CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#घोषणा CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
काष्ठा ssl_connect_तथ्य {
  स्थिर काष्ठा Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  काष्ठा ssl_peer peer;             /* peer the filter talks to */
  स्थिर काष्ठा alpn_spec *alpn;     /* ALPN to use or NULL for none */
  व्योम *backend;                    /* vtls backend specific props */
  काष्ठा cf_call_तथ्य call_तथ्य;    /* data handle used in current call */
  काष्ठा Curl_ssl_session *session; /* TLS session in use or NULL */
  काष्ठा curltime handshake_done;   /* time when handshake finished */
  काष्ठा {
    अक्षर *alpn;                     /* ALPN value or NULL */
  } negotiated;
  काष्ठा bufq earlydata;            /* earlydata to be send to peer */
  माप_प्रकार earlydata_max;             /* max earlydata allowed by peer */
  माप_प्रकार earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  पूर्णांक io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(निवेश_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

काष्ठा Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  अचिन्हित पूर्णांक supports; /* bitfield, see above */
  माप_प्रकार sizeof_ssl_backend_तथ्य;

  पूर्णांक (*init)(व्योम);
  व्योम (*cleanup)(व्योम);

  माप_प्रकार (*version)(अक्षर *बफर, माप_प्रकार आकार);
  CURLcode (*shut_down)(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*तथ्य_pending)(काष्ठा Curl_cfilter *cf, स्थिर काष्ठा Curl_easy *तथ्य);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(काष्ठा Curl_easy *तथ्य, अचिन्हित अक्षर *entropy,
                     माप_प्रकार लंबाई);
  bool (*cert_status_request)(व्योम);

  CURLcode (*do_connect)(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps);
  व्योम *(*get_internals)(काष्ठा ssl_connect_तथ्य *connssl, CURLINFO info);
  व्योम (*बंद)(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य);
  व्योम (*close_all)(काष्ठा Curl_easy *तथ्य);

  काष्ठा curl_slist *(*engines_list)(काष्ठा Curl_easy *तथ्य);

  CURLcode (*sha256sum)(स्थिर अचिन्हित अक्षर *निवेश, माप_प्रकार inputlen,
                        अचिन्हित अक्षर *sha256sum, माप_प्रकार sha256sumlen);
  CURLcode (*recv_plain)(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                         अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread);
  CURLcode (*send_plain)(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                         स्थिर व्योम *mem, माप_प्रकार len, माप_प्रकार *pnwritten);

  CURLcode (*get_channel_binding)(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                                  काष्ठा dynbuf *binding);
};

बाह्य स्थिर काष्ठा Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(काष्ठा Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा alpn_spec *alpns,
                               काष्ठा Curl_ssl_session *scs,
                               bool *do_early_तथ्य, bool early_तथ्य_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
काष्ठा Curl_ssl_session *Curl_ssl_get_cf_session(काष्ठा Curl_easy *तथ्य,
                                                 स्थिर काष्ठा Curl_cftype *cft,
                                                 int8_t sockindex);

#पूर्ण_अगर /* USE_SSL */

#पूर्ण_अगर /* HEADER_CURL_VTLS_INT_H */

#अगर_घोषित USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#अघोषित CF_CTX_CALL_तथ्य
#घोषणा CF_CTX_CALL_तथ्य(cf) ((काष्ठा ssl_connect_तथ्य *)(cf)->ctx)->call_तथ्य
#पूर्ण_अगर
