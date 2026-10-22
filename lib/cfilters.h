#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CFILTERS_H
#నిర్వచించు శీర్షిక_CURL_CFILTERS_H
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
#అన్తర్భావ "curlx/timediff.h"

నిర్మాణం bufq;
నిర్మాణం Curl_cfilter;
నిర్మాణం Curl_easy;
నిర్మాణం Curl_dns_నమోదు;
నిర్మాణం connectdata;
నిర్మాణం ip_quadruple;
నిర్మాణం curl_tlssessioninfo;

/* Callback to destroy resources held by this filter instance.
 * Implementations MUST NOT chain calls to cf->next.
 */
రకనిర్వచనం శూన్య     Curl_cft_destroy_this(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం);

/* Callback to close the connection filter gracefully, non-blocking.
 * Implementations MUST NOT chain calls to cf->next.
 */
రకనిర్వచనం CURLcode Curl_cft_shutdown(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   bool *పూర్తి);

రకనిర్వచనం CURLcode Curl_cft_connect(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool *పూర్తి);

నిర్మాణం easy_pollset;

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
రకనిర్వచనం CURLcode Curl_cft_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం easy_pollset *ps);

రకనిర్వచనం bool     Curl_cft_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                       స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం);

రకనిర్వచనం CURLcode Curl_cft_send(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం, /* transfer */
                               స్థిరరాశి uint8_t *buf,     /* data to write */
                               size_t len,             /* amount to write */
                               bool eos,               /* last chunk */
                               size_t *pnwritten);     /* how much sent */

రకనిర్వచనం CURLcode Curl_cft_recv(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం, /* transfer */
                               అక్షరం *buf,              /* store data here */
                               size_t len,             /* amount to read */
                               size_t *pnread);        /* how much received */

రకనిర్వచనం bool     Curl_cft_conn_is_alive(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        bool *ఆదానం_pending);

రకనిర్వచనం CURLcode Curl_cft_conn_keep_alive(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం);

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
#నిర్వచించు CF_CTRL_దత్తాంశం_అమరిక              4  /* 0          NULL     first fail */
/* unused now                           5 */
#నిర్వచించు CF_CTRL_దత్తాంశం_PAUSE              6  /* on/off     NULL     first fail */
#నిర్వచించు CF_CTRL_దత్తాంశం_పూర్తి               7  /* premature  NULL     ignored */
#నిర్వచించు CF_CTRL_దత్తాంశం_పూర్తి_SEND          8  /* 0          NULL     ignored */
/* update conn info at connection and data */
#నిర్వచించు CF_CTRL_CONN_INFO_UPDATE (256 + 0) /* 0          NULL     ignored */
#నిర్వచించు CF_CTRL_FORGET_SOCKET    (256 + 1) /* 0          NULL     ignored */
#నిర్వచించు CF_CTRL_FLUSH            (256 + 2) /* 0          NULL     first fail */
#నిర్వచించు CF_CTRL_REPORT_STATS     (256 + 3) /* 0          NULL     ignored */

/**
 * Handle event/control for the filter.
 * Implementations MUST NOT chain calls to cf->next.
 */
రకనిర్వచనం CURLcode Curl_cft_cntrl(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2);

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
#నిర్వచించు CF_QUERY_MAX_CONCURRENT     1  /* number     -        */
#నిర్వచించు CF_QUERY_CONNECT_జవాబు_MS   2  /* number     -        */
#నిర్వచించు CF_QUERY_SOCKET             3  /* -          curl_socket_t */
/* unused 4 + 5 */
#నిర్వచించు CF_QUERY_ప్రవాహం_ERROR       6  /* error code - */
#నిర్వచించు CF_QUERY_NEED_FLUSH         7  /* TRUE/FALSE - */
#నిర్వచించు CF_QUERY_IP_INFO            8  /* TRUE/FALSE struct ip_quadruple */
#నిర్వచించు CF_QUERY_HTTP_సంస్కరణ       9  /* number (10/11/20/30)   - */
/* pass in a `const struct Curl_sockaddr_ex **` as `pres2`. Gets set
 * to NULL when not connected. */
