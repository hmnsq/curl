#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HTTP_PROXY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HTTP_PROXY_H
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

#ਜੇ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

ਗਿਣਤੀ Curl_proxy_use {
  ਸਿਰਲੇਖ_SERVER,  /* direct to server */
  ਸਿਰਲੇਖ_PROXY,   /* regular request to proxy */
  ਸਿਰਲੇਖ_CONNECT, /* sending CONNECT to a proxy */
  ਸਿਰਲੇਖ_CONNECT_UDP /* sending CONNECT-UDP to a proxy */
};

/* HTTP version for proxy tunnel request creation */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  PROXY_HTTP_V1 = 1,
  PROXY_HTTP_V2 = 2,
  PROXY_HTTP_V3 = 3
} proxy_http_ver;

/* Result from inspecting a proxy tunnel response */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  PROXY_INSPECT_OK,         /* Tunnel established */
  PROXY_INSPECT_FAILED,     /* Tunnel failed */
  PROXY_INSPECT_AUTH_RETRY  /* Retry with auth */
} proxy_inspect_ਨਤੀਜਾ;

/* Create CONNECT or CONNECT-UDP request */
CURLcode Curl_http_proxy_create_tunnel_request(
    ਬਣਤਰ httpreq **preq, ਬਣਤਰ Curl_cfilter *cf,
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_peer *peer, ਬਣਤਰ Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel);

/* Inspect tunnel response for H2/H3 proxy (capsule-protocol, auth) */
ਬਣਤਰ http_resp;
CURLcode Curl_http_proxy_inspect_tunnel_response(
    ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
    ਬਣਤਰ http_resp *resp, bool udp_tunnel,
    proxy_inspect_ਨਤੀਜਾ *presult);

/* Default proxy timeout in milliseconds */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_TIMEOUT (3600 * 1000)

CURLcode Curl_cf_http_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_peer *peer,
                                         ਬਣਤਰ Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype);

ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_http_proxy;

uint8_t Curl_http_proxy_transport(uint8_t proxytype);

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#ਜੇ_ਅੰਤ /* HEADER_CURL_HTTP_PROXY_H */
