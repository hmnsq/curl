#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_HTTP_PROXY_H
#وصف_ڪريو سرو_CURL_HTTP_PROXY_H
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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ضمن "urldata.h"

تعداد Curl_proxy_use {
  سرو_SERVER,  /* direct to server */
  سرو_PROXY,   /* regular request to proxy */
  سرو_CONNECT, /* sending CONNECT to a proxy */
  سرو_CONNECT_UDP /* sending CONNECT-UDP to a proxy */
};

/* HTTP version for proxy tunnel request creation */
تعريفنوع تعداد {
  PROXY_HTTP_V1 = 1,
  PROXY_HTTP_V2 = 2,
  PROXY_HTTP_V3 = 3
} proxy_http_ver;

/* Result from inspecting a proxy tunnel response */
تعريفنوع تعداد {
  PROXY_INSPECT_OK,         /* Tunnel established */
  PROXY_INSPECT_FAILED,     /* Tunnel failed */
  PROXY_INSPECT_AUTH_RETRY  /* Retry with auth */
} proxy_inspect_نتيجو;

/* Create CONNECT or CONNECT-UDP request */
CURLcode Curl_http_proxy_create_tunnel_request(
    بنية httpreq **preq, بنية Curl_cfilter *cf,
    بنية Curl_easy *معلومات, بنية Curl_peer *peer, بنية Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel);

/* Inspect tunnel response for H2/H3 proxy (capsule-protocol, auth) */
بنية http_resp;
CURLcode Curl_http_proxy_inspect_tunnel_response(
    بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
    بنية http_resp *resp, bool udp_tunnel,
    proxy_inspect_نتيجو *presult);

/* Default proxy timeout in milliseconds */
#وصف_ڪريو PROXY_TIMEOUT (3600 * 1000)

CURLcode Curl_cf_http_proxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                         بنية Curl_easy *معلومات,
                                         بنية Curl_peer *peer,
                                         بنية Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype);

خارجي بنية Curl_cftype Curl_cft_http_proxy;

uint8_t Curl_http_proxy_transport(uint8_t proxytype);

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#جيڪڏهن_پورو /* HEADER_CURL_HTTP_PROXY_H */
