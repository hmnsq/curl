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
#അന്തര്ഭാവ "curl_setup.h"

ഘടന Curl_addrinfo;
ഘടന Curl_cfilter;
ഘടന Curl_easy;
ഘടന connectdata;
ഘടന Curl_sockaddr_ex;
ഘടന ip_quadruple;

#define curl_sa_addr    addr.sa
#define curl_sa_addrbuf addr.buf

/*
 * Parse interface option, and return the interface name and the host part.
 */
CURLcode Curl_parse_interface(സ്ഥിരം അക്ഷരം *നിവേശം,
                              അക്ഷരം **dev, അക്ഷരം **iface, അക്ഷരം **host);

CURLcode Curl_socket_addr_from_ai(ഘടന Curl_sockaddr_ex *addr,
                                  സ്ഥിരം ഘടന Curl_addrinfo *ai,
                                  uint8_t transport);

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * Fill in 'addr' and 'sockfd' accordingly if OK is returned. If the open
 * socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(ഘടന Curl_easy *data,
                          സ്ഥിരം ഘടന Curl_addrinfo *ai,
                          ഘടന Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd);

#ifdef USE_SO_NOSIGPIPE
/* Set SO_NOSIGPIPE on socket, return < 0 on error. */
പൂർണ്ണസംഖ്യ Curl_sock_nosigpipe(curl_socket_t sockfd);
#endif

പൂർണ്ണസംഖ്യ Curl_socket_close(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                      curl_socket_t sock);

/**
 * Creates a cfilter that opens a TCP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_tcp_create(ഘടന Curl_cfilter **pcf,
                            ഘടന Curl_easy *data,
                            ഘടന Curl_peer *origin,
                            ഘടന Curl_peer *peer,
                            uint8_t transport_peer,
                            ഘടന connectdata *conn,
                            ഘടന Curl_sockaddr_ex *addr,
                            ഘടന Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UDP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_udp_create(ഘടന Curl_cfilter **pcf,
                            ഘടന Curl_easy *data,
                            ഘടന Curl_peer *origin,
                            ഘടന Curl_peer *peer,
                            uint8_t transport_peer,
                            ഘടന connectdata *conn,
                            ഘടന Curl_sockaddr_ex *addr,
                            ഘടന Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UNIX socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_unix_create(ഘടന Curl_cfilter **pcf,
                             ഘടന Curl_easy *data,
                             ഘടന Curl_peer *origin,
                             ഘടന Curl_peer *peer,
                             uint8_t transport_peer,
                             ഘടന connectdata *conn,
                             ഘടന Curl_sockaddr_ex *addr,
                             ഘടന Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

/**
 * Creates a cfilter that keeps a listening socket.
 */
CURLcode Curl_conn_tcp_listen_set(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn,
                                  int8_t sockindex,
                                  curl_socket_t *s);

/**
 * Return TRUE iff the last filter at `sockindex` was set via
 * Curl_conn_tcp_listen_set().
 */
bool Curl_conn_is_tcp_listen(ഘടന Curl_easy *data,
                             int8_t sockindex);

/**
 * Peek at the socket and remote ip/port the socket filter is using.
 * The filter owns all returned values.
 * @param psock             pointer to hold socket descriptor or NULL
 * @param paddr             pointer to hold addr reference or NULL
 * @param pip               pointer to get IP quadruple or NULL
 * Returns error if the filter is of invalid type.
 */
CURLcode Curl_cf_socket_peek(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             curl_socket_t *psock,
                             സ്ഥിരം ഘടന Curl_sockaddr_ex **paddr,
                             ഘടന ip_quadruple *pip) WARN_UNUSED_RESULT;

ബാഹ്യ ഘടന Curl_cftype Curl_cft_tcp;
ബാഹ്യ ഘടന Curl_cftype Curl_cft_udp;
ബാഹ്യ ഘടന Curl_cftype Curl_cft_unix;
ബാഹ്യ ഘടന Curl_cftype Curl_cft_tcp_accept;

#endif /* HEADER_CURL_CF_SOCKET_H */
