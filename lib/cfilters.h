#اگر_غیر_تعریف_شدہ سرخی_CURL_CFILTERS_H
#تعریف_کرو سرخی_CURL_CFILTERS_H
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
#شامل "curlx/timediff.h"

ساخت bufq;
ساخت Curl_cfilter;
ساخت Curl_easy;
ساخت Curl_dns_اندراج;
ساخت connectdata;
ساخت ip_quadruple;
ساخت curl_tlssessioninfo;

/* Callback to destroy resources held by this filter instance.
 * Implementations MUST NOT chain calls to cf->next.
 */
قسمتعریف خالی     Curl_cft_destroy_this(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا);

/* Callback to close the connection filter gracefully, non-blocking.
 * Implementations MUST NOT chain calls to cf->next.
 */
قسمتعریف CURLcode Curl_cft_shutdown(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   bool *مکمل);

قسمتعریف CURLcode Curl_cft_connect(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  bool *مکمل);

ساخت easy_pollset;

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
قسمتعریف CURLcode Curl_cft_adjust_pollset(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت easy_pollset *ps);

قسمتعریف bool     Curl_cft_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                       ثابت ساخت Curl_easy *ڈیٹا);

قسمتعریف CURLcode Curl_cft_send(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا, /* transfer */
                               ثابت uint8_t *buf,     /* data to write */
                               size_t len,             /* amount to write */
                               bool eos,               /* last chunk */
                               size_t *pnwritten);     /* how much sent */

قسمتعریف CURLcode Curl_cft_recv(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا, /* transfer */
                               حرف *buf,              /* store data here */
                               size_t len,             /* amount to read */
                               size_t *pnread);        /* how much received */

قسمتعریف bool     Curl_cft_conn_is_alive(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        bool *داخلہ_pending);

قسمتعریف CURLcode Curl_cft_conn_keep_alive(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا);

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
#تعریف_کرو CF_CTRL_ڈیٹا_تیاری              4  /* 0          NULL     first fail */
/* unused now                           5 */
#تعریف_کرو CF_CTRL_ڈیٹا_PAUSE              6  /* on/off     NULL     first fail */
#تعریف_کرو CF_CTRL_ڈیٹا_مکمل               7  /* premature  NULL     ignored */
#تعریف_کرو CF_CTRL_ڈیٹا_مکمل_SEND          8  /* 0          NULL     ignored */
/* update conn info at connection and data */
#تعریف_کرو CF_CTRL_CONN_INFO_UPDATE (256 + 0) /* 0          NULL     ignored */
#تعریف_کرو CF_CTRL_FORGET_SOCKET    (256 + 1) /* 0          NULL     ignored */
#تعریف_کرو CF_CTRL_FLUSH            (256 + 2) /* 0          NULL     first fail */
#تعریف_کرو CF_CTRL_REPORT_STATS     (256 + 3) /* 0          NULL     ignored */

/**
 * Handle event/control for the filter.
 * Implementations MUST NOT chain calls to cf->next.
 */
قسمتعریف CURLcode Curl_cft_cntrl(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح event, صحیح arg1, خالی *arg2);

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
#تعریف_کرو CF_QUERY_MAX_CONCURRENT     1  /* number     -        */
#تعریف_کرو CF_QUERY_CONNECT_جواب_MS   2  /* number     -        */
#تعریف_کرو CF_QUERY_SOCKET             3  /* -          curl_socket_t */
/* unused 4 + 5 */
#تعریف_کرو CF_QUERY_دھارا_ERROR       6  /* error code - */
#تعریف_کرو CF_QUERY_NEED_FLUSH         7  /* TRUE/FALSE - */
#تعریف_کرو CF_QUERY_IP_INFO            8  /* TRUE/FALSE struct ip_quadruple */
#تعریف_کرو CF_QUERY_HTTP_نسخہ       9  /* number (10/11/20/30)   - */
/* pass in a `const struct Curl_sockaddr_ex **` as `pres2`. Gets set
 * to NULL when not connected. */
#تعریف_کرو CF_QUERY_REMOTE_ADDR       10  /* -          `Curl_sockaddr_ex *` */
#تعریف_کرو CF_QUERY_HOST_پورٹ         11  /* port       const char * */
#تعریف_کرو CF_QUERY_SSL_INFO          12  /* -    struct curl_tlssessioninfo * */
#تعریف_کرو CF_QUERY_SSL_CTX_INFO      13  /* -    struct curl_tlssessioninfo * */
#تعریف_کرو CF_QUERY_TRANSPORT         14  /* TRNSPRT_*  - * */
#تعریف_کرو CF_QUERY_ALPN_NEGOTIATED   15  /* -          const char * */
#تعریف_کرو CF_QUERY_REALLY_CONNECTED  16  /* TRUE/FALSE - */

