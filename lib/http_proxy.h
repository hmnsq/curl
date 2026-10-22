#ifndef HEADER_CURL_HTTP_PROXY_H
#define HEADER_CURL_HTTP_PROXY_H
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
#include "curl_setup.h"

#ᬬᬾᬦ᭄ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#include "urldata.h"

ga_nanaa Curl_proxy_use {
  HEADER_SERVER,  /* direct to server */
  HEADER_PROXY,   /* regular request to proxy */
  HEADER_CONNECT, /* sending CONNECT to a proxy */
  HEADER_CONNECT_UDP /* sending CONNECT-UDP to a proxy */
};

/* HTTP version for proxy tunnel request creation */
typedef ga_nanaa {
  PROXY_HTTP_V1 = 1,
  PROXY_HTTP_V2 = 2,
  PROXY_HTTP_V3 = 3
} proxy_http_ver;

/* Result from inspecting a proxy tunnel response */
typedef ga_nanaa {
  PROXY_INSPECT_OK,         /* Tunnel established */
  PROXY_INSPECT_FAILED,     /* Tunnel failed */
  PROXY_INSPECT_AUTH_RETRY  /* Retry with auth */
} proxy_inspect_result;

/* Create CONNECT or CONNECT-UDP request */
CURLcode Curl_http_proxy_create_tunnel_request(
    sa_nracanaa httpreq **preq, sa_nracanaa Curl_cfilter *cf,
    sa_nracanaa Curl_easy *data, sa_nracanaa Curl_peer *peer, sa_nracanaa Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel);

/* Inspect tunnel response for H2/H3 proxy (capsule-protocol, auth) */
sa_nracanaa http_resp;
CURLcode Curl_http_proxy_inspect_tunnel_response(
    sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
    sa_nracanaa http_resp *resp, bool udp_tunnel,
    proxy_inspect_result *presult);

/* Default proxy timeout in milliseconds */
#define PROXY_TIMEOUT (3600 * 1000)

CURLcode Curl_cf_http_proxy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                         sa_nracanaa Curl_easy *data,
                                         sa_nracanaa Curl_peer *peer,
                                         sa_nracanaa Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype);

extern sa_nracanaa Curl_cftype Curl_cft_http_proxy;

uint8_t Curl_http_proxy_transport(uint8_t proxytype);

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#endif /* HEADER_CURL_HTTP_PROXY_H */
