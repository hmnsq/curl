#اگر_غیر_تعریف_شدہ سرخی_CURL_CONNECT_H
#تعریف_کرو سرخی_CURL_CONNECT_H
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
#شامل "curl_setup.h"

#شامل "vdns/hostip.h"
#شامل "curlx/timeval.h"

ساخت Curl_peer;
ساخت Curl_str;

شمار alpnid Curl_alpn2alpnid(ثابت غیرعلامتی حرف *name, size_t len);

/* generic function that returns how much time there is left to run, according
   to the timeouts set */
timediff_t Curl_timeleft_ms(ساخت Curl_easy *ڈیٹا);
timediff_t Curl_timeleft_now_ms(ساخت Curl_easy *ڈیٹا,
                                ثابت ساخت curltime *pnow);

#تعریف_کرو DEFAULT_CONNECT_TIMEOUT 300000 /* milliseconds == five minutes */

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata **connp);

/*
 * Curl_conncontrol() manipulates the `conn->bits.close` bit on
 * a connection:
 * - CONNCTRL_CONN_KEEP: clear the bit
 * - CONNCTRL_CONN_CLOSE: set the bit
 * - CONNCTRL_STREAM_CLOSE: set the bit when the connection is not
 *                          multiplexed
 * The call does *NOT* cause any immediate connection close.
 */
#تعریف_کرو CONNCTRL_CONN_KEEP       0
#تعریف_کرو CONNCTRL_CONN_CLOSE      1
#تعریف_کرو CONNCTRL_دھارا_CLOSE    2

خالی Curl_conncontrol(ساخت connectdata *conn, صحیح ctrl);

#تعریف_کرو streamclose(x) Curl_conncontrol((x), CONNCTRL_دھارا_CLOSE)
#تعریف_کرو connclose(x)   Curl_conncontrol((x), CONNCTRL_CONN_CLOSE)
#تعریف_کرو connkeep(x)    Curl_conncontrol((x), CONNCTRL_CONN_KEEP)

/**
 * Setup the cfilters at `sockindex` in connection `conn`.
 * If no filter chain is installed yet, inspects the configuration
 * in `data` and `conn` to install a suitable filter chain.
 */
CURLcode Curl_conn_تیاری(ساخت Curl_easy *ڈیٹا,
                         ساخت connectdata *conn,
                         int8_t sockindex,
                         صحیح ssl_منوال);

/**
 * Bring the filter chain at `sockindex` for connection `data->conn` into
 * connected state. Which will set `*done` to TRUE.
 * This can be called on an already connected chain with no side effects.
 * When not `blocking`, calls may return without error and `*done != TRUE`,
 * while the individual filters negotiated the connection.
 */
CURLcode Curl_conn_connect(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                           bool blocking, bool *مکمل);

/**
 * The filter chain at `sockindex` failed to connect, e.g. the TLS handshake
 * timed out. Let the filters that did connect, like the TCP socket, report
 * their IP information and stats to the transfer.
 */
خالی Curl_conn_report_failed(ساخت Curl_easy *ڈیٹا, int8_t sockindex);

/* Set conn to allow multiplexing. */
خالی Curl_conn_سیٹ_multiplex(ساخت connectdata *conn);

/* Get the origin peer at sockindex. */
ساخت Curl_peer *Curl_conn_get_origin(ساخت connectdata *conn,
                                       int8_t sockindex);

/* Get the peer the connection actually connects to at sockindex.
 * Often the same as "origin", but can be redirected via "connect-to"
 * or "alt-svc". May tunnel through proxies. */
ساخت Curl_peer *Curl_conn_get_destination(ساخت connectdata *conn,
                                            int8_t sockindex);

/* Get the peer curl connects its socket to.
 * Can be origin, "connect-to" or the first proxy. */
ساخت Curl_peer *Curl_conn_get_پہلا_peer(ساخت connectdata *conn,
                                           int8_t sockindex);

#اگر_ختم /* HEADER_CURL_CONNECT_H */
