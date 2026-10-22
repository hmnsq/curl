#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CONNECT_H
#నిర్వచించు శీర్షిక_CURL_CONNECT_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "curlx/timeval.h"

నిర్మాణం Curl_peer;
నిర్మాణం Curl_str;

గణన alpnid Curl_alpn2alpnid(స్థిరరాశి గుర్తులేని అక్షరం *name, size_t len);

/* generic function that returns how much time there is left to run, according
   to the timeouts set */
timediff_t Curl_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం);
timediff_t Curl_timeleft_now_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                స్థిరరాశి నిర్మాణం curltime *pnow);

#నిర్వచించు DEFAULT_CONNECT_TIMEOUT 300000 /* milliseconds == five minutes */

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata **connp);

/*
 * Curl_conncontrol() manipulates the `conn->bits.close` bit on
 * a connection:
 * - CONNCTRL_CONN_KEEP: clear the bit
 * - CONNCTRL_CONN_CLOSE: set the bit
 * - CONNCTRL_STREAM_CLOSE: set the bit when the connection is not
 *                          multiplexed
 * The call does *NOT* cause any immediate connection close.
 */
#నిర్వచించు CONNCTRL_CONN_KEEP       0
#నిర్వచించు CONNCTRL_CONN_CLOSE      1
#నిర్వచించు CONNCTRL_ప్రవాహం_CLOSE    2

శూన్య Curl_conncontrol(నిర్మాణం connectdata *conn, పూర్ణాంకం ctrl);

#నిర్వచించు streamclose(x) Curl_conncontrol((x), CONNCTRL_ప్రవాహం_CLOSE)
#నిర్వచించు connclose(x)   Curl_conncontrol((x), CONNCTRL_CONN_CLOSE)
#నిర్వచించు connkeep(x)    Curl_conncontrol((x), CONNCTRL_CONN_KEEP)

/**
 * Setup the cfilters at `sockindex` in connection `conn`.
 * If no filter chain is installed yet, inspects the configuration
 * in `data` and `conn` to install a suitable filter chain.
 */
CURLcode Curl_conn_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం connectdata *conn,
                         int8_t sockindex,
                         పూర్ణాంకం ssl_బహుళకం);

/**
 * Bring the filter chain at `sockindex` for connection `data->conn` into
 * connected state. Which will set `*done` to TRUE.
 * This can be called on an already connected chain with no side effects.
 * When not `blocking`, calls may return without error and `*done != TRUE`,
 * while the individual filters negotiated the connection.
 */
CURLcode Curl_conn_connect(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                           bool blocking, bool *పూర్తి);

/**
 * The filter chain at `sockindex` failed to connect, e.g. the TLS handshake
 * timed out. Let the filters that did connect, like the TCP socket, report
 * their IP information and stats to the transfer.
 */
శూన్య Curl_conn_report_failed(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex);

/* Set conn to allow multiplexing. */
శూన్య Curl_conn_సమితి_multiplex(నిర్మాణం connectdata *conn);

/* Get the origin peer at sockindex. */
నిర్మాణం Curl_peer *Curl_conn_get_origin(నిర్మాణం connectdata *conn,
                                       int8_t sockindex);

/* Get the peer the connection actually connects to at sockindex.
 * Often the same as "origin", but can be redirected via "connect-to"
 * or "alt-svc". May tunnel through proxies. */
నిర్మాణం Curl_peer *Curl_conn_get_destination(నిర్మాణం connectdata *conn,
                                            int8_t sockindex);

/* Get the peer curl connects its socket to.
 * Can be origin, "connect-to" or the first proxy. */
నిర్మాణం Curl_peer *Curl_conn_get_మొదటి_peer(నిర్మాణం connectdata *conn,
                                           int8_t sockindex);

#అయితే_ముగింపు /* HEADER_CURL_CONNECT_H */