#నిర్వచించు CF_QUERY_REMOTE_ADDR       10  /* -          `Curl_sockaddr_ex *` */
#నిర్వచించు CF_QUERY_HOST_పోర్టు         11  /* port       const char * */
#నిర్వచించు CF_QUERY_SSL_INFO          12  /* -    struct curl_tlssessioninfo * */
#నిర్వచించు CF_QUERY_SSL_CTX_INFO      13  /* -    struct curl_tlssessioninfo * */
#నిర్వచించు CF_QUERY_TRANSPORT         14  /* TRNSPRT_*  - * */
#నిర్వచించు CF_QUERY_ALPN_NEGOTIATED   15  /* -          const char * */
#నిర్వచించు CF_QUERY_REALLY_CONNECTED  16  /* TRUE/FALSE - */

/**
 * Query the cfilter for properties. Filters ignorant of a query will
 * pass it "down" the filter chain.
 */
రకనిర్వచనం CURLcode Curl_cft_query(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2);

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
#నిర్వచించు CF_TYPE_IP_CONNECT  (1 << 0)
#నిర్వచించు CF_TYPE_SSL         (1 << 1)
#నిర్వచించు CF_TYPE_MULTIPLEX   (1 << 2)
#నిర్వచించు CF_TYPE_PROXY       (1 << 3)
#నిర్వచించు CF_TYPE_HTTP        (1 << 4)
#నిర్వచించు CF_TYPE_అమరిక       (1 << 5)
#నిర్వచించు CF_TYPE_DNS         (1 << 6)

/* A connection filter type, e.g. specific implementation. */
నిర్మాణం Curl_cftype {
  స్థిరరాశి అక్షరం *name;                        /* name of the filter type */
  పూర్ణాంకం flags;                               /* flags of filter type */
  పూర్ణాంకం log_level;                           /* log level for such filters */
  Curl_cft_destroy_this *destroy;          /* destroy resources of this cf */
  Curl_cft_connect *do_connect;            /* establish connection */
  Curl_cft_shutdown *do_shutdown;          /* shutdown conn */
  Curl_cft_adjust_pollset *adjust_pollset; /* adjust transfer poll set */
  Curl_cft_దత్తాంశం_pending *has_దత్తాంశం_pending; /* conn has data pending */
  Curl_cft_send *do_send;                  /* send data */
  Curl_cft_recv *do_recv;                  /* receive data */
  Curl_cft_cntrl *cntrl;                   /* events/control */
  Curl_cft_conn_is_alive *is_alive;        /* FALSE if conn is dead, Jim! */
  Curl_cft_conn_keep_alive *keep_alive;    /* try to keep it alive */
  Curl_cft_query *query;                   /* query filter chain */
};

/* A connection filter instance, e.g. registered at a connection */
నిర్మాణం Curl_cfilter {
  స్థిరరాశి నిర్మాణం Curl_cftype *cft; /* the type providing implementation */
  నిర్మాణం Curl_cfilter *తదుపరి;     /* next filter in chain */
  శూన్య *ctx;                     /* filter type specific settings */
  నిర్మాణం connectdata *conn;      /* the connection this filter belongs to */
  int8_t sockindex;                 /* the index the filter is installed at */
  బిట్టు(connected);                /* != 0 iff this filter is connected */
  బిట్టు(shutdown);                 /* != 0 iff this filter has shut down */
};

/* Default implementations for the type functions, implementing nop. */
శూన్య Curl_cf_def_destroy_this(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం);

/* Default implementations for the type functions, implementing pass-through
 * the filter chain. */
CURLcode Curl_cf_def_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం easy_pollset *ps);
bool     Curl_cf_def_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                  స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_cf_def_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten);
CURLcode Curl_cf_def_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          అక్షరం *buf, size_t len, size_t *pnread);
CURLcode Curl_cf_def_cntrl(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2);
bool     Curl_cf_def_conn_is_alive(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   bool *ఆదానం_pending);
CURLcode Curl_cf_def_conn_keep_alive(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_cf_def_query(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2);
CURLcode Curl_cf_def_shutdown(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);
CURLcode Curl_cf_def_connect(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);

/**
 * Create a new filter instance, unattached to the filter chain.
 * Use Curl_conn_cf_add() to add it to the chain.
 * @param pcf  on success holds the created instance
 * @param cft   the filter type
 * @param ctx  the type specific context to use
 */
CURLcode Curl_cf_create(నిర్మాణం Curl_cfilter **pcf,
                        స్థిరరాశి నిర్మాణం Curl_cftype *cft,
                        శూన్య *ctx);

/**
 * Add a filter instance to the `sockindex` filter chain at connection
 * `conn`. The filter must not already be attached. It is inserted at
 * the start of the chain (top).
 */
శూన్య Curl_conn_cf_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం connectdata *conn,
                      int8_t sockindex,
                      నిర్మాణం Curl_cfilter *cf);

