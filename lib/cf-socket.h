#अगर_अघोषित HEADER_CURL_CF_SOCKET_H
#घोषणा HEADER_CURL_CF_SOCKET_H
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

काष्ठा Curl_addrinfo;
काष्ठा Curl_cfilter;
काष्ठा Curl_easy;
काष्ठा connectdata;
काष्ठा Curl_sockaddr_ex;
काष्ठा ip_quadruple;

#घोषणा curl_sa_addr    addr.sa
#घोषणा curl_sa_addrbuf addr.buf

/*
 * Parse interface option, and return the interface name and the host part.
 */
CURLcode Curl_parse_interface(स्थिर अक्षर *निवेश,
                              अक्षर **dev, अक्षर **iface, अक्षर **host);

CURLcode Curl_socket_addr_from_ai(काष्ठा Curl_sockaddr_ex *addr,
                                  स्थिर काष्ठा Curl_addrinfo *ai,
                                  uint8_t transport);

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * Fill in 'addr' and 'sockfd' accordingly if OK is returned. If the open
 * socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(काष्ठा Curl_easy *तथ्य,
                          स्थिर काष्ठा Curl_addrinfo *ai,
                          काष्ठा Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd);

#अगर_घोषित USE_SO_NOSIGPIPE
/* Set SO_NOSIGPIPE on socket, return < 0 on error. */
पूर्णांक Curl_sock_nosigpipe(curl_socket_t sockfd);
#पूर्ण_अगर

पूर्णांक Curl_socket_close(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                      curl_socket_t sock);

/**
 * Creates a cfilter that opens a TCP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_tcp_create(काष्ठा Curl_cfilter **pcf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_peer *origin,
                            काष्ठा Curl_peer *peer,
                            uint8_t transport_peer,
                            काष्ठा connectdata *conn,
                            काष्ठा Curl_sockaddr_ex *addr,
                            काष्ठा Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UDP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_udp_create(काष्ठा Curl_cfilter **pcf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा Curl_peer *origin,
                            काष्ठा Curl_peer *peer,
                            uint8_t transport_peer,
                            काष्ठा connectdata *conn,
                            काष्ठा Curl_sockaddr_ex *addr,
                            काष्ठा Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UNIX socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_unix_create(काष्ठा Curl_cfilter **pcf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_peer *origin,
                             काष्ठा Curl_peer *peer,
                             uint8_t transport_peer,
                             काष्ठा connectdata *conn,
                             काष्ठा Curl_sockaddr_ex *addr,
                             काष्ठा Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

/**
 * Creates a cfilter that keeps a listening socket.
 */
CURLcode Curl_conn_tcp_listen_set(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  int8_t sockindex,
                                  curl_socket_t *s);

/**
 * Return TRUE iff the last filter at `sockindex` was set via
 * Curl_conn_tcp_listen_set().
 */
bool Curl_conn_is_tcp_listen(काष्ठा Curl_easy *तथ्य,
                             int8_t sockindex);

/**
 * Peek at the socket and remote ip/port the socket filter is using.
 * The filter owns all returned values.
 * @param psock             pointer to hold socket descriptor or NULL
 * @param paddr             pointer to hold addr reference or NULL
 * @param pip               pointer to get IP quadruple or NULL
 * Returns error if the filter is of invalid type.
 */
CURLcode Curl_cf_socket_peek(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             curl_socket_t *psock,
                             स्थिर काष्ठा Curl_sockaddr_ex **paddr,
                             काष्ठा ip_quadruple *pip) WARN_UNUSED_परिणाम;

बाह्य काष्ठा Curl_cftype Curl_cft_tcp;
बाह्य काष्ठा Curl_cftype Curl_cft_udp;
बाह्य काष्ठा Curl_cftype Curl_cft_unix;
बाह्य काष्ठा Curl_cftype Curl_cft_tcp_accept;

#पूर्ण_अगर /* HEADER_CURL_CF_SOCKET_H */