/**
 * Query the cfilter for properties. Filters ignorant of a query will
 * pass it "down" the filter chain.
 */
قسمتعریف CURLcode Curl_cft_query(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح query, صحیح *pres1, خالی *pres2);

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
#تعریف_کرو CF_TYPE_IP_CONNECT  (1 << 0)
#تعریف_کرو CF_TYPE_SSL         (1 << 1)
#تعریف_کرو CF_TYPE_MULTIPLEX   (1 << 2)
#تعریف_کرو CF_TYPE_PROXY       (1 << 3)
#تعریف_کرو CF_TYPE_HTTP        (1 << 4)
#تعریف_کرو CF_TYPE_تیاری       (1 << 5)
#تعریف_کرو CF_TYPE_DNS         (1 << 6)

/* A connection filter type, e.g. specific implementation. */
ساخت Curl_cftype {
  ثابت حرف *name;                        /* name of the filter type */
  صحیح flags;                               /* flags of filter type */
  صحیح log_level;                           /* log level for such filters */
  Curl_cft_destroy_this *destroy;          /* destroy resources of this cf */
  Curl_cft_connect *do_connect;            /* establish connection */
  Curl_cft_shutdown *do_shutdown;          /* shutdown conn */
  Curl_cft_adjust_pollset *adjust_pollset; /* adjust transfer poll set */
  Curl_cft_ڈیٹا_pending *has_ڈیٹا_pending; /* conn has data pending */
  Curl_cft_send *do_send;                  /* send data */
  Curl_cft_recv *do_recv;                  /* receive data */
  Curl_cft_cntrl *cntrl;                   /* events/control */
  Curl_cft_conn_is_alive *is_alive;        /* FALSE if conn is dead, Jim! */
  Curl_cft_conn_keep_alive *keep_alive;    /* try to keep it alive */
  Curl_cft_query *query;                   /* query filter chain */
};

/* A connection filter instance, e.g. registered at a connection */
ساخت Curl_cfilter {
  ثابت ساخت Curl_cftype *cft; /* the type providing implementation */
  ساخت Curl_cfilter *اگلا;     /* next filter in chain */
  خالی *ctx;                     /* filter type specific settings */
  ساخت connectdata *conn;      /* the connection this filter belongs to */
  int8_t sockindex;                 /* the index the filter is installed at */
  ثنائی_ہندسہ(connected);                /* != 0 iff this filter is connected */
  ثنائی_ہندسہ(shutdown);                 /* != 0 iff this filter has shut down */
};

/* Default implementations for the type functions, implementing nop. */
خالی Curl_cf_def_destroy_this(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا);

/* Default implementations for the type functions, implementing pass-through
 * the filter chain. */
CURLcode Curl_cf_def_adjust_pollset(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت easy_pollset *ps);
bool     Curl_cf_def_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                  ثابت ساخت Curl_easy *ڈیٹا);
CURLcode Curl_cf_def_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          ثابت uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten);
CURLcode Curl_cf_def_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          حرف *buf, size_t len, size_t *pnread);
CURLcode Curl_cf_def_cntrl(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           صحیح event, صحیح arg1, خالی *arg2);
bool     Curl_cf_def_conn_is_alive(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   bool *داخلہ_pending);
CURLcode Curl_cf_def_conn_keep_alive(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا);
CURLcode Curl_cf_def_query(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           صحیح query, صحیح *pres1, خالی *pres2);
CURLcode Curl_cf_def_shutdown(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا, bool *مکمل);
CURLcode Curl_cf_def_connect(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا, bool *مکمل);

/**
 * Create a new filter instance, unattached to the filter chain.
 * Use Curl_conn_cf_add() to add it to the chain.
 * @param pcf  on success holds the created instance
 * @param cft   the filter type
 * @param ctx  the type specific context to use
 */
CURLcode Curl_cf_create(ساخت Curl_cfilter **pcf,
                        ثابت ساخت Curl_cftype *cft,
                        خالی *ctx);

/**
 * Add a filter instance to the `sockindex` filter chain at connection
 * `conn`. The filter must not already be attached. It is inserted at
 * the start of the chain (top).
 */
خالی Curl_conn_cf_جوڑو(ساخت Curl_easy *ڈیٹا,
                      ساخت connectdata *conn,
                      int8_t sockindex,
                      ساخت Curl_cfilter *cf);