/**
 * Insert a filter (chain) after `cf_at`.
 * `cf_new` must not already be attached.
 */
శూన్య Curl_conn_cf_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                               నిర్మాణం Curl_cfilter *cf_new);

/**
 * Extract filter `*pcf` from its connection filter chain.
 * Destroy `*pcf`, even if it was not part of the chain and NULL it.
 * Returns TRUE of cf has been part of chain.
 */
bool Curl_conn_cf_discard(నిర్మాణం Curl_cfilter **pcf,
                          నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Discard all cfilters starting with `*pcf` and clearing it afterwards.
 */
శూన్య Curl_conn_cf_discard_chain(నిర్మాణం Curl_cfilter **pcf,
                                నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Remove and destroy all filters at chain `sockindex` on connection `conn`.
 */
శూన్య Curl_conn_cf_discard_all(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn, int8_t sockindex);

CURLcode Curl_conn_cf_connect(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool *పూర్తి);
CURLcode Curl_conn_cf_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                           స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten);
CURLcode Curl_conn_cf_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                           అక్షరం *buf, size_t len, size_t *pnread);
CURLcode Curl_conn_cf_cntrl(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            bool ignore_ఫలితం,
                            పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2);

/**
 * Get the socket used by the filter chain starting at `cf`.
 * Returns CURL_SOCKET_BAD if not available.
 */
curl_socket_t Curl_conn_cf_get_socket(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_conn_cf_get_ip_info(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool *is_ipv6, నిర్మాణం ip_quadruple *ipquad);

bool Curl_conn_cf_needs_flush(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం);

bool Curl_conn_cf_is_ip_connected(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం);

గుర్తులేని అక్షరం Curl_conn_cf_get_transport(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం);

పూర్ణాంకం Curl_socktype_for_transport(uint8_t transport);
పూర్ణాంకం Curl_protocol_for_transport(uint8_t transport);

స్థిరరాశి అక్షరం *Curl_conn_cf_get_alpn_negotiated(నిర్మాణం Curl_cfilter *cf,
                                             నిర్మాణం Curl_easy *దత్తాంశం);

#నిర్వచించు CURL_CF_SSL_DEFAULT  (-1)
#నిర్వచించు CURL_CF_SSL_DISABLE  0
#నిర్వచించు CURL_CF_SSL_ENABLE   1

/**
 * Check if a filter chain at `sockindex` for connection `conn` exists.
 */
bool Curl_conn_is_అమరిక(నిర్మాణం connectdata *conn, int8_t sockindex);

/**
 * Check if the filter chain at `sockindex` for connection `conn` is
 * completely connected.
 */
bool Curl_conn_is_connected(నిర్మాణం connectdata *conn, int8_t sockindex);

/**
 * Determine if we have reached the remote host on IP level, e.g.
 * have a TCP connection. This turns TRUE before a possible SSL
 * handshake has been started/done.
 */
bool Curl_conn_is_ip_connected(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex);

/**
 * Determine if the connection is using SSL to the remote host
 * (or will be once connected). This will return FALSE, if SSL
 * is only used in proxying and not for the tunnel itself.
 */
bool Curl_conn_is_ssl(నిర్మాణం connectdata *conn, int8_t sockindex);

/* Determine if the connection has one or more proxy filters.
 * e.g. is tunneling. */
bool Curl_conn_is_tunneling(నిర్మాణం connectdata *conn, int8_t sockindex);

/*
 * Fill `info` with information about the TLS instance securing the connection
 * when available, otherwise e.g. when Curl_conn_is_ssl() is FALSE, return
 * FALSE. 'query' should be CF_QUERY_SSL_INFO or CF_QUERY_SSL_CTX_INFO.
 */
bool Curl_conn_get_ssl_info(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం connectdata *conn, int8_t sockindex,
                            పూర్ణాంకం query,
                            నిర్మాణం curl_tlssessioninfo *info);

CURLcode Curl_conn_get_ip_info(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, నిర్మాణం ip_quadruple *ipquad);

/**
 * Connection provides multiplexing of easy handles at `socketindex`.
 */
bool Curl_conn_is_multiplex(నిర్మాణం connectdata *conn, int8_t sockindex);

/**
 * Return the HTTP version used on the FIRSTSOCKET connection filters
 * or 0 if unknown. Value otherwise is 09, 10, 11, etc.
 */
గుర్తులేని అక్షరం Curl_conn_http_సంస్కరణ(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn);

/* Get the TRNSPRT_* the connection is using */
గుర్తులేని అక్షరం Curl_conn_get_transport(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn);

/* Get the negotiated ALPN protocol or NULL if none in play */
స్థిరరాశి అక్షరం *Curl_conn_get_alpn_negotiated(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం connectdata *conn);

శూన్య Curl_conn_cntrl_update_info(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం connectdata *conn);

శూన్య Curl_conn_cntrl_report_stats(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  పూర్ణాంకం sockindex);

శూన్య Curl_conn_remove_అమరిక_filters(నిర్మాణం Curl_easy *దత్తాంశం,
                                    int8_t sockindex);

/**
 * Return if data is pending in some connection filter at chain
 * `sockindex` for connection `data->conn`.
 */
bool Curl_conn_దత్తాంశం_pending(నిర్మాణం Curl_easy *దత్తాంశం,
                            int8_t sockindex);

/**
 * Return TRUE if any of the connection filters at chain `sockindex`
 * have data still to send.
 */
bool Curl_conn_needs_flush(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex);

/**
 * Flush any pending data on the connection filters at chain `sockindex`.
 */
CURLcode Curl_conn_flush(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex);

/**
 * Return the socket used on data's connection for FIRSTSOCKET,
 * querying filters if the whole chain has not connected yet.
 * Returns CURL_SOCKET_BAD if not available.
 */
curl_socket_t Curl_conn_get_మొదటి_socket(నిర్మాణం Curl_easy *దత్తాంశం);

/* Return a pointer to the connected socket address or NULL. */
స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *
Curl_conn_get_remote_addr(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex);

/**
 * Tell filters to forget about the socket at sockindex.
 */
శూన్య Curl_conn_forget_socket(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex);

/**
 * Adjust the pollset for the filter chain starting at `cf`.
 */
CURLcode Curl_conn_cf_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం easy_pollset *ps);

