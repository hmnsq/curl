#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HTTP_PROXY_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HTTP_PROXY_H
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

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ಅನ್ತರ್ಭಾವ "urldata.h"

ಎಣಿಕೆ Curl_proxy_use {
  ಶೀರ್ಷಿಕೆ_SERVER,  /* direct to server */
  ಶೀರ್ಷಿಕೆ_PROXY,   /* regular request to proxy */
  ಶೀರ್ಷಿಕೆ_CONNECT, /* sending CONNECT to a proxy */
  ಶೀರ್ಷಿಕೆ_CONNECT_UDP /* sending CONNECT-UDP to a proxy */
};

/* HTTP version for proxy tunnel request creation */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  PROXY_HTTP_V1 = 1,
  PROXY_HTTP_V2 = 2,
  PROXY_HTTP_V3 = 3
} proxy_http_ver;

/* Result from inspecting a proxy tunnel response */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  PROXY_INSPECT_OK,         /* Tunnel established */
  PROXY_INSPECT_FAILED,     /* Tunnel failed */
  PROXY_INSPECT_AUTH_RETRY  /* Retry with auth */
} proxy_inspect_ಫಲಿತಾಂಶ;

/* Create CONNECT or CONNECT-UDP request */
CURLcode Curl_http_proxy_create_tunnel_request(
    ರಚನೆ httpreq **preq, ರಚನೆ Curl_cfilter *cf,
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_peer *peer, ರಚನೆ Curl_peer *dest,
    proxy_http_ver ver, bool udp_tunnel);

/* Inspect tunnel response for H2/H3 proxy (capsule-protocol, auth) */
ರಚನೆ http_resp;
CURLcode Curl_http_proxy_inspect_tunnel_response(
    ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
    ರಚನೆ http_resp *resp, bool udp_tunnel,
    proxy_inspect_ಫಲಿತಾಂಶ *presult);

/* Default proxy timeout in milliseconds */
#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_TIMEOUT (3600 * 1000)

CURLcode Curl_cf_http_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ Curl_peer *peer,
                                         ರಚನೆ Curl_peer *tunnel_peer,
                                         uint8_t tunnel_transport,
                                         uint8_t proxytype);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_http_proxy;

uint8_t Curl_http_proxy_transport(uint8_t proxytype);

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HTTP_PROXY_H */
