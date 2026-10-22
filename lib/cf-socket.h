#ifndef HEADER_CURL_CF_SOCKET_H
#define HEADER_CURL_CF_SOCKET_H
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

ساختار Curl_addrinfo;
ساختار Curl_cfilter;
ساختار Curl_easy;
ساختار connectdata;
ساختار Curl_sockaddr_ex;
ساختار ip_quadruple;

#define curl_sa_addr    addr.sa
#define curl_sa_addrbuf addr.buf

/*
 * Parse interface option, and return the interface name and the host part.
 */
CURLcode Curl_parse_interface(ثابت نویسه *درون_داد,
                              نویسه **dev, نویسه **iface, نویسه **host);

CURLcode Curl_socket_addr_from_ai(ساختار Curl_sockaddr_ex *addr,
                                  ثابت ساختار Curl_addrinfo *ai,
                                  uint8_t transport);

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * Fill in 'addr' and 'sockfd' accordingly if OK is returned. If the open
 * socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(ساختار Curl_easy *data,
                          ثابت ساختار Curl_addrinfo *ai,
                          ساختار Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd);

#ifdef USE_SO_NOSIGPIPE
/* Set SO_NOSIGPIPE on socket, return < 0 on error. */
صحیح Curl_sock_nosigpipe(curl_socket_t sockfd);
#endif

صحیح Curl_socket_close(ساختار Curl_easy *data, ساختار connectdata *conn,
                      curl_socket_t sock);

/**
 * Creates a cfilter that opens a TCP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_tcp_create(ساختار Curl_cfilter **pcf,
                            ساختار Curl_easy *data,
                            ساختار Curl_peer *origin,
                            ساختار Curl_peer *peer,
                            uint8_t transport_peer,
                            ساختار connectdata *conn,
                            ساختار Curl_sockaddr_ex *addr,
                            ساختار Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UDP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_udp_create(ساختار Curl_cfilter **pcf,
                            ساختار Curl_easy *data,
                            ساختار Curl_peer *origin,
                            ساختار Curl_peer *peer,
                            uint8_t transport_peer,
                            ساختار connectdata *conn,
                            ساختار Curl_sockaddr_ex *addr,
                            ساختار Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UNIX socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_unix_create(ساختار Curl_cfilter **pcf,
                             ساختار Curl_easy *data,
                             ساختار Curl_peer *origin,
                             ساختار Curl_peer *peer,
                             uint8_t transport_peer,
                             ساختار connectdata *conn,
                             ساختار Curl_sockaddr_ex *addr,
                             ساختار Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

/**
 * Creates a cfilter that keeps a listening socket.
 */
CURLcode Curl_conn_tcp_listen_set(ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  int8_t sockindex,
                                  curl_socket_t *s);

/**
 * Return TRUE iff the last filter at `sockindex` was set via
 * Curl_conn_tcp_listen_set().
 */
bool Curl_conn_is_tcp_listen(ساختار Curl_easy *data,
                             int8_t sockindex);

/**
 * Peek at the socket and remote ip/port the socket filter is using.
 * The filter owns all returned values.
 * @param psock             pointer to hold socket descriptor or NULL
 * @param paddr             pointer to hold addr reference or NULL
 * @param pip               pointer to get IP quadruple or NULL
 * Returns error if the filter is of invalid type.
 */
CURLcode Curl_cf_socket_peek(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             curl_socket_t *psock,
                             ثابت ساختار Curl_sockaddr_ex **paddr,
                             ساختار ip_quadruple *pip) WARN_UNUSED_RESULT;

بیرونی ساختار Curl_cftype Curl_cft_tcp;
بیرونی ساختار Curl_cftype Curl_cft_udp;
بیرونی ساختار Curl_cftype Curl_cft_unix;
بیرونی ساختار Curl_cftype Curl_cft_tcp_accept;

#endif /* HEADER_CURL_CF_SOCKET_H */