/**
 * Adjust pollset from filters installed at transfer's connection.
 */
CURLcode Curl_conn_adjust_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  నిర్మాణం easy_pollset *ps);

/**
 * Curl_poll() the filter chain at `cf` with timeout `timeout_ms`.
 * Returns 0 on timeout, negative on error or number of sockets
 * with requested poll events.
 */
పూర్ణాంకం Curl_conn_cf_poll(నిర్మాణం Curl_cfilter *cf,
                      నిర్మాణం Curl_easy *దత్తాంశం,
                      timediff_t timeout_ms);

/**
 * Receive data through the filter chain at `sockindex` for connection
 * `data->conn`. Copy at most `len` bytes into `buf`. Return the
 * actual number of bytes copied in `*pnread`or an error.
 */
CURLcode Curl_cf_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex, అక్షరం *buf,
                      size_t len, size_t *pnread);

/**
 * Send `len` bytes of data from `buf` through the filter chain `sockindex`
 * at connection `data->conn`. Return the actual number of bytes written
 * in `*pnwritten` or on error.
 */
CURLcode Curl_cf_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                      స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten);

/**
 * Receive bytes from connection filter `cf` into `bufq`.
 * Convenience wrapper around `Curl_bufq_sipn()`,
 * so users do not have to implement a callback.
 */
CURLcode Curl_cf_recv_bufq(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం bufq *bufq,
                           size_t maxlen,
                           size_t *pnread);

/**
 * Send bytes in `bufq` using connection filter `cf`.
 * A convenience wrapper around `Curl_bufq_write_pass()`,
 * so users do not have to implement a callback.
 */
CURLcode Curl_cf_send_bufq(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం bufq *bufq,
                           స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t blen,
                           size_t *pnwritten);

/**
 * Notify connection filters that they need to setup data for
 * a transfer.
 */
CURLcode Curl_conn_ev_దత్తాంశం_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
శూన్య Curl_conn_ev_దత్తాంశం_పూర్తి_send(నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
శూన్య Curl_conn_ev_దత్తాంశం_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, bool premature);

/**
 * Notify connection filters that the transfer of data is paused/unpaused.
 */
CURLcode Curl_conn_ev_దత్తాంశం_pause(నిర్మాణం Curl_easy *దత్తాంశం, bool do_pause);

/**
 * Check if FIRSTSOCKET's cfilter chain deems connection alive.
 */
