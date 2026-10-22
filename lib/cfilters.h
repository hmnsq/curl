#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CFILTERS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CFILTERS_H
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
#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"

ರಚನೆ bufq;
ರಚನೆ Curl_cfilter;
ರಚನೆ Curl_easy;
ರಚನೆ Curl_dns_ನಮೂದು;
ರಚನೆ connectdata;
ರಚನೆ ip_quadruple;
ರಚನೆ curl_tlssessioninfo;

/* Callback to destroy resources held by this filter instance.
 * Implementations MUST NOT chain calls to cf->next.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ     Curl_cft_destroy_this(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Callback to close the connection filter gracefully, non-blocking.
 * Implementations MUST NOT chain calls to cf->next.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_shutdown(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   bool *ಮುಗಿದ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_connect(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool *ಮುಗಿದ);

ರಚನೆ easy_pollset;

/* Passing in an easy_pollset for monitoring of sockets, let
 * filters add or remove sockets actions (CURL_POLL_OUT, CURL_POLL_IN).
 * This may add a socket or, in case no actions remain, remove
 * a socket from the set.
 *
 * Filter implementations need to call filters "below" *after* they have
 * made their adjustments. This allows lower filters to override "upper"
 * actions. If a "lower" filter is unable to write, it needs to be able
 * to disallow POLL_OUT.
 *
 * A filter without own restrictions/preferences should not modify
 * the pollset. Filters, whose filter "below" is not connected, should
 * also do no adjustments.
 *
 * Examples: a TLS handshake, while ongoing, might remove POLL_IN when it
 * needs to write, or vice versa. An HTTP/2 filter might remove POLL_OUT when
 * a stream window is exhausted and a WINDOW_UPDATE needs to be received first
 * and add instead POLL_IN.
 *
 * @param cf     the filter to ask
 * @param data   the easy handle the pollset is about
 * @param ps     the pollset (inout) for the easy handle
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ easy_pollset *ps);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool     Curl_cft_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                       ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_send(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, /* transfer */
                               ಸ್ಥಿರಾಂಕ uint8_t *buf,     /* data to write */
                               size_t len,             /* amount to write */
                               bool eos,               /* last chunk */
                               size_t *pnwritten);     /* how much sent */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_recv(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, /* transfer */
                               ಅಕ್ಷರ *buf,              /* store data here */
                               size_t len,             /* amount to read */
                               size_t *pnread);        /* how much received */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool     Curl_cft_conn_is_alive(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        bool *ಒಳಹರಿವು_pending);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_conn_keep_alive(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * Events/controls for connection filters, their arguments and
 * return code handling. Filter callbacks are invoked "top down".
 * Return code handling:
 * "first fail" meaning that the first filter returning != CURLE_OK, will
 *              abort further event distribution and determine the result.
 * "ignored" meaning return values are ignored and the event is distributed
 *           to all filters in the chain. Overall result is always CURLE_OK.
 */
/*      data event                            arg1       arg2     return */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ              4  /* 0          NULL     first fail */
/* unused now                           5 */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_ದತ್ತಾಂಶ_PAUSE              6  /* on/off     NULL     first fail */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ               7  /* premature  NULL     ignored */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ_SEND          8  /* 0          NULL     ignored */
/* update conn info at connection and data */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_CONN_INFO_UPDATE (256 + 0) /* 0          NULL     ignored */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_FORGET_SOCKET    (256 + 1) /* 0          NULL     ignored */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_FLUSH            (256 + 2) /* 0          NULL     first fail */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTRL_REPORT_STATS     (256 + 3) /* 0          NULL     ignored */

/**
 * Handle event/control for the filter.
 * Implementations MUST NOT chain calls to cf->next.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_cntrl(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2);

/**
 * Queries to ask via a `Curl_cft_query *query` method on a cfilter chain.
 * - MAX_CONCURRENT: the maximum number of parallel transfers the filter
 *                   chain expects to handle at the same time.
 *                   default: 1 if no filter overrides.
 * - CONNECT_REPLY_MS: milliseconds until the first indication of a server
 *                   response was received on a connect. For TCP, this
 *                   reflects the time until the socket connected. On UDP
 *                   this gives the time the first bytes from the server
 *                   were received.
 *                   -1 if not determined yet.
 * - CF_QUERY_SOCKET: the socket used by the filter chain
 * - CF_QUERY_NEED_FLUSH: TRUE iff any of the filters have unsent data
 * - CF_QUERY_IP_INFO: res1 says if connection used IPv6, res2 is the
 *                   ip quadruple
 * - CF_QUERY_HOST_PORT: the remote hostname and port a filter talks to
 * - CF_QUERY_SSL_INFO: fill out the passed curl_tlssessioninfo with the
 *                      internal from the SSL secured connection when
 *                      available.
 * - CF_QUERY_SSL_CTX_INFO: same as CF_QUERY_SSL_INFO, but give the SSL_CTX
 *                      when available, or the same internal pointer
 *                      when the TLS stack does not differentiate.
 * - CF_QUERY_ALPN_NEGOTIATED: The ALPN selected by the server as
                        null-terminated string or NULL if none
                        selected/handshake not done. Implemented by filter
                        types CF_TYPE_SSL or CF_TYPE_IP_CONNECT.
 * - CF_QUERY_REALLY_CONNECTED: implemented in socket filters to return
 *                      if a reply from a server has really arrived. For
 *                      non-UDP sockets this is TRUE when the socket became
 *                      writable. For UDP sockets, this is TRUE when the
 *                      first byte from the peer was received.
 */
/*      query                             res1       res2     */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_MAX_CONCURRENT     1  /* number     -        */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_CONNECT_ಉತ್ತರ_MS   2  /* number     -        */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_SOCKET             3  /* -          curl_socket_t */
/* unused 4 + 5 */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_ಹರಿವು_ERROR       6  /* error code - */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_NEED_FLUSH         7  /* TRUE/FALSE - */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_IP_INFO            8  /* TRUE/FALSE struct ip_quadruple */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_HTTP_ಆವೃತ್ತಿ       9  /* number (10/11/20/30)   - */
/* pass in a `const struct Curl_sockaddr_ex **` as `pres2`. Gets set
 * to NULL when not connected. */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_REMOTE_ADDR       10  /* -          `Curl_sockaddr_ex *` */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_HOST_ಪೋರ್ಟ್         11  /* port       const char * */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_SSL_INFO          12  /* -    struct curl_tlssessioninfo * */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_SSL_CTX_INFO      13  /* -    struct curl_tlssessioninfo * */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_TRANSPORT         14  /* TRNSPRT_*  - * */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_ALPN_NEGOTIATED   15  /* -          const char * */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_QUERY_REALLY_CONNECTED  16  /* TRUE/FALSE - */

/**
 * Query the cfilter for properties. Filters ignorant of a query will
 * pass it "down" the filter chain.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_cft_query(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2);

/**
 * Type flags for connection filters. A filter can have none, one or
 * many of those. Use to evaluate state/capabilities of a filter chain.
 *
 * CF_TYPE_IP_CONNECT: provides an IP connection or sth equivalent, like
 *                     a CONNECT tunnel, a UNIX domain socket, a QUIC
 *                     connection, etc.
 * CF_TYPE_SSL:        provide SSL/TLS
 * CF_TYPE_MULTIPLEX:  provides multiplexing of easy handles
 * CF_TYPE_PROXY       provides proxying
 * CF_TYPE_HTTP        implement a version of the HTTP protocol
 * CF_TYPE_SETUP       filter is only needed for connection setup and
 *                     can be removed once connected
 * CF_TYPE_DNS         filter managing DNS resolve operations
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CF_TYPE_IP_CONNECT  (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CF_TYPE_SSL         (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು CF_TYPE_MULTIPLEX   (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು CF_TYPE_PROXY       (1 << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು CF_TYPE_HTTP        (1 << 4)
#ವ್ಯಾಖ್ಯಾನಿಸು CF_TYPE_ಸಿದ್ಧತೆ       (1 << 5)
#ವ್ಯಾಖ್ಯಾನಿಸು CF_TYPE_DNS         (1 << 6)

/* A connection filter type, e.g. specific implementation. */
ರಚನೆ Curl_cftype {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;                        /* name of the filter type */
  ಪೂರ್ಣಾಂಕ flags;                               /* flags of filter type */
  ಪೂರ್ಣಾಂಕ log_level;                           /* log level for such filters */
  Curl_cft_destroy_this *destroy;          /* destroy resources of this cf */
  Curl_cft_connect *do_connect;            /* establish connection */
  Curl_cft_shutdown *do_shutdown;          /* shutdown conn */
  Curl_cft_adjust_pollset *adjust_pollset; /* adjust transfer poll set */
  Curl_cft_ದತ್ತಾಂಶ_pending *has_ದತ್ತಾಂಶ_pending; /* conn has data pending */
  Curl_cft_send *do_send;                  /* send data */
  Curl_cft_recv *do_recv;                  /* receive data */
  Curl_cft_cntrl *cntrl;                   /* events/control */
  Curl_cft_conn_is_alive *is_alive;        /* FALSE if conn is dead, Jim! */
  Curl_cft_conn_keep_alive *keep_alive;    /* try to keep it alive */
  Curl_cft_query *query;                   /* query filter chain */
};

/* A connection filter instance, e.g. registered at a connection */
ರಚನೆ Curl_cfilter {
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cftype *cft; /* the type providing implementation */
  ರಚನೆ Curl_cfilter *ಮುಂದಿನ;     /* next filter in chain */
  ಶೂನ್ಯ *ctx;                     /* filter type specific settings */
  ರಚನೆ connectdata *conn;      /* the connection this filter belongs to */
  int8_t sockindex;                 /* the index the filter is installed at */
  ಬಿಟ್(connected);                /* != 0 iff this filter is connected */
  ಬಿಟ್(shutdown);                 /* != 0 iff this filter has shut down */
};

/* Default implementations for the type functions, implementing nop. */
ಶೂನ್ಯ Curl_cf_def_destroy_this(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Default implementations for the type functions, implementing pass-through
 * the filter chain. */
CURLcode Curl_cf_def_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ easy_pollset *ps);
bool     Curl_cf_def_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_cf_def_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten);
CURLcode Curl_cf_def_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಅಕ್ಷರ *buf, size_t len, size_t *pnread);
CURLcode Curl_cf_def_cntrl(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2);
bool     Curl_cf_def_conn_is_alive(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   bool *ಒಳಹರಿವು_pending);
CURLcode Curl_cf_def_conn_keep_alive(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_cf_def_query(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2);
CURLcode Curl_cf_def_shutdown(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);
CURLcode Curl_cf_def_connect(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);

/**
 * Create a new filter instance, unattached to the filter chain.
 * Use Curl_conn_cf_add() to add it to the chain.
 * @param pcf  on success holds the created instance
 * @param cft   the filter type
 * @param ctx  the type specific context to use
 */
CURLcode Curl_cf_create(ರಚನೆ Curl_cfilter **pcf,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cftype *cft,
                        ಶೂನ್ಯ *ctx);

/**
 * Add a filter instance to the `sockindex` filter chain at connection
 * `conn`. The filter must not already be attached. It is inserted at
 * the start of the chain (top).
 */
ಶೂನ್ಯ Curl_conn_cf_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ connectdata *conn,
                      int8_t sockindex,
                      ರಚನೆ Curl_cfilter *cf);

