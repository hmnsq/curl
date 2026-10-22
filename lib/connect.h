#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CONNECT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CONNECT_H
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

#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

ਬਣਤਰ Curl_peer;
ਬਣਤਰ Curl_str;

ਗਿਣਤੀ alpnid Curl_alpn2alpnid(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *name, size_t len);

/* generic function that returns how much time there is left to run, according
   to the timeouts set */
timediff_t Curl_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
timediff_t Curl_timeleft_now_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅਚਲ ਬਣਤਰ curltime *pnow);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_CONNECT_TIMEOUT 300000 /* milliseconds == five minutes */

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata **connp);

/*
 * Curl_conncontrol() manipulates the `conn->bits.close` bit on
 * a connection:
 * - CONNCTRL_CONN_KEEP: clear the bit
 * - CONNCTRL_CONN_CLOSE: set the bit
 * - CONNCTRL_STREAM_CLOSE: set the bit when the connection is not
 *                          multiplexed
 * The call does *NOT* cause any immediate connection close.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONNCTRL_CONN_KEEP       0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONNCTRL_CONN_CLOSE      1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONNCTRL_ਧਾਰਾ_CLOSE    2

ਖਾਲੀ Curl_conncontrol(ਬਣਤਰ connectdata *conn, ਪੂਰਨ ctrl);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ streamclose(x) Curl_conncontrol((x), CONNCTRL_ਧਾਰਾ_CLOSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ connclose(x)   Curl_conncontrol((x), CONNCTRL_CONN_CLOSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ connkeep(x)    Curl_conncontrol((x), CONNCTRL_CONN_KEEP)

/**
 * Setup the cfilters at `sockindex` in connection `conn`.
 * If no filter chain is installed yet, inspects the configuration
 * in `data` and `conn` to install a suitable filter chain.
 */
CURLcode Curl_conn_ਤਿਆਰੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ connectdata *conn,
                         int8_t sockindex,
                         ਪੂਰਨ ssl_ਬਹੁਲਕ);

/**
 * Bring the filter chain at `sockindex` for connection `data->conn` into
 * connected state. Which will set `*done` to TRUE.
 * This can be called on an already connected chain with no side effects.
 * When not `blocking`, calls may return without error and `*done != TRUE`,
 * while the individual filters negotiated the connection.
 */
CURLcode Curl_conn_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                           bool blocking, bool *ਪੂਰਾ);

/**
 * The filter chain at `sockindex` failed to connect, e.g. the TLS handshake
 * timed out. Let the filters that did connect, like the TCP socket, report
 * their IP information and stats to the transfer.
 */
ਖਾਲੀ Curl_conn_report_failed(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex);

/* Set conn to allow multiplexing. */
ਖਾਲੀ Curl_conn_ਸੈੱਟ_multiplex(ਬਣਤਰ connectdata *conn);

/* Get the origin peer at sockindex. */
ਬਣਤਰ Curl_peer *Curl_conn_get_origin(ਬਣਤਰ connectdata *conn,
                                       int8_t sockindex);

/* Get the peer the connection actually connects to at sockindex.
 * Often the same as "origin", but can be redirected via "connect-to"
 * or "alt-svc". May tunnel through proxies. */
ਬਣਤਰ Curl_peer *Curl_conn_get_destination(ਬਣਤਰ connectdata *conn,
                                            int8_t sockindex);

/* Get the peer curl connects its socket to.
 * Can be origin, "connect-to" or the first proxy. */
ਬਣਤਰ Curl_peer *Curl_conn_get_ਪਹਿਲਾ_peer(ਬਣਤਰ connectdata *conn,
                                           int8_t sockindex);

#ਜੇ_ਅੰਤ /* HEADER_CURL_CONNECT_H */