bool Curl_conn_is_alive(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                        bool *ఆదానం_pending);

/**
 * Try to upkeep the connection filters.
 */
CURLcode Curl_conn_keep_alive(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn);

/**
 * Get the remote hostname and port that the connection is currently
 * talking to (or will talk to).
 * Once connected or before connect starts,
 * it is `conn->origin->hostname` and `conn->origin->port`.
 * During connect, when tunneling proxies are involved (http or socks),
 * it will be the name and port the proxy currently negotiates with.
 */
శూన్య Curl_conn_get_current_host(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                                స్థిరరాశి అక్షరం **phost, పూర్ణాంకం *pport);

/**
 * Get the maximum number of parallel transfers the connection
 * expects to be able to handle at `sockindex`.
 */
size_t Curl_conn_get_max_concurrent(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn,
                                    int8_t sockindex);

/**
 * Get the underlying error code for a transfer stream or 0 if not known.
 */
పూర్ణాంకం Curl_conn_get_ప్రవాహం_error(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               int8_t sockindex);

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_conn_trc_filters(నిర్మాణం Curl_easy *దత్తాంశం,
                           int8_t sockindex, స్థిరరాశి అక్షరం *info);
#అయితే_ముగింపు

/**
 * Get the index of the given socket in the connection's sockets.
 * Useful in calling `Curl_conn_send()/Curl_conn_recv()` with the
 * correct socket index.
 */
int8_t Curl_conn_sockindex(నిర్మాణం Curl_easy *దత్తాంశం, curl_socket_t sockfd);

/*
 * Receive data on the connection, using FIRSTSOCKET/SECONDARYSOCKET.
 * Return CURLE_AGAIN iff blocked on receiving.
 */
CURLcode Curl_conn_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                        అక్షరం *buf, size_t len, size_t *pnread);

/*
 * Send data on the connection, using FIRSTSOCKET/SECONDARYSOCKET.
 * Return CURLE_AGAIN iff blocked on sending.
 */
CURLcode Curl_conn_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                        స్థిరరాశి శూన్య *buf, size_t len, bool eos,
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
నిర్మాణం cf_call_దత్తాంశం {
  నిర్మాణం Curl_easy *దత్తాంశం;
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం లోతు;
#అయితే_ముగింపు
};

/**
 * define to access the `struct cf_call_data for a cfilter. Normally
 * a member in the cfilter's `ctx`.
 *
 * #define CF_CTX_CALL_DATA(cf)   -> struct cf_call_data instance
 */

#నిర్వచితమైతే DEBUGBUILD

#నిర్వచించు CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం) \
  చేయి { \
    (save) = CF_CTX_CALL_దత్తాంశం(cf); \
    DEBUGASSERT(!(save).దత్తాంశం || (save).లోతు > 0); \
    CF_CTX_CALL_దత్తాంశం(cf).లోతు++;  \
    CF_CTX_CALL_దత్తాంశం(cf).దత్తాంశం = (నిర్మాణం Curl_easy *)CURL_UNCONST(దత్తాంశం); \
  } అయితే(0)

#నిర్వచించు CF_దత్తాంశం_RESTORE(cf, save) \
  చేయి { \
    DEBUGASSERT(CF_CTX_CALL_దత్తాంశం(cf).లోతు == (save).లోతు + 1); \
    DEBUGASSERT(!(save).దత్తాంశం || (save).లోతు > 0); \
    CF_CTX_CALL_దత్తాంశం(cf) = (save); \
  } అయితే(0)

#లేకపోతే /* DEBUGBUILD */

#నిర్వచించు CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం) \
  చేయి { \
    (save) = CF_CTX_CALL_దత్తాంశం(cf); \
    CF_CTX_CALL_దత్తాంశం(cf).దత్తాంశం = (నిర్మాణం Curl_easy *)CURL_UNCONST(దత్తాంశం); \
  } అయితే(0)

#నిర్వచించు CF_దత్తాంశం_RESTORE(cf, save) \
  చేయి { \
    CF_CTX_CALL_దత్తాంశం(cf) = (save); \
  } అయితే(0)

#అయితే_ముగింపు /* !DEBUGBUILD */

#నిర్వచించు CF_దత్తాంశం_CURRENT(cf) ((cf) ? (CF_CTX_CALL_దత్తాంశం(cf).దత్తాంశం) : ఖాళీ)

#అయితే_ముగింపు /* HEADER_CURL_CFILTERS_H */