/**
 * Insert a filter (chain) after `cf_at`.
 * `cf_new` must not already be attached.
 */
ಶೂನ್ಯ Curl_conn_cf_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                               ರಚನೆ Curl_cfilter *cf_new);

/**
 * Extract filter `*pcf` from its connection filter chain.
 * Destroy `*pcf`, even if it was not part of the chain and NULL it.
 * Returns TRUE of cf has been part of chain.
 */
bool Curl_conn_cf_discard(ರಚನೆ Curl_cfilter **pcf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * Discard all cfilters starting with `*pcf` and clearing it afterwards.
 */
ಶೂನ್ಯ Curl_conn_cf_discard_chain(ರಚನೆ Curl_cfilter **pcf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * Remove and destroy all filters at chain `sockindex` on connection `conn`.
 */
ಶೂನ್ಯ Curl_conn_cf_discard_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn, int8_t sockindex);

CURLcode Curl_conn_cf_connect(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool *ಮುಗಿದ);
CURLcode Curl_conn_cf_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten);
CURLcode Curl_conn_cf_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಅಕ್ಷರ *buf, size_t len, size_t *pnread);
CURLcode Curl_conn_cf_cntrl(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool ignore_ಫಲಿತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2);

/**
 * Get the socket used by the filter chain starting at `cf`.
 * Returns CURL_SOCKET_BAD if not available.
 */
