#यदि_अपरिभाषितम् शीर्षकम्_CURL_HTTP_PROXY_H
#परिभाषय शीर्षकम्_CURL_HTTP_PROXY_H
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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#अन्तर्भाव "urldata.h"

गणना Curl_proxy_use {
  शीर्षकम्_SERVER,  /* direct to server */
  शीर्षकम्_PROXY,   /* regular request to proxy */
  शीर्षकम्_CONNECT, /* sending CONNECT to a proxy */
  शीर्षकम्_CONNECT_UDP /* sending CONNECT-UDP to a proxy */
};

/* HTTP version for proxy tunnel request creation */
प्रकारनिर्देश गणना {
  PROXY_HTTP_V1 = 1,
  PROXY_HTTP_V2 = 2,
  PROXY_HTTP_V3 = 3
} proxy_http_ver;

/* Result from inspecting a proxy tunnel response */
प्रकारनिर्देश गणना {
  PROXY_INSPECT_OK,         /* Tunnel established */
  PROXY_INSPECT_FAILED,     /* Tunnel failed */
  PROXY_INSPECT_AUTH_RETRY  /* Retry with auth */
} proxy_inspect_परिणाम;

/* Create CONNECT or CONNECT-UDP request */
CURLcode Curl_http_proxy_create_tunnel_request(
    संरचना httpreq **preq, संरचना Curl_cfilter *cf,
    संरचना Curl_easy *दत्त, संरचना Curl_peer *peer, संरचना Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel);

/* Inspect tunnel response for H2/H3 proxy (capsule-protocol, auth) */
संरचना http_resp;
CURLcode Curl_http_proxy_inspect_tunnel_response(
    संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
    संरचना http_resp *resp, bool udp_tunnel,
    proxy_inspect_परिणाम *presult);

/* Default proxy timeout in milliseconds */
#परिभाषय PROXY_TIMEOUT (3600 * 1000)

CURLcode Curl_cf_http_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                         संरचना Curl_easy *दत्त,
                                         संरचना Curl_peer *peer,
                                         संरचना Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype);

बाह्य संरचना Curl_cftype Curl_cft_http_proxy;

uint8_t Curl_http_proxy_transport(uint8_t proxytype);

#यदि_अन्तः /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#यदि_अन्तः /* HEADER_CURL_HTTP_PROXY_H */
