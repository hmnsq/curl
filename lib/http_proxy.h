#अगर_अघोषित HEADER_CURL_HTTP_PROXY_H
#घोषणा HEADER_CURL_HTTP_PROXY_H
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

#अगर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#समावेश "urldata.h"

क्रमागत Curl_proxy_use {
  HEADER_SERVER,  /* direct to server */
  HEADER_PROXY,   /* regular request to proxy */
  HEADER_CONNECT, /* sending CONNECT to a proxy */
  HEADER_CONNECT_UDP /* sending CONNECT-UDP to a proxy */
};

/* HTTP version for proxy tunnel request creation */
प्रकार क्रमागत {
  PROXY_HTTP_V1 = 1,
  PROXY_HTTP_V2 = 2,
  PROXY_HTTP_V3 = 3
} proxy_http_ver;

/* Result from inspecting a proxy tunnel response */
प्रकार क्रमागत {
  PROXY_INSPECT_OK,         /* Tunnel established */
  PROXY_INSPECT_FAILED,     /* Tunnel failed */
  PROXY_INSPECT_AUTH_RETRY  /* Retry with auth */
} proxy_inspect_परिणाम;

/* Create CONNECT or CONNECT-UDP request */
CURLcode Curl_http_proxy_create_tunnel_request(
    काष्ठा httpreq **preq, काष्ठा Curl_cfilter *cf,
    काष्ठा Curl_easy *तथ्य, काष्ठा Curl_peer *peer, काष्ठा Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel);

/* Inspect tunnel response for H2/H3 proxy (capsule-protocol, auth) */
काष्ठा http_resp;
CURLcode Curl_http_proxy_inspect_tunnel_response(
    काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
    काष्ठा http_resp *resp, bool udp_tunnel,
    proxy_inspect_परिणाम *presult);

/* Default proxy timeout in milliseconds */
#घोषणा PROXY_TIMEOUT (3600 * 1000)

CURLcode Curl_cf_http_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_peer *peer,
                                         काष्ठा Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype);

बाह्य काष्ठा Curl_cftype Curl_cft_http_proxy;

uint8_t Curl_http_proxy_transport(uint8_t proxytype);

#पूर्ण_अगर /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#पूर्ण_अगर /* HEADER_CURL_HTTP_PROXY_H */
