#ifndef HEADER_CURL_CONNECT_H
#define HEADER_CURL_CONNECT_H
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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "vdns/hostip.h"
#অন্তর্ভাৱ "curlx/timeval.h"

সংরচনা Curl_peer;
সংরচনা Curl_str;

গণনা alpnid Curl_alpn2alpnid(ধ্রুৱ unsigned ৱর্ণ *name, size_t len);

/* generic function that returns how much time there is left to run, according
   to the timeouts set */
timediff_t Curl_timeleft_ms(সংরচনা Curl_easy *data);
timediff_t Curl_timeleft_now_ms(সংরচনা Curl_easy *data,
                                ধ্রুৱ সংরচনা curltime *pnow);

#define DEFAULT_CONNECT_TIMEOUT 300000 /* milliseconds == five minutes */

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata **connp);

/*
 * Curl_conncontrol() manipulates the `conn->bits.close` bit on
 * a connection:
 * - CONNCTRL_CONN_KEEP: clear the bit
 * - CONNCTRL_CONN_CLOSE: set the bit
 * - CONNCTRL_STREAM_CLOSE: set the bit when the connection is not
 *                          multiplexed
 * The call does *NOT* cause any immediate connection close.
 */
#define CONNCTRL_CONN_KEEP       0
#define CONNCTRL_CONN_CLOSE      1
#define CONNCTRL_STREAM_CLOSE    2

শূন্য Curl_conncontrol(সংরচনা connectdata *conn, পূর্ণাঙ্ক ctrl);

#define streamclose(x) Curl_conncontrol((x), CONNCTRL_STREAM_CLOSE)
#define connclose(x)   Curl_conncontrol((x), CONNCTRL_CONN_CLOSE)
#define connkeep(x)    Curl_conncontrol((x), CONNCTRL_CONN_KEEP)

/**
 * Setup the cfilters at `sockindex` in connection `conn`.
 * If no filter chain is installed yet, inspects the configuration
 * in `data` and `conn` to install a suitable filter chain.
 */
CURLcode Curl_conn_setup(সংরচনা Curl_easy *data,
                         সংরচনা connectdata *conn,
                         int8_t sockindex,
                         পূর্ণাঙ্ক ssl_mode);

/**
 * Bring the filter chain at `sockindex` for connection `data->conn` into
 * connected state. Which will set `*done` to TRUE.
 * This can be called on an already connected chain with no side effects.
 * When not `blocking`, calls may return without error and `*done != TRUE`,
 * while the individual filters negotiated the connection.
 */
CURLcode Curl_conn_connect(সংরচনা Curl_easy *data, int8_t sockindex,
                           bool blocking, bool *done);

/**
 * The filter chain at `sockindex` failed to connect, e.g. the TLS handshake
 * timed out. Let the filters that did connect, like the TCP socket, report
 * their IP information and stats to the transfer.
 */
শূন্য Curl_conn_report_failed(সংরচনা Curl_easy *data, int8_t sockindex);

/* Set conn to allow multiplexing. */
শূন্য Curl_conn_set_multiplex(সংরচনা connectdata *conn);

/* Get the origin peer at sockindex. */
সংরচনা Curl_peer *Curl_conn_get_origin(সংরচনা connectdata *conn,
                                       int8_t sockindex);

/* Get the peer the connection actually connects to at sockindex.
 * Often the same as "origin", but can be redirected via "connect-to"
 * or "alt-svc". May tunnel through proxies. */
সংরচনা Curl_peer *Curl_conn_get_destination(সংরচনা connectdata *conn,
                                            int8_t sockindex);

/* Get the peer curl connects its socket to.
 * Can be origin, "connect-to" or the first proxy. */
সংরচনা Curl_peer *Curl_conn_get_first_peer(সংরচনা connectdata *conn,
                                           int8_t sockindex);

#endif /* HEADER_CURL_CONNECT_H */
