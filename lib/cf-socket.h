#اگر_غیر_تعریف_شدہ سرخی_CURL_CF_SOCKET_H
#تعریف_کرو سرخی_CURL_CF_SOCKET_H
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

ساخت Curl_addrinfo;
ساخت Curl_cfilter;
ساخت Curl_easy;
ساخت connectdata;
ساخت Curl_sockaddr_ex;
ساخت ip_quadruple;

#تعریف_کرو curl_sa_addr    addr.sa
#تعریف_کرو curl_sa_addrbuf addr.buf

/*
 * Parse interface option, and return the interface name and the host part.
 */
CURLcode Curl_parse_interface(ثابت حرف *داخلہ,
                              حرف **dev, حرف **iface, حرف **host);

CURLcode Curl_socket_addr_from_ai(ساخت Curl_sockaddr_ex *addr,
                                  ثابت ساخت Curl_addrinfo *ai,
                                  uint8_t transport);

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * Fill in 'addr' and 'sockfd' accordingly if OK is returned. If the open
 * socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(ساخت Curl_easy *ڈیٹا,
                          ثابت ساخت Curl_addrinfo *ai,
                          ساخت Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd);

#اگر_تعریف_شدہ USE_SO_NOSIGPIPE
/* Set SO_NOSIGPIPE on socket, return < 0 on error. */
صحیح Curl_sock_nosigpipe(curl_socket_t sockfd);
#اگر_ختم

صحیح Curl_socket_close(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                      curl_socket_t sock);

/**
 * Creates a cfilter that opens a TCP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_tcp_create(ساخت Curl_cfilter **pcf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_peer *origin,
                            ساخت Curl_peer *peer,
                            uint8_t transport_peer,
                            ساخت connectdata *conn,
                            ساخت Curl_sockaddr_ex *addr,
                            ساخت Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UDP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_udp_create(ساخت Curl_cfilter **pcf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_peer *origin,
                            ساخت Curl_peer *peer,
                            uint8_t transport_peer,
                            ساخت connectdata *conn,
                            ساخت Curl_sockaddr_ex *addr,
                            ساخت Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UNIX socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_unix_create(ساخت Curl_cfilter **pcf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_peer *origin,
                             ساخت Curl_peer *peer,
                             uint8_t transport_peer,
                             ساخت connectdata *conn,
                             ساخت Curl_sockaddr_ex *addr,
                             ساخت Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

/**
 * Creates a cfilter that keeps a listening socket.
 */
CURLcode Curl_conn_tcp_listen_سیٹ(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  int8_t sockindex,
                                  curl_socket_t *s);

/**
 * Return TRUE iff the last filter at `sockindex` was set via
 * Curl_conn_tcp_listen_set().
 */
bool Curl_conn_is_tcp_listen(ساخت Curl_easy *ڈیٹا,
                             int8_t sockindex);

/**
 * Peek at the socket and remote ip/port the socket filter is using.
 * The filter owns all returned values.
 * @param psock             pointer to hold socket descriptor or NULL
 * @param paddr             pointer to hold addr reference or NULL
 * @param pip               pointer to get IP quadruple or NULL
 * Returns error if the filter is of invalid type.
 */
CURLcode Curl_cf_socket_peek(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             curl_socket_t *psock,
                             ثابت ساخت Curl_sockaddr_ex **paddr,
                             ساخت ip_quadruple *pip) WARN_غیر_مستعمل_نتیجہ;

خارجی ساخت Curl_cftype Curl_cft_tcp;
خارجی ساخت Curl_cftype Curl_cft_udp;
خارجی ساخت Curl_cftype Curl_cft_unix;
خارجی ساخت Curl_cftype Curl_cft_tcp_accept;

#اگر_ختم /* HEADER_CURL_CF_SOCKET_H */