/**
 * Insert a filter (chain) after `cf_at`.
 * `cf_new` must not already be attached.
 */
خالی Curl_conn_cf_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                               ساخت Curl_cfilter *cf_new);

/**
 * Extract filter `*pcf` from its connection filter chain.
 * Destroy `*pcf`, even if it was not part of the chain and NULL it.
 * Returns TRUE of cf has been part of chain.
 */
bool Curl_conn_cf_discard(ساخت Curl_cfilter **pcf,
                          ساخت Curl_easy *ڈیٹا);

/**
 * Discard all cfilters starting with `*pcf` and clearing it afterwards.
 */
خالی Curl_conn_cf_discard_chain(ساخت Curl_cfilter **pcf,
                                ساخت Curl_easy *ڈیٹا);

/**
 * Remove and destroy all filters at chain `sockindex` on connection `conn`.
 */
خالی Curl_conn_cf_discard_all(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn, int8_t sockindex);

CURLcode Curl_conn_cf_connect(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool *مکمل);
CURLcode Curl_conn_cf_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten);
CURLcode Curl_conn_cf_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                           حرف *buf, size_t len, size_t *pnread);
CURLcode Curl_conn_cf_cntrl(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            bool ignore_نتیجہ,
                            صحیح event, صحیح arg1, خالی *arg2);

/**
 * Get the socket used by the filter chain starting at `cf`.
 * Returns CURL_SOCKET_BAD if not available.
 */
curl_socket_t Curl_conn_cf_get_socket(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا);

CURLcode Curl_conn_cf_get_ip_info(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  bool *is_ipv6, ساخت ip_quadruple *ipquad);

bool Curl_conn_cf_needs_flush(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا);

bool Curl_conn_cf_is_ip_connected(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا);

غیرعلامتی حرف Curl_conn_cf_get_transport(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا);

صحیح Curl_socktype_for_transport(uint8_t transport);
صحیح Curl_protocol_for_transport(uint8_t transport);

ثابت حرف *Curl_conn_cf_get_alpn_negotiated(ساخت Curl_cfilter *cf,
                                             ساخت Curl_easy *ڈیٹا);

#تعریف_کرو CURL_CF_SSL_DEFAULT  (-1)
#تعریف_کرو CURL_CF_SSL_DISABLE  0
#تعریف_کرو CURL_CF_SSL_ENABLE   1

/**
 * Check if a filter chain at `sockindex` for connection `conn` exists.
 */
bool Curl_conn_is_تیاری(ساخت connectdata *conn, int8_t sockindex);

/**
 * Check if the filter chain at `sockindex` for connection `conn` is
 * completely connected.
 */
bool Curl_conn_is_connected(ساخت connectdata *conn, int8_t sockindex);

/**
 * Determine if we have reached the remote host on IP level, e.g.
 * have a TCP connection. This turns TRUE before a possible SSL
 * handshake has been started/done.
 */
bool Curl_conn_is_ip_connected(ساخت Curl_easy *ڈیٹا, int8_t sockindex);

/**
 * Determine if the connection is using SSL to the remote host
 * (or will be once connected). This will return FALSE, if SSL
 * is only used in proxying and not for the tunnel itself.
 */
bool Curl_conn_is_ssl(ساخت connectdata *conn, int8_t sockindex);

/* Determine if the connection has one or more proxy filters.
 * e.g. is tunneling. */
bool Curl_conn_is_tunneling(ساخت connectdata *conn, int8_t sockindex);

/*
 * Fill `info` with information about the TLS instance securing the connection
 * when available, otherwise e.g. when Curl_conn_is_ssl() is FALSE, return
 * FALSE. 'query' should be CF_QUERY_SSL_INFO or CF_QUERY_SSL_CTX_INFO.
 */
bool Curl_conn_get_ssl_info(ساخت Curl_easy *ڈیٹا,
                            ساخت connectdata *conn, int8_t sockindex,
                            صحیح query,
                            ساخت curl_tlssessioninfo *info);

CURLcode Curl_conn_get_ip_info(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ساخت ip_quadruple *ipquad);

/**
 * Connection provides multiplexing of easy handles at `socketindex`.
 */
bool Curl_conn_is_multiplex(ساخت connectdata *conn, int8_t sockindex);

/**
 * Return the HTTP version used on the FIRSTSOCKET connection filters
 * or 0 if unknown. Value otherwise is 09, 10, 11, etc.
 */
غیرعلامتی حرف Curl_conn_http_نسخہ(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn);

