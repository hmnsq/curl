#ifndef HEADER_CURL_H1_PROXY_H
#define HEADER_CURL_H1_PROXY_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

ସଂରଚନା Curl_peer;

CURLcode Curl_cf_h1_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_peer *peer,
                                       ସଂରଚନା Curl_peer *dest,
                                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା httpversion,
                                       bool udp_tunnel);

ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_h1_proxy;

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#endif /* HEADER_CURL_H1_PROXY_H */
