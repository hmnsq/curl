#अगर_अघोषित HEADER_CURL_CONNECT_H
#घोषणा HEADER_CURL_CONNECT_H
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
#समावेश "curl_setup.h"

#समावेश "vdns/hostip.h"
#समावेश "curlx/timeval.h"

काष्ठा Curl_peer;
काष्ठा Curl_str;

क्रमागत alpnid Curl_alpn2alpnid(स्थिर अचिन्हित अक्षर *name, माप_प्रकार len);

/* generic function that returns how much time there is left to run, according
   to the timeouts set */
timediff_t Curl_timeleft_ms(काष्ठा Curl_easy *तथ्य);
timediff_t Curl_timeleft_now_ms(काष्ठा Curl_easy *तथ्य,
                                स्थिर काष्ठा curltime *pnow);

#घोषणा DEFAULT_CONNECT_TIMEOUT 300000 /* milliseconds == five minutes */

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata **connp);

/*
 * Curl_conncontrol() manipulates the `conn->bits.close` bit on
 * a connection:
 * - CONNCTRL_CONN_KEEP: clear the bit
 * - CONNCTRL_CONN_CLOSE: set the bit
 * - CONNCTRL_STREAM_CLOSE: set the bit when the connection is not
 *                          multiplexed
 * The call does *NOT* cause any immediate connection close.
 */
#घोषणा CONNCTRL_CONN_KEEP       0
#घोषणा CONNCTRL_CONN_CLOSE      1
#घोषणा CONNCTRL_STREAM_CLOSE    2

व्योम Curl_conncontrol(काष्ठा connectdata *conn, पूर्णांक ctrl);

#घोषणा streamclose(x) Curl_conncontrol((x), CONNCTRL_STREAM_CLOSE)
#घोषणा connclose(x)   Curl_conncontrol((x), CONNCTRL_CONN_CLOSE)
#घोषणा connkeep(x)    Curl_conncontrol((x), CONNCTRL_CONN_KEEP)

/**
 * Setup the cfilters at `sockindex` in connection `conn`.
 * If no filter chain is installed yet, inspects the configuration
 * in `data` and `conn` to install a suitable filter chain.
 */
CURLcode Curl_conn_setup(काष्ठा Curl_easy *तथ्य,
                         काष्ठा connectdata *conn,
                         int8_t sockindex,
                         पूर्णांक ssl_mode);

/**
 * Bring the filter chain at `sockindex` for connection `data->conn` into
 * connected state. Which will set `*done` to TRUE.
 * This can be called on an already connected chain with no side effects.
 * When not `blocking`, calls may return without error and `*done != TRUE`,
 * while the individual filters negotiated the connection.
 */
CURLcode Curl_conn_connect(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                           bool blocking, bool *done);

/**
 * The filter chain at `sockindex` failed to connect, e.g. the TLS handshake
 * timed out. Let the filters that did connect, like the TCP socket, report
 * their IP information and stats to the transfer.
 */
व्योम Curl_conn_report_failed(काष्ठा Curl_easy *तथ्य, int8_t sockindex);

/* Set conn to allow multiplexing. */
व्योम Curl_conn_set_multiplex(काष्ठा connectdata *conn);

/* Get the origin peer at sockindex. */
काष्ठा Curl_peer *Curl_conn_get_origin(काष्ठा connectdata *conn,
                                       int8_t sockindex);

/* Get the peer the connection actually connects to at sockindex.
 * Often the same as "origin", but can be redirected via "connect-to"
 * or "alt-svc". May tunnel through proxies. */
काष्ठा Curl_peer *Curl_conn_get_destination(काष्ठा connectdata *conn,
                                            int8_t sockindex);

/* Get the peer curl connects its socket to.
 * Can be origin, "connect-to" or the first proxy. */
काष्ठा Curl_peer *Curl_conn_get_first_peer(काष्ठा connectdata *conn,
                                           int8_t sockindex);

#पूर्ण_अगर /* HEADER_CURL_CONNECT_H */
