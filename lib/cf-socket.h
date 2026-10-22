#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CF_SOCKET_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CF_SOCKET_H
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

ਬਣਤਰ Curl_addrinfo;
ਬਣਤਰ Curl_cfilter;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;
ਬਣਤਰ Curl_sockaddr_ex;
ਬਣਤਰ ip_quadruple;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_sa_addr    addr.sa
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_sa_addrbuf addr.buf

/*
 * Parse interface option, and return the interface name and the host part.
 */
CURLcode Curl_parse_interface(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼,
                              ਅੱਖਰ **dev, ਅੱਖਰ **iface, ਅੱਖਰ **host);

CURLcode Curl_socket_addr_from_ai(ਬਣਤਰ Curl_sockaddr_ex *addr,
                                  ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai,
                                  uint8_t transport);

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * Fill in 'addr' and 'sockfd' accordingly if OK is returned. If the open
 * socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਬਣਤਰ Curl_addrinfo *ai,
                          ਬਣਤਰ Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SO_NOSIGPIPE
/* Set SO_NOSIGPIPE on socket, return < 0 on error. */
ਪੂਰਨ Curl_sock_nosigpipe(curl_socket_t sockfd);
#ਜੇ_ਅੰਤ

ਪੂਰਨ Curl_socket_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                      curl_socket_t sock);

/**
 * Creates a cfilter that opens a TCP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_tcp_create(ਬਣਤਰ Curl_cfilter **pcf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_peer *origin,
                            ਬਣਤਰ Curl_peer *peer,
                            uint8_t transport_peer,
                            ਬਣਤਰ connectdata *conn,
                            ਬਣਤਰ Curl_sockaddr_ex *addr,
                            ਬਣਤਰ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UDP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_udp_create(ਬਣਤਰ Curl_cfilter **pcf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_peer *origin,
                            ਬਣਤਰ Curl_peer *peer,
                            uint8_t transport_peer,
                            ਬਣਤਰ connectdata *conn,
                            ਬਣਤਰ Curl_sockaddr_ex *addr,
                            ਬਣਤਰ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UNIX socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_unix_create(ਬਣਤਰ Curl_cfilter **pcf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_peer *origin,
                             ਬਣਤਰ Curl_peer *peer,
                             uint8_t transport_peer,
                             ਬਣਤਰ connectdata *conn,
                             ਬਣਤਰ Curl_sockaddr_ex *addr,
                             ਬਣਤਰ Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

/**
 * Creates a cfilter that keeps a listening socket.
 */
CURLcode Curl_conn_tcp_listen_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  int8_t sockindex,
                                  curl_socket_t *s);

/**
 * Return TRUE iff the last filter at `sockindex` was set via
 * Curl_conn_tcp_listen_set().
 */
bool Curl_conn_is_tcp_listen(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             int8_t sockindex);

/**
 * Peek at the socket and remote ip/port the socket filter is using.
 * The filter owns all returned values.
 * @param psock             pointer to hold socket descriptor or NULL
 * @param paddr             pointer to hold addr reference or NULL
 * @param pip               pointer to get IP quadruple or NULL
 * Returns error if the filter is of invalid type.
 */
CURLcode Curl_cf_socket_peek(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             curl_socket_t *psock,
                             ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex **paddr,
                             ਬਣਤਰ ip_quadruple *pip) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_tcp;
ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_udp;
ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_unix;
ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_tcp_accept;

#ਜੇ_ਅੰਤ /* HEADER_CURL_CF_SOCKET_H */