/* Get the TRNSPRT_* the connection is using */
غیرعلامتی حرف Curl_conn_get_transport(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn);

/* Get the negotiated ALPN protocol or NULL if none in play */
ثابت حرف *Curl_conn_get_alpn_negotiated(ساخت Curl_easy *ڈیٹا,
                                          ساخت connectdata *conn);

خالی Curl_conn_cntrl_update_info(ساخت Curl_easy *ڈیٹا,
                                 ساخت connectdata *conn);

خالی Curl_conn_cntrl_report_stats(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  صحیح sockindex);

خالی Curl_conn_remove_تیاری_filters(ساخت Curl_easy *ڈیٹا,
                                    int8_t sockindex);

/**
 * Return if data is pending in some connection filter at chain
 * `sockindex` for connection `data->conn`.
 */
bool Curl_conn_ڈیٹا_pending(ساخت Curl_easy *ڈیٹا,
                            int8_t sockindex);

/**
 * Return TRUE if any of the connection filters at chain `sockindex`
 * have data still to send.
 */
bool Curl_conn_needs_flush(ساخت Curl_easy *ڈیٹا, int8_t sockindex);

/**
 * Flush any pending data on the connection filters at chain `sockindex`.
 */
CURLcode Curl_conn_flush(ساخت Curl_easy *ڈیٹا, int8_t sockindex);

/**
 * Return the socket used on data's connection for FIRSTSOCKET,
 * querying filters if the whole chain has not connected yet.
 * Returns CURL_SOCKET_BAD if not available.
 */
curl_socket_t Curl_conn_get_پہلا_socket(ساخت Curl_easy *ڈیٹا);

/* Return a pointer to the connected socket address or NULL. */
ثابت ساخت Curl_sockaddr_ex *
Curl_conn_get_remote_addr(ساخت Curl_easy *ڈیٹا, int8_t sockindex);

/**
 * Tell filters to forget about the socket at sockindex.
 */
خالی Curl_conn_forget_socket(ساخت Curl_easy *ڈیٹا, int8_t sockindex);

/**
 * Adjust the pollset for the filter chain starting at `cf`.
 */
CURLcode Curl_conn_cf_adjust_pollset(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     ساخت easy_pollset *ps);

/**
 * Adjust pollset from filters installed at transfer's connection.
 */
CURLcode Curl_conn_adjust_pollset(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  ساخت easy_pollset *ps);

/**
 * Curl_poll() the filter chain at `cf` with timeout `timeout_ms`.
 * Returns 0 on timeout, negative on error or number of sockets
 * with requested poll events.
 */
صحیح Curl_conn_cf_poll(ساخت Curl_cfilter *cf,
                      ساخت Curl_easy *ڈیٹا,
                      timediff_t timeout_ms);

/**
 * Receive data through the filter chain at `sockindex` for connection
 * `data->conn`. Copy at most `len` bytes into `buf`. Return the
 * actual number of bytes copied in `*pnread`or an error.
 */
CURLcode Curl_cf_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex, حرف *buf,
                      size_t len, size_t *pnread);

/**
 * Send `len` bytes of data from `buf` through the filter chain `sockindex`
 * at connection `data->conn`. Return the actual number of bytes written
 * in `*pnwritten` or on error.
 */
CURLcode Curl_cf_send(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                      ثابت uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten);

/**
 * Receive bytes from connection filter `cf` into `bufq`.
 * Convenience wrapper around `Curl_bufq_sipn()`,
 * so users do not have to implement a callback.
 */
CURLcode Curl_cf_recv_bufq(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت bufq *bufq,
                           size_t maxlen,
                           size_t *pnread);

/**
 * Send bytes in `bufq` using connection filter `cf`.
 * A convenience wrapper around `Curl_bufq_write_pass()`,
 * so users do not have to implement a callback.
 */
CURLcode Curl_cf_send_bufq(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت bufq *bufq,
                           ثابت غیرعلامتی حرف *buf, size_t blen,
                           size_t *pnwritten);

/**
 * Notify connection filters that they need to setup data for
 * a transfer.
 */
CURLcode Curl_conn_ev_ڈیٹا_تیاری(ساخت Curl_easy *ڈیٹا);

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
خالی Curl_conn_ev_ڈیٹا_مکمل_send(ساخت Curl_easy *ڈیٹا);

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
خالی Curl_conn_ev_ڈیٹا_مکمل(ساخت Curl_easy *ڈیٹا, bool premature);

/**
 * Notify connection filters that the transfer of data is paused/unpaused.
 */
