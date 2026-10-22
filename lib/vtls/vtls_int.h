#यदि_अपरिभाषितम् शीर्षकम्_CURL_VTLS_INT_H
#परिभाषय शीर्षकम्_CURL_VTLS_INT_H
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

#यदि_परिभाषितम् USE_SSL

#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"

संरचना Curl_ssl;
संरचना ssl_connect_दत्त;
संरचना Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#परिभाषय ALPN_HTTP_1_0_दीर्घता 8
#परिभाषय ALPN_HTTP_1_0        "http/1.0"
#परिभाषय ALPN_HTTP_1_1_दीर्घता 8
#परिभाषय ALPN_HTTP_1_1        "http/1.1"
#परिभाषय ALPN_H2_दीर्घता       2
#परिभाषय ALPN_H2              "h2"
#परिभाषय ALPN_H3_दीर्घता       2
#परिभाषय ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#परिभाषय ALPN_NAME_MAX      10
#परिभाषय ALPN_ENTRIES_MAX   3
#परिभाषय ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

संरचना alpn_spec {
  वर्ण entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t गणनम्; /* number of entries */
};

संरचना alpn_proto_buf {
  अचिह्नित वर्ण दत्त[ALPN_PROTO_BUF_MAX];
  पूर्णाङ्क len;
};

CURLcode Curl_alpn_गन्तव्यम्_proto_buf(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec);
CURLcode Curl_alpn_गन्तव्यम्_proto_str(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec);
शून्य Curl_alpn_restrict_गन्तव्यम्(संरचना alpn_spec *spec, ध्रुव वर्ण *proto);
शून्य Curl_alpn_copy(संरचना alpn_spec *dest, ध्रुव संरचना alpn_spec *src);

CURLcode Curl_alpn_समुच्चयः_negotiated(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना ssl_connect_दत्त *connssl,
                                  ध्रुव अचिह्नित वर्ण *proto,
                                  size_t proto_len);

bool Curl_alpn_अन्तर्भवति_proto(ध्रुव संरचना alpn_spec *spec, ध्रुव वर्ण *proto);

/* enum for the nonblocking SSL connection state machine */
प्रकारनिर्देश गणना {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_समाप्तम्
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
  ssl_earlydata_प्रेषितम्,
  ssl_earlydata_accepted,
  ssl_earlydata_rejected
} ssl_earlydata_state;

#परिभाषय CURL_SSL_IO_NEED_NONE   0
#परिभाषय CURL_SSL_IO_NEED_RECV   (1 << 0)
#परिभाषय CURL_SSL_IO_NEED_SEND   (1 << 1)

/* Max earlydata payload we want to send */
#परिभाषय CURL_SSL_EARLY_MAX      (64 * 1024)

/* Information in each SSL cfilter context: cf->ctx */
संरचना ssl_connect_दत्त {
  ध्रुव संरचना Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  संरचना ssl_peer peer;             /* peer the filter talks to */
  ध्रुव संरचना alpn_spec *alpn;     /* ALPN to use or NULL for none */
  शून्य *backend;                    /* vtls backend specific props */
  संरचना cf_call_दत्त call_दत्त;    /* data handle used in current call */
  संरचना Curl_ssl_session *session; /* TLS session in use or NULL */
  संरचना curltime handshake_समाप्तम्;   /* time when handshake finished */
  संरचना {
    वर्ण *alpn;                     /* ALPN value or NULL */
  } negotiated;
  संरचना bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  पूर्णाङ्क io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(निवेशः_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

संरचना Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  अचिह्नित पूर्णाङ्क supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_दत्त;

  पूर्णाङ्क (*init)(शून्य);
  शून्य (*cleanup)(शून्य);

  size_t (*संस्करणम्_2)(वर्ण *बफर, size_t आकार);
  CURLcode (*shut_अधस्तात्)(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                        bool send_shutdown, bool *समाप्तम्);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*दत्त_pending)(संरचना Curl_cfilter *cf, ध्रुव संरचना Curl_easy *दत्त);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(संरचना Curl_easy *दत्त, अचिह्नित वर्ण *entropy,
                     size_t दीर्घता);
  bool (*cert_स्थितिः_request)(शून्य);

  CURLcode (*do_connect)(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                         bool *समाप्तम्);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps);
  शून्य *(*get_internals)(संरचना ssl_connect_दत्त *connssl, CURLINFO info);
  शून्य (*close)(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त);
  शून्य (*close_all)(संरचना Curl_easy *दत्त);

  संरचना curl_slist *(*engines_list)(संरचना Curl_easy *दत्त);

  CURLcode (*sha256sum)(ध्रुव अचिह्नित वर्ण *निवेशः, size_t inputlen,
                        अचिह्नित वर्ण *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                         वर्ण *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                         ध्रुव शून्य *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(संरचना Curl_easy *दत्त, int8_t sockindex,
                                  संरचना dynbuf *binding);
};

बाह्य ध्रुव संरचना Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(संरचना Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना alpn_spec *alpns,
                               संरचना Curl_ssl_session *scs,
                               bool *do_early_दत्त, bool early_दत्त_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
संरचना Curl_ssl_session *Curl_ssl_get_cf_session(संरचना Curl_easy *दत्त,
                                                 ध्रुव संरचना Curl_cftype *cft,
                                                 int8_t sockindex);

#यदि_अन्तः /* USE_SSL */

#यदि_अन्तः /* HEADER_CURL_VTLS_INT_H */

#यदि_परिभाषितम् USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#अपरिभाषय CF_CTX_CALL_दत्त
#परिभाषय CF_CTX_CALL_दत्त(cf) ((संरचना ssl_connect_दत्त *)(cf)->ctx)->call_दत्त
#यदि_अन्तः