curl_socket_t Curl_conn_cf_get_socket(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_conn_cf_get_ip_info(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool *is_ipv6, ರಚನೆ ip_quadruple *ipquad);

bool Curl_conn_cf_needs_flush(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

bool Curl_conn_cf_is_ip_connected(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_conn_cf_get_transport(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಪೂರ್ಣಾಂಕ Curl_socktype_for_transport(uint8_t transport);
ಪೂರ್ಣಾಂಕ Curl_protocol_for_transport(uint8_t transport);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_conn_cf_get_alpn_negotiated(ರಚನೆ Curl_cfilter *cf,
                                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CF_SSL_DEFAULT  (-1)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CF_SSL_DISABLE  0
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CF_SSL_ENABLE   1

/**
 * Check if a filter chain at `sockindex` for connection `conn` exists.
 */
bool Curl_conn_is_ಸಿದ್ಧತೆ(ರಚನೆ connectdata *conn, int8_t sockindex);

/**
 * Check if the filter chain at `sockindex` for connection `conn` is
 * completely connected.
 */
bool Curl_conn_is_connected(ರಚನೆ connectdata *conn, int8_t sockindex);

/**
 * Determine if we have reached the remote host on IP level, e.g.
 * have a TCP connection. This turns TRUE before a possible SSL
 * handshake has been started/done.
 */
bool Curl_conn_is_ip_connected(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex);

/**
 * Determine if the connection is using SSL to the remote host
 * (or will be once connected). This will return FALSE, if SSL
 * is only used in proxying and not for the tunnel itself.
 */
bool Curl_conn_is_ssl(ರಚನೆ connectdata *conn, int8_t sockindex);

/* Determine if the connection has one or more proxy filters.
 * e.g. is tunneling. */
bool Curl_conn_is_tunneling(ರಚನೆ connectdata *conn, int8_t sockindex);

/*
 * Fill `info` with information about the TLS instance securing the connection
 * when available, otherwise e.g. when Curl_conn_is_ssl() is FALSE, return
 * FALSE. 'query' should be CF_QUERY_SSL_INFO or CF_QUERY_SSL_CTX_INFO.
 */
bool Curl_conn_get_ssl_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ connectdata *conn, int8_t sockindex,
                            ಪೂರ್ಣಾಂಕ query,
                            ರಚನೆ curl_tlssessioninfo *info);

CURLcode Curl_conn_get_ip_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ರಚನೆ ip_quadruple *ipquad);

/**
 * Connection provides multiplexing of easy handles at `socketindex`.
 */
bool Curl_conn_is_multiplex(ರಚನೆ connectdata *conn, int8_t sockindex);

/**
 * Return the HTTP version used on the FIRSTSOCKET connection filters
 * or 0 if unknown. Value otherwise is 09, 10, 11, etc.
 */
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_conn_http_ಆವೃತ್ತಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn);

/* Get the TRNSPRT_* the connection is using */
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Curl_conn_get_transport(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn);

/* Get the negotiated ALPN protocol or NULL if none in play */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_conn_get_alpn_negotiated(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ connectdata *conn);

ಶೂನ್ಯ Curl_conn_cntrl_update_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ connectdata *conn);

ಶೂನ್ಯ Curl_conn_cntrl_report_stats(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ಪೂರ್ಣಾಂಕ sockindex);

ಶೂನ್ಯ Curl_conn_remove_ಸಿದ್ಧತೆ_filters(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    int8_t sockindex);

/**
 * Return if data is pending in some connection filter at chain
 * `sockindex` for connection `data->conn`.
 */
bool Curl_conn_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            int8_t sockindex);