CURLcode Curl_conn_ev_ڈیٹا_pause(ساخت Curl_easy *ڈیٹا, bool do_pause);

/**
 * Check if FIRSTSOCKET's cfilter chain deems connection alive.
 */
bool Curl_conn_is_alive(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                        bool *داخلہ_pending);

/**
 * Try to upkeep the connection filters.
 */
CURLcode Curl_conn_keep_alive(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn);

/**
 * Get the remote hostname and port that the connection is currently
 * talking to (or will talk to).
 * Once connected or before connect starts,
 * it is `conn->origin->hostname` and `conn->origin->port`.
 * During connect, when tunneling proxies are involved (http or socks),
 * it will be the name and port the proxy currently negotiates with.
 */
خالی Curl_conn_get_current_host(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                                ثابت حرف **phost, صحیح *pport);

/**
 * Get the maximum number of parallel transfers the connection
 * expects to be able to handle at `sockindex`.
 */
size_t Curl_conn_get_max_concurrent(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn,
                                    int8_t sockindex);

/**
 * Get the underlying error code for a transfer stream or 0 if not known.
 */
صحیح Curl_conn_get_دھارا_error(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               int8_t sockindex);

#اگر_تعریف_شدہ CURLVERBOSE
خالی Curl_conn_trc_filters(ساخت Curl_easy *ڈیٹا,
                           int8_t sockindex, ثابت حرف *info);
#اگر_ختم

/**
 * Get the index of the given socket in the connection's sockets.
 * Useful in calling `Curl_conn_send()/Curl_conn_recv()` with the
 * correct socket index.
 */
int8_t Curl_conn_sockindex(ساخت Curl_easy *ڈیٹا, curl_socket_t sockfd);

/*
 * Receive data on the connection, using FIRSTSOCKET/SECONDARYSOCKET.
 * Return CURLE_AGAIN iff blocked on receiving.
 */
CURLcode Curl_conn_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                        حرف *buf, size_t len, size_t *pnread);

/*
 * Send data on the connection, using FIRSTSOCKET/SECONDARYSOCKET.
 * Return CURLE_AGAIN iff blocked on sending.
 */
CURLcode Curl_conn_send(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                        ثابت خالی *buf, size_t len, bool eos,
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
ساخت cf_call_ڈیٹا {
  ساخت Curl_easy *ڈیٹا;
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح گہرائی;
#اگر_ختم
};

/**
 * define to access the `struct cf_call_data for a cfilter. Normally
 * a member in the cfilter's `ctx`.
 *
 * #define CF_CTX_CALL_DATA(cf)   -> struct cf_call_data instance
 */

#اگر_تعریف_شدہ DEBUGBUILD

#تعریف_کرو CF_ڈیٹا_SAVE(save, cf, ڈیٹا) \
  کرو { \
    (save) = CF_CTX_CALL_ڈیٹا(cf); \
    DEBUGASSERT(!(save).ڈیٹا || (save).گہرائی > 0); \
    CF_CTX_CALL_ڈیٹا(cf).گہرائی++;  \
    CF_CTX_CALL_ڈیٹا(cf).ڈیٹا = (ساخت Curl_easy *)CURL_UNCONST(ڈیٹا); \
  } جب(0)

#تعریف_کرو CF_ڈیٹا_RESTORE(cf, save) \
  کرو { \
    DEBUGASSERT(CF_CTX_CALL_ڈیٹا(cf).گہرائی == (save).گہرائی + 1); \
    DEBUGASSERT(!(save).ڈیٹا || (save).گہرائی > 0); \
    CF_CTX_CALL_ڈیٹا(cf) = (save); \
  } جب(0)

#ورنہ /* DEBUGBUILD */

#تعریف_کرو CF_ڈیٹا_SAVE(save, cf, ڈیٹا) \
  کرو { \
    (save) = CF_CTX_CALL_ڈیٹا(cf); \
    CF_CTX_CALL_ڈیٹا(cf).ڈیٹا = (ساخت Curl_easy *)CURL_UNCONST(ڈیٹا); \
  } جب(0)

#تعریف_کرو CF_ڈیٹا_RESTORE(cf, save) \
  کرو { \
    CF_CTX_CALL_ڈیٹا(cf) = (save); \
  } جب(0)

#اگر_ختم /* !DEBUGBUILD */

#تعریف_کرو CF_ڈیٹا_CURRENT(cf) ((cf) ? (CF_CTX_CALL_ڈیٹا(cf).ڈیٹا) : تہی)

#اگر_ختم /* HEADER_CURL_CFILTERS_H */
