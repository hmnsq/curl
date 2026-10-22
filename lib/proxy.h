#اگر_غیر_تعریف_شدہ سرخی_CURL_PROXY_H
#تعریف_کرو سرخی_CURL_PROXY_H
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
#شامل "curl_setup.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

ساخت Curl_easy;
ساخت Curl_peer;
ساخت Curl_creds;
ساخت connectdata;

ساخت proxy_info {
  ساخت Curl_peer *peer; /* proxy to this peer */
  ساخت Curl_creds *creds; /* use these credentials, maybe NULL */
  uint8_t proxytype; /* what kind of proxy that is in use */
};

#تعریف_کرو CURL_PROXY_IS_HTTPS(t)  \
  (((t) == CURLPROXY_HTTPS) ||  \
   ((t) == CURLPROXY_HTTPS2) || \
   ((t) == CURLPROXY_HTTPS3))

#تعریف_کرو CURL_PROXY_IS_HTTP(t)   \
  (((t) == CURLPROXY_HTTP) ||   \
   ((t) == CURLPROXY_HTTP_1_0))

#تعریف_کرو CURL_PROXY_IS_ANY_HTTP(t) \
  (CURL_PROXY_IS_HTTP(t) ||       \
   CURL_PROXY_IS_HTTPS(t))

CURLcode Curl_proxy_init_conn(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn);

#اگر_ختم /* !CURL_DISABLE_PROXY */

#اگر_ختم /* HEADER_CURL_PROXY_H */
