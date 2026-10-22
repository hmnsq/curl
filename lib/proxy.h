#جے_بنا_تعریف سرخی_CURL_PROXY_H
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
#ضمن "curl_setup.h"

#جے_بنا_تعریف CURL_DISABLE_PROXY

بنية Curl_easy;
بنية Curl_peer;
بنية Curl_creds;
بنية connectdata;

بنية proxy_info {
  بنية Curl_peer *peer; /* proxy to this peer */
  بنية Curl_creds *creds; /* use these credentials, maybe NULL */
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

CURLcode Curl_proxy_init_conn(بنية Curl_easy *انکڑے,
                              بنية connectdata *conn);

#جے_انت /* !CURL_DISABLE_PROXY */

#جے_انت /* HEADER_CURL_PROXY_H */
