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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ସଂରଚନା Curl_addrinfo;
ସଂରଚନା Curl_cfilter;
ସଂରଚନା Curl_easy;
ସଂରଚନା connectdata;
ସଂରଚନା Curl_sockaddr_ex;
ସଂରଚନା ip_quadruple;

#define curl_sa_addr    addr.sa
#define curl_sa_addrbuf addr.buf

/*
 * Parse interface option, and return the interface name and the host part.
 */
CURLcode Curl_parse_interface(ଅଚଳ ଅକ୍ଷର *ନିବେଶ,
                              ଅକ୍ଷର **dev, ଅକ୍ଷର **iface, ଅକ୍ଷର **host);

CURLcode Curl_socket_addr_from_ai(ସଂରଚନା Curl_sockaddr_ex *addr,
                                  ଅଚଳ ସଂରଚନା Curl_addrinfo *ai,
                                  uint8_t transport);

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * Fill in 'addr' and 'sockfd' accordingly if OK is returned. If the open
 * socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(ସଂରଚନା Curl_easy *data,
                          ଅଚଳ ସଂରଚନା Curl_addrinfo *ai,
                          ସଂରଚନା Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd);

#ifdef USE_SO_NOSIGPIPE
/* Set SO_NOSIGPIPE on socket, return < 0 on error. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_sock_nosigpipe(curl_socket_t sockfd);
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_socket_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                      curl_socket_t sock);

/**
 * Creates a cfilter that opens a TCP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_tcp_create(ସଂରଚନା Curl_cfilter **pcf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_peer *origin,
                            ସଂରଚନା Curl_peer *peer,
                            uint8_t transport_peer,
                            ସଂରଚନା connectdata *conn,
                            ସଂରଚନା Curl_sockaddr_ex *addr,
                            ସଂରଚନା Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UDP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_udp_create(ସଂରଚନା Curl_cfilter **pcf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_peer *origin,
                            ସଂରଚନା Curl_peer *peer,
                            uint8_t transport_peer,
                            ସଂରଚନା connectdata *conn,
                            ସଂରଚନା Curl_sockaddr_ex *addr,
                            ସଂରଚନା Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UNIX socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_unix_create(ସଂରଚନା Curl_cfilter **pcf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_peer *origin,
                             ସଂରଚନା Curl_peer *peer,
                             uint8_t transport_peer,
                             ସଂରଚନା connectdata *conn,
                             ସଂରଚନା Curl_sockaddr_ex *addr,
                             ସଂରଚନା Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

/**
 * Creates a cfilter that keeps a listening socket.
 */
CURLcode Curl_conn_tcp_listen_set(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  int8_t sockindex,
                                  curl_socket_t *s);

/**
 * Return TRUE iff the last filter at `sockindex` was set via
 * Curl_conn_tcp_listen_set().
 */
bool Curl_conn_is_tcp_listen(ସଂରଚନା Curl_easy *data,
                             int8_t sockindex);

/**
 * Peek at the socket and remote ip/port the socket filter is using.
 * The filter owns all returned values.
 * @param psock             pointer to hold socket descriptor or NULL
 * @param paddr             pointer to hold addr reference or NULL
 * @param pip               pointer to get IP quadruple or NULL
 * Returns error if the filter is of invalid type.
 */
CURLcode Curl_cf_socket_peek(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             curl_socket_t *psock,
                             ଅଚଳ ସଂରଚନା Curl_sockaddr_ex **paddr,
                             ସଂରଚନା ip_quadruple *pip) WARN_UNUSED_RESULT;

ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_tcp;
ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_udp;
ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_unix;
ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_tcp_accept;

#endif /* HEADER_CURL_CF_SOCKET_H */
