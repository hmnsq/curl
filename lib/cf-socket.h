#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CF_SOCKET_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CF_SOCKET_H
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

ರಚನೆ Curl_addrinfo;
ರಚನೆ Curl_cfilter;
ರಚನೆ Curl_easy;
ರಚನೆ connectdata;
ರಚನೆ Curl_sockaddr_ex;
ರಚನೆ ip_quadruple;

#ವ್ಯಾಖ್ಯಾನಿಸು curl_sa_addr    addr.sa
#ವ್ಯಾಖ್ಯಾನಿಸು curl_sa_addrbuf addr.buf

/*
 * Parse interface option, and return the interface name and the host part.
 */
CURLcode Curl_parse_interface(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು,
                              ಅಕ್ಷರ **dev, ಅಕ್ಷರ **iface, ಅಕ್ಷರ **host);

CURLcode Curl_socket_addr_from_ai(ರಚನೆ Curl_sockaddr_ex *addr,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai,
                                  uint8_t transport);

/*
 * Create a socket based on info from 'conn' and 'ai'.
 *
 * Fill in 'addr' and 'sockfd' accordingly if OK is returned. If the open
 * socket callback is set, used that!
 *
 */
CURLcode Curl_socket_open(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_addrinfo *ai,
                          ರಚನೆ Curl_sockaddr_ex *addr,
                          uint8_t transport,
                          curl_socket_t *sockfd);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SO_NOSIGPIPE
/* Set SO_NOSIGPIPE on socket, return < 0 on error. */
ಪೂರ್ಣಾಂಕ Curl_sock_nosigpipe(curl_socket_t sockfd);
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ Curl_socket_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                      curl_socket_t sock);

/**
 * Creates a cfilter that opens a TCP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_tcp_create(ರಚನೆ Curl_cfilter **pcf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_peer *origin,
                            ರಚನೆ Curl_peer *peer,
                            uint8_t transport_peer,
                            ರಚನೆ connectdata *conn,
                            ರಚನೆ Curl_sockaddr_ex *addr,
                            ರಚನೆ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UDP socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_udp_create(ರಚನೆ Curl_cfilter **pcf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_peer *origin,
                            ರಚನೆ Curl_peer *peer,
                            uint8_t transport_peer,
                            ರಚನೆ connectdata *conn,
                            ರಚನೆ Curl_sockaddr_ex *addr,
                            ರಚನೆ Curl_peer *tunnel_peer,
                            uint8_t tunnel_transport);

/**
 * Creates a cfilter that opens a UNIX socket to the given address
 * when calling its `connect` implementation.
 * The filter will not touch any connection/data flags and can be
 * used in happy eyeballing. Once selected for use, its `_active()`
 * method needs to be called.
 */
CURLcode Curl_cf_unix_create(ರಚನೆ Curl_cfilter **pcf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_peer *origin,
                             ರಚನೆ Curl_peer *peer,
                             uint8_t transport_peer,
                             ರಚನೆ connectdata *conn,
                             ರಚನೆ Curl_sockaddr_ex *addr,
                             ರಚನೆ Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

/**
 * Creates a cfilter that keeps a listening socket.
 */
CURLcode Curl_conn_tcp_listen_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  int8_t sockindex,
                                  curl_socket_t *s);

/**
 * Return TRUE iff the last filter at `sockindex` was set via
 * Curl_conn_tcp_listen_set().
 */
bool Curl_conn_is_tcp_listen(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             int8_t sockindex);

/**
 * Peek at the socket and remote ip/port the socket filter is using.
 * The filter owns all returned values.
 * @param psock             pointer to hold socket descriptor or NULL
 * @param paddr             pointer to hold addr reference or NULL
 * @param pip               pointer to get IP quadruple or NULL
 * Returns error if the filter is of invalid type.
 */
CURLcode Curl_cf_socket_peek(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             curl_socket_t *psock,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex **paddr,
                             ರಚನೆ ip_quadruple *pip) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_tcp;
ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_udp;
ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_unix;
ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_tcp_accept;

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CF_SOCKET_H */