/**
 * Return TRUE if any of the connection filters at chain `sockindex`
 * have data still to send.
 */
bool Curl_conn_needs_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex);

/**
 * Flush any pending data on the connection filters at chain `sockindex`.
 */
CURLcode Curl_conn_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex);

/**
 * Return the socket used on data's connection for FIRSTSOCKET,
 * querying filters if the whole chain has not connected yet.
 * Returns CURL_SOCKET_BAD if not available.
 */
curl_socket_t Curl_conn_get_ಮೊದಲ_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Return a pointer to the connected socket address or NULL. */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *
Curl_conn_get_remote_addr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex);

/**
 * Tell filters to forget about the socket at sockindex.
 */
ಶೂನ್ಯ Curl_conn_forget_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex);

/**
 * Adjust the pollset for the filter chain starting at `cf`.
 */
CURLcode Curl_conn_cf_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ easy_pollset *ps);

/**
 * Adjust pollset from filters installed at transfer's connection.
 */
CURLcode Curl_conn_adjust_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ರಚನೆ easy_pollset *ps);

/**
 * Curl_poll() the filter chain at `cf` with timeout `timeout_ms`.
 * Returns 0 on timeout, negative on error or number of sockets
 * with requested poll events.
 */
ಪೂರ್ಣಾಂಕ Curl_conn_cf_poll(ರಚನೆ Curl_cfilter *cf,
                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      timediff_t timeout_ms);

