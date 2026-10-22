#నిర్వచితం_కాకపోతే శీర్షిక_CURL_PROXY_H
#నిర్వచించు శీర్షిక_CURL_PROXY_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

నిర్మాణం Curl_easy;
నిర్మాణం Curl_peer;
నిర్మాణం Curl_creds;
నిర్మాణం connectdata;

నిర్మాణం proxy_info {
  నిర్మాణం Curl_peer *peer; /* proxy to this peer */
  నిర్మాణం Curl_creds *creds; /* use these credentials, maybe NULL */
  uint8_t proxytype; /* what kind of proxy that is in use */
};

#నిర్వచించు CURL_PROXY_IS_HTTPS(t)  \
  (((t) == CURLPROXY_HTTPS) ||  \
   ((t) == CURLPROXY_HTTPS2) || \
   ((t) == CURLPROXY_HTTPS3))

#నిర్వచించు CURL_PROXY_IS_HTTP(t)   \
  (((t) == CURLPROXY_HTTP) ||   \
   ((t) == CURLPROXY_HTTP_1_0))

#నిర్వచించు CURL_PROXY_IS_ANY_HTTP(t) \
  (CURL_PROXY_IS_HTTP(t) ||       \
   CURL_PROXY_IS_HTTPS(t))

CURLcode Curl_proxy_init_conn(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn);

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */

#అయితే_ముగింపు /* HEADER_CURL_PROXY_H */
