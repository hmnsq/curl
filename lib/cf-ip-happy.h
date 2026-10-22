#ifndef HEADER_CURL_IP_HAPPY_H
#define HEADER_CURL_IP_HAPPY_H
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
#অন্তর্ভাব "curl_setup.h"

কাঠামো connectdata;
কাঠামো Curl_addrinfo;
কাঠামো Curl_cfilter;
কাঠামো Curl_easy;
কাঠামো Curl_peer;
কাঠামো Curl_sockaddr_ex;

/**
 * Create a cfilter to connect to `origin` via an optional `peer`
 * using `transport_peer` and `addr`.
 * With a `tunnel_peer` present, the filter will be used to proxy tunnel
 * to it and the tunnel will use `tunnel_transport`.
 * `pcf`: the filter created on success
 * `data`: the transfer initiating the connect
 * `conn`: the connection that gets connected
 *
 * The filter is used in "happy eyeball" scenarios. Once connected,
 * it MAY be installed in the connection filter chain to serve transfers.
 */
প্রকারলেখ CURLcode cf_ip_connect_create(কাঠামো Curl_cfilter **pcf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো Curl_peer *origin,
                                      কাঠামো Curl_peer *peer,
                                      uint8_t transport_peer,
                                      কাঠামো connectdata *conn,
                                      কাঠামো Curl_sockaddr_ex *addr,
                                      কাঠামো Curl_peer *tunnel_peer,
                                      uint8_t tunnel_transport);

/**
 * Create an IP happy eyeball connection filter that connects to `origin`
 * via an optional `peer` using `transport_peer`.
 * With a `tunnel_peer` present, the filter will be used to proxy tunnel
 * to it and the tunnel will use `tunnel_transport`.
 */
CURLcode Curl_cf_ip_happy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_peer *origin,
                                       কাঠামো Curl_peer *peer,
                                       uint8_t transport_peer,
                                       কাঠামো Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

বাহ্যিক কাঠামো Curl_cftype Curl_cft_ip_happy;

#endif /* HEADER_CURL_IP_HAPPY_H */
