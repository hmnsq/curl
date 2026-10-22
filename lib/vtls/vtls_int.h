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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_SSL

#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "vtls/vtls.h"

কাঠামো Curl_ssl;
কাঠামো ssl_connect_তথ্য;
কাঠামো Curl_ssl_session;

/* see https://www.iana.org/assignments/tls-extensiontype-values/ */
#define ALPN_HTTP_1_0_দৈর্ঘ্য 8
#define ALPN_HTTP_1_0        "http/1.0"
#define ALPN_HTTP_1_1_দৈর্ঘ্য 8
#define ALPN_HTTP_1_1        "http/1.1"
#define ALPN_H2_দৈর্ঘ্য       2
#define ALPN_H2              "h2"
#define ALPN_H3_দৈর্ঘ্য       2
#define ALPN_H3              "h3"

/* conservative sizes on the ALPN entries and count we are handling,
 * we can increase these if we ever feel the need or have to accommodate
 * ALPN strings from the "outside". */
#define ALPN_NAME_MAX      10
#define ALPN_ENTRIES_MAX   3
#define ALPN_PROTO_BUF_MAX (ALPN_ENTRIES_MAX * (ALPN_NAME_MAX + 1))

কাঠামো alpn_spec {
  অক্ষর entries[ALPN_ENTRIES_MAX][ALPN_NAME_MAX];
  size_t গুনতি; /* number of entries */
};

কাঠামো alpn_proto_buf {
  unsigned অক্ষর তথ্য[ALPN_PROTO_BUF_MAX];
  পূর্ণসংখ্যা len;
};

CURLcode Curl_alpn_to_proto_buf(কাঠামো alpn_proto_buf *buf,
                                ধ্রুবক কাঠামো alpn_spec *spec);
CURLcode Curl_alpn_to_proto_str(কাঠামো alpn_proto_buf *buf,
                                ধ্রুবক কাঠামো alpn_spec *spec);
শূন্য Curl_alpn_restrict_to(কাঠামো alpn_spec *spec, ধ্রুবক অক্ষর *proto);
শূন্য Curl_alpn_copy(কাঠামো alpn_spec *dest, ধ্রুবক কাঠামো alpn_spec *src);

CURLcode Curl_alpn_set_negotiated(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssl_connect_তথ্য *connssl,
                                  ধ্রুবক unsigned অক্ষর *proto,
                                  size_t proto_len);

bool Curl_alpn_contains_proto(ধ্রুবক কাঠামো alpn_spec *spec, ধ্রুবক অক্ষর *proto);

/* enum for the nonblocking SSL connection state machine */
প্রকারলেখ গণনা {
  ssl_connect_1,
  ssl_connect_2,
  ssl_connect_3,
  ssl_connect_done
} ssl_connect_state;

প্রকারলেখ গণনা {
  ssl_connection_none,
  ssl_connection_deferred,
  ssl_connection_negotiating,
  ssl_connection_complete
} ssl_connection_state;