/**
 * Receive data through the filter chain at `sockindex` for connection
 * `data->conn`. Copy at most `len` bytes into `buf`. Return the
 * actual number of bytes copied in `*pnread`or an error.
 */
CURLcode Curl_cf_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex, ಅಕ್ಷರ *buf,
                      size_t len, size_t *pnread);

/**
 * Send `len` bytes of data from `buf` through the filter chain `sockindex`
 * at connection `data->conn`. Return the actual number of bytes written
 * in `*pnwritten` or on error.
 */
CURLcode Curl_cf_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                      ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten);

/**
 * Receive bytes from connection filter `cf` into `bufq`.
 * Convenience wrapper around `Curl_bufq_sipn()`,
 * so users do not have to implement a callback.
 */
CURLcode Curl_cf_recv_bufq(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ bufq *bufq,
                           size_t maxlen,
                           size_t *pnread);

/**
 * Send bytes in `bufq` using connection filter `cf`.
 * A convenience wrapper around `Curl_bufq_write_pass()`,
 * so users do not have to implement a callback.
 */
CURLcode Curl_cf_send_bufq(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ bufq *bufq,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t blen,
                           size_t *pnwritten);

/**
 * Notify connection filters that they need to setup data for
 * a transfer.
 */
CURLcode Curl_conn_ev_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
ಶೂನ್ಯ Curl_conn_ev_ದತ್ತಾಂಶ_ಮುಗಿದ_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
ಶೂನ್ಯ Curl_conn_ev_ದತ್ತಾಂಶ_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool premature);

/**
 * Notify connection filters that the transfer of data is paused/unpaused.
 */
CURLcode Curl_conn_ev_ದತ್ತಾಂಶ_pause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool do_pause);

/**
 * Check if FIRSTSOCKET's cfilter chain deems connection alive.
 */
bool Curl_conn_is_alive(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                        bool *ಒಳಹರಿವು_pending);

/**
 * Try to upkeep the connection filters.
 */
CURLcode Curl_conn_keep_alive(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn);

/**
 * Get the remote hostname and port that the connection is currently
 * talking to (or will talk to).
 * Once connected or before connect starts,
 * it is `conn->origin->hostname` and `conn->origin->port`.
 * During connect, when tunneling proxies are involved (http or socks),
 * it will be the name and port the proxy currently negotiates with.
 */
ಶೂನ್ಯ Curl_conn_get_current_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **phost, ಪೂರ್ಣಾಂಕ *pport);

/**
 * Get the maximum number of parallel transfers the connection
 * expects to be able to handle at `sockindex`.
 */
size_t Curl_conn_get_max_concurrent(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn,
                                    int8_t sockindex);

/**
 * Get the underlying error code for a transfer stream or 0 if not known.
 */
ಪೂರ್ಣಾಂಕ Curl_conn_get_ಹರಿವು_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               int8_t sockindex);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಶೂನ್ಯ Curl_conn_trc_filters(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           int8_t sockindex, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *info);
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Get the index of the given socket in the connection's sockets.
 * Useful in calling `Curl_conn_send()/Curl_conn_recv()` with the
 * correct socket index.
 */
