#यदि_अपरिभाषितम् शीर्षकम्_CURL_CONNECT_H
#परिभाषय शीर्षकम्_CURL_CONNECT_H
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

#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "curlx/timeval.h"

संरचना Curl_peer;
संरचना Curl_str;

गणना alpnid Curl_alpn2alpnid(ध्रुव अचिह्नित वर्ण *name, size_t len);

/* generic function that returns how much time there is left to run, according
   to the timeouts set */
timediff_t Curl_timeleft_ms(संरचना Curl_easy *दत्त);
timediff_t Curl_timeleft_now_ms(संरचना Curl_easy *दत्त,
                                ध्रुव संरचना curltime *pnow);

#परिभाषय DEFAULT_CONNECT_TIMEOUT 300000 /* milliseconds == five minutes */

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(संरचना Curl_easy *दत्त,
                                  संरचना connectdata **connp);

/*
 * Curl_conncontrol() manipulates the `conn->bits.close` bit on
 * a connection:
 * - CONNCTRL_CONN_KEEP: clear the bit
 * - CONNCTRL_CONN_CLOSE: set the bit
 * - CONNCTRL_STREAM_CLOSE: set the bit when the connection is not
 *                          multiplexed
 * The call does *NOT* cause any immediate connection close.
 */
#परिभाषय CONNCTRL_CONN_KEEP       0
#परिभाषय CONNCTRL_CONN_CLOSE      1
#परिभाषय CONNCTRL_प्रवाहः_CLOSE    2

शून्य Curl_conncontrol(संरचना connectdata *conn, पूर्णाङ्क ctrl);

#परिभाषय streamclose(x) Curl_conncontrol((x), CONNCTRL_प्रवाहः_CLOSE)
#परिभाषय connclose(x)   Curl_conncontrol((x), CONNCTRL_CONN_CLOSE)
#परिभाषय connkeep(x)    Curl_conncontrol((x), CONNCTRL_CONN_KEEP)

/**
 * Setup the cfilters at `sockindex` in connection `conn`.
 * If no filter chain is installed yet, inspects the configuration
 * in `data` and `conn` to install a suitable filter chain.
 */
CURLcode Curl_conn_सज्जता(संरचना Curl_easy *दत्त,
                         संरचना connectdata *conn,
                         int8_t sockindex,
                         पूर्णाङ्क ssl_बहुलकम्);

/**
 * Bring the filter chain at `sockindex` for connection `data->conn` into
 * connected state. Which will set `*done` to TRUE.
 * This can be called on an already connected chain with no side effects.
 * When not `blocking`, calls may return without error and `*done != TRUE`,
 * while the individual filters negotiated the connection.
 */
CURLcode Curl_conn_connect(संरचना Curl_easy *दत्त, int8_t sockindex,
                           bool blocking, bool *समाप्तम्);

/**
 * The filter chain at `sockindex` failed to connect, e.g. the TLS handshake
 * timed out. Let the filters that did connect, like the TCP socket, report
 * their IP information and stats to the transfer.
 */
शून्य Curl_conn_report_failed(संरचना Curl_easy *दत्त, int8_t sockindex);

/* Set conn to allow multiplexing. */
शून्य Curl_conn_समुच्चयः_multiplex(संरचना connectdata *conn);

/* Get the origin peer at sockindex. */
संरचना Curl_peer *Curl_conn_get_origin(संरचना connectdata *conn,
                                       int8_t sockindex);

/* Get the peer the connection actually connects to at sockindex.
 * Often the same as "origin", but can be redirected via "connect-to"
 * or "alt-svc". May tunnel through proxies. */
संरचना Curl_peer *Curl_conn_get_destination(संरचना connectdata *conn,
                                            int8_t sockindex);

/* Get the peer curl connects its socket to.
 * Can be origin, "connect-to" or the first proxy. */
संरचना Curl_peer *Curl_conn_get_first_peer(संरचना connectdata *conn,
                                           int8_t sockindex);

#यदि_अन्तः /* HEADER_CURL_CONNECT_H */