প্রকারলেখ গণনা {
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
কাঠামো ssl_connect_তথ্য {
  ধ্রুবক কাঠামো Curl_ssl *ssl_impl;  /* TLS backend for this filter */
  কাঠামো ssl_peer peer;             /* peer the filter talks to */
  ধ্রুবক কাঠামো alpn_spec *alpn;     /* ALPN to use or NULL for none */
  শূন্য *backend;                    /* vtls backend specific props */
  কাঠামো cf_call_তথ্য call_তথ্য;    /* data handle used in current call */
  কাঠামো Curl_ssl_session *session; /* TLS session in use or NULL */
  কাঠামো curltime handshake_done;   /* time when handshake finished */
  কাঠামো {
    অক্ষর *alpn;                     /* ALPN value or NULL */
  } negotiated;
  কাঠামো bufq earlydata;            /* earlydata to be send to peer */
  size_t earlydata_max;             /* max earlydata allowed by peer */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                     * is accepted by peer */
  ssl_connection_state state;
  ssl_connect_state connecting_state;
  ssl_earlydata_state earlydata_state;
  পূর্ণসংখ্যা io_need;                      /* TLS signals special SEND/RECV needs */
  BIT(peer_closed);                 /* peer has closed connection */
  BIT(prefs_checked);               /* SSL preferences have been checked */
  BIT(প্রবেশ_pending);               /* data for SSL_read() may be available */
  BIT(stats_reported);              /* connect times have been reported */
};

/* Definitions for SSL Implementations */

কাঠামো Curl_ssl {
  /*
   * This *must* be the first entry to allow returning the list of available
   * backends in curl_global_sslset().
   */
  curl_ssl_backend info;
  unsigned পূর্ণসংখ্যা supports; /* bitfield, see above */
  size_t sizeof_ssl_backend_তথ্য;

  পূর্ণসংখ্যা (*init)(শূন্য);
  শূন্য (*cleanup)(শূন্য);

  size_t (*version)(অক্ষর *বাফার, size_t আকার_2);
  CURLcode (*shut_down)(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                        bool send_shutdown, bool *done);

  /* data_pending() shall return TRUE when it wants to get called again to
     drain internal buffers and deliver data instead of waiting for the socket
     to get readable */
  bool (*তথ্য_pending)(কাঠামো Curl_cfilter *cf, ধ্রুবক কাঠামো Curl_easy *তথ্য);

  /* return 0 if a find random is filled in */
  CURLcode (*random)(কাঠামো Curl_easy *তথ্য, unsigned অক্ষর *entropy,
                     size_t দৈর্ঘ্য);
  bool (*cert_status_request)(শূন্য);

  CURLcode (*do_connect)(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                         bool *done);

  /* During handshake/shutdown, adjust the pollset to include the socket
   * for POLLOUT or POLLIN as needed. Mandatory. */
  CURLcode (*adjust_pollset)(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps);
  শূন্য *(*get_internals)(কাঠামো ssl_connect_তথ্য *connssl, CURLINFO info);
  শূন্য (*close)(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য);
  শূন্য (*close_all)(কাঠামো Curl_easy *তথ্য);

  কাঠামো curl_slist *(*engines_list)(কাঠামো Curl_easy *তথ্য);

  CURLcode (*sha256sum)(ধ্রুবক unsigned অক্ষর *প্রবেশ, size_t inputlen,
                        unsigned অক্ষর *sha256sum, size_t sha256sumlen);
  CURLcode (*recv_plain)(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                         অক্ষর *buf, size_t len, size_t *pnread);
  CURLcode (*send_plain)(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                         ধ্রুবক শূন্য *mem, size_t len, size_t *pnwritten);

  CURLcode (*get_channel_binding)(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                                  কাঠামো dynbuf *binding);
};

বাহ্যিক ধ্রুবক কাঠামো Curl_ssl *Curl_ssl;

CURLcode Curl_ssl_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো easy_pollset *ps);

/**
 * Get the SSL filter below the given one or NULL if there is none.
 */
bool Curl_ssl_cf_is_proxy(কাঠামো Curl_cfilter *cf);

CURLcode Curl_on_session_reuse(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো alpn_spec *alpns,
                               কাঠামো Curl_ssl_session *scs,
                               bool *do_early_তথ্য, bool early_তথ্য_allowed);

/* Retrieve the SSL session held at the filter of type `cft` at
 * data's connection at `sockindex` or NULL if not found/available. */
কাঠামো Curl_ssl_session *Curl_ssl_get_cf_session(কাঠামো Curl_easy *তথ্য,
                                                 ধ্রুবক কাঠামো Curl_cftype *cft,
                                                 int8_t sockindex);

#endif /* USE_SSL */

#endif /* HEADER_CURL_VTLS_INT_H */

#ifdef USE_SSL
/* Restore the default SSL filter call_data accessor for unity builds. */
#undef CF_CTX_CALL_তথ্য
#define CF_CTX_CALL_তথ্য(cf) ((কাঠামো ssl_connect_তথ্য *)(cf)->ctx)->call_তথ্য
#endif
