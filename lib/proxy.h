#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_PROXY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_PROXY_H
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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_peer;
ਬਣਤਰ Curl_creds;
ਬਣਤਰ connectdata;

ਬਣਤਰ proxy_info {
  ਬਣਤਰ Curl_peer *peer; /* proxy to this peer */
  ਬਣਤਰ Curl_creds *creds; /* use these credentials, maybe NULL */
  uint8_t proxytype; /* what kind of proxy that is in use */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PROXY_IS_HTTPS(t)  \
  (((t) == CURLPROXY_HTTPS) ||  \
   ((t) == CURLPROXY_HTTPS2) || \
   ((t) == CURLPROXY_HTTPS3))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PROXY_IS_HTTP(t)   \
  (((t) == CURLPROXY_HTTP) ||   \
   ((t) == CURLPROXY_HTTP_1_0))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PROXY_IS_ANY_HTTP(t) \
  (CURL_PROXY_IS_HTTP(t) ||       \
   CURL_PROXY_IS_HTTPS(t))

CURLcode Curl_proxy_init_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn);

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */

#ਜੇ_ਅੰਤ /* HEADER_CURL_PROXY_H */
