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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#অন্তর্ভাব "urldata.h"

গণনা Curl_proxy_use {
  HEADER_SERVER,  /* direct to server */
  HEADER_PROXY,   /* regular request to proxy */
  HEADER_CONNECT, /* sending CONNECT to a proxy */
  HEADER_CONNECT_UDP /* sending CONNECT-UDP to a proxy */
};

/* HTTP version for proxy tunnel request creation */
প্রকারলেখ গণনা {
  PROXY_HTTP_V1 = 1,
  PROXY_HTTP_V2 = 2,
  PROXY_HTTP_V3 = 3
} proxy_http_ver;

/* Result from inspecting a proxy tunnel response */
প্রকারলেখ গণনা {
  PROXY_INSPECT_OK,         /* Tunnel established */
  PROXY_INSPECT_FAILED,     /* Tunnel failed */
  PROXY_INSPECT_AUTH_RETRY  /* Retry with auth */
} proxy_inspect_ফলাফল;

/* Create CONNECT or CONNECT-UDP request */
CURLcode Curl_http_proxy_create_tunnel_request(
    কাঠামো httpreq **preq, কাঠামো Curl_cfilter *cf,
    কাঠামো Curl_easy *তথ্য, কাঠামো Curl_peer *peer, কাঠামো Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel);

/* Inspect tunnel response for H2/H3 proxy (capsule-protocol, auth) */
কাঠামো http_resp;
CURLcode Curl_http_proxy_inspect_tunnel_response(
    কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
    কাঠামো http_resp *resp, bool udp_tunnel,
    proxy_inspect_ফলাফল *presult);

/* Default proxy timeout in milliseconds */
#define PROXY_TIMEOUT (3600 * 1000)

CURLcode Curl_cf_http_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো Curl_peer *peer,
                                         কাঠামো Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype);

বাহ্যিক কাঠামো Curl_cftype Curl_cft_http_proxy;

uint8_t Curl_http_proxy_transport(uint8_t proxytype);

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#endif /* HEADER_CURL_HTTP_PROXY_H */