int8_t Curl_conn_sockindex(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_socket_t sockfd);

/*
 * Receive data on the connection, using FIRSTSOCKET/SECONDARYSOCKET.
 * Return CURLE_AGAIN iff blocked on receiving.
 */
CURLcode Curl_conn_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                        ಅಕ್ಷರ *buf, size_t len, size_t *pnread);

/*
 * Send data on the connection, using FIRSTSOCKET/SECONDARYSOCKET.
 * Return CURLE_AGAIN iff blocked on sending.
 */
CURLcode Curl_conn_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t len, bool eos,
                        size_t *pnwritten);

/**
 * Types and macros used to keep the current easy handle in filter calls,
 * allowing for nested invocations. See #10336.
 *
 * `cf_call_data` is intended to be a member of the cfilter's `ctx` type.
 * A filter defines the macro `CF_CTX_CALL_DATA` to give access to that.
 *
 * With all values 0, the default, this indicates that there is no cfilter
 * call with `data` ongoing.
 * Macro `CF_DATA_SAVE` preserves the current `cf_call_data` in a local
 * variable and sets the `data` given, incrementing the `depth` counter.
 *
 * Macro `CF_DATA_RESTORE` restores the old values from the local variable,
 * while checking that `depth` values are as expected (debug build), catching
 * cases where a "lower" RESTORE was not called.
 *
 * Finally, macro `CF_DATA_CURRENT` gives the easy handle of the current
 * invocation.
 */
ರಚನೆ cf_call_ದತ್ತಾಂಶ {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ ಆಳ;
#ಆದರೆ_ಅಂತ್ಯ
};

/**
 * define to access the `struct cf_call_data for a cfilter. Normally
 * a member in the cfilter's `ctx`.
 *
 * #define CF_CTX_CALL_DATA(cf)   -> struct cf_call_data instance
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD

#ವ್ಯಾಖ್ಯಾನಿಸು CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ) \
  ಮಾಡು { \
    (save) = CF_CTX_CALL_ದತ್ತಾಂಶ(cf); \
    DEBUGASSERT(!(save).ದತ್ತಾಂಶ || (save).ಆಳ > 0); \
    CF_CTX_CALL_ದತ್ತಾಂಶ(cf).ಆಳ++;  \
    CF_CTX_CALL_ದತ್ತಾಂಶ(cf).ದತ್ತಾಂಶ = (ರಚನೆ Curl_easy *)CURL_UNCONST(ದತ್ತಾಂಶ); \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು CF_ದತ್ತಾಂಶ_RESTORE(cf, save) \
  ಮಾಡು { \
    DEBUGASSERT(CF_CTX_CALL_ದತ್ತಾಂಶ(cf).ಆಳ == (save).ಆಳ + 1); \
    DEBUGASSERT(!(save).ದತ್ತಾಂಶ || (save).ಆಳ > 0); \
    CF_CTX_CALL_ದತ್ತಾಂಶ(cf) = (save); \
  } ಆಗ(0)

#ಇಲ್ಲದಿದ್ದರೆ /* DEBUGBUILD */

#ವ್ಯಾಖ್ಯಾನಿಸು CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ) \
  ಮಾಡು { \
    (save) = CF_CTX_CALL_ದತ್ತಾಂಶ(cf); \
    CF_CTX_CALL_ದತ್ತಾಂಶ(cf).ದತ್ತಾಂಶ = (ರಚನೆ Curl_easy *)CURL_UNCONST(ದತ್ತಾಂಶ); \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು CF_ದತ್ತಾಂಶ_RESTORE(cf, save) \
  ಮಾಡು { \
    CF_CTX_CALL_ದತ್ತಾಂಶ(cf) = (save); \
  } ಆಗ(0)

#ಆದರೆ_ಅಂತ್ಯ /* !DEBUGBUILD */

#ವ್ಯಾಖ್ಯಾನಿಸು CF_ದತ್ತಾಂಶ_CURRENT(cf) ((cf) ? (CF_CTX_CALL_ದತ್ತಾಂಶ(cf).ದತ್ತಾಂಶ) : ಖಾಲಿ)

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CFILTERS_H */
