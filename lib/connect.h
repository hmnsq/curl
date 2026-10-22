#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CONNECT_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CONNECT_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

ರಚನೆ Curl_peer;
ರಚನೆ Curl_str;

ಎಣಿಕೆ alpnid Curl_alpn2alpnid(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *name, size_t len);

/* generic function that returns how much time there is left to run, according
   to the timeouts set */
timediff_t Curl_timeleft_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
timediff_t Curl_timeleft_now_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_CONNECT_TIMEOUT 300000 /* milliseconds == five minutes */

/*
 * Used to extract socket and connectdata struct for the most recent
 * transfer on the given Curl_easy.
 *
 * The returned socket will be CURL_SOCKET_BAD in case of failure!
 */
curl_socket_t Curl_getconnectinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata **connp);

/*
 * Curl_conncontrol() manipulates the `conn->bits.close` bit on
 * a connection:
 * - CONNCTRL_CONN_KEEP: clear the bit
 * - CONNCTRL_CONN_CLOSE: set the bit
 * - CONNCTRL_STREAM_CLOSE: set the bit when the connection is not
 *                          multiplexed
 * The call does *NOT* cause any immediate connection close.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CONNCTRL_CONN_KEEP       0
#ವ್ಯಾಖ್ಯಾನಿಸು CONNCTRL_CONN_CLOSE      1
#ವ್ಯಾಖ್ಯಾನಿಸು CONNCTRL_ಹರಿವು_CLOSE    2

ಶೂನ್ಯ Curl_conncontrol(ರಚನೆ connectdata *conn, ಪೂರ್ಣಾಂಕ ctrl);

#ವ್ಯಾಖ್ಯಾನಿಸು streamclose(x) Curl_conncontrol((x), CONNCTRL_ಹರಿವು_CLOSE)
#ವ್ಯಾಖ್ಯಾನಿಸು connclose(x)   Curl_conncontrol((x), CONNCTRL_CONN_CLOSE)
#ವ್ಯಾಖ್ಯಾನಿಸು connkeep(x)    Curl_conncontrol((x), CONNCTRL_CONN_KEEP)

/**
 * Setup the cfilters at `sockindex` in connection `conn`.
 * If no filter chain is installed yet, inspects the configuration
 * in `data` and `conn` to install a suitable filter chain.
 */
CURLcode Curl_conn_ಸಿದ್ಧತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ connectdata *conn,
                         int8_t sockindex,
                         ಪೂರ್ಣಾಂಕ ssl_ಬಹುಲಕ);

/**
 * Bring the filter chain at `sockindex` for connection `data->conn` into
 * connected state. Which will set `*done` to TRUE.
 * This can be called on an already connected chain with no side effects.
 * When not `blocking`, calls may return without error and `*done != TRUE`,
 * while the individual filters negotiated the connection.
 */
CURLcode Curl_conn_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                           bool blocking, bool *ಮುಗಿದ);

/**
 * The filter chain at `sockindex` failed to connect, e.g. the TLS handshake
 * timed out. Let the filters that did connect, like the TCP socket, report
 * their IP information and stats to the transfer.
 */
ಶೂನ್ಯ Curl_conn_report_failed(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex);

/* Set conn to allow multiplexing. */
ಶೂನ್ಯ Curl_conn_ಗಣ_multiplex(ರಚನೆ connectdata *conn);

/* Get the origin peer at sockindex. */
ರಚನೆ Curl_peer *Curl_conn_get_origin(ರಚನೆ connectdata *conn,
                                       int8_t sockindex);

/* Get the peer the connection actually connects to at sockindex.
 * Often the same as "origin", but can be redirected via "connect-to"
 * or "alt-svc". May tunnel through proxies. */
ರಚನೆ Curl_peer *Curl_conn_get_destination(ರಚನೆ connectdata *conn,
                                            int8_t sockindex);

/* Get the peer curl connects its socket to.
 * Can be origin, "connect-to" or the first proxy. */
ರಚನೆ Curl_peer *Curl_conn_get_ಮೊದಲ_peer(ರಚನೆ connectdata *conn,
                                           int8_t sockindex);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CONNECT_H */
