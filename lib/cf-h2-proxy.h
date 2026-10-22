#ifndef HEADER_CURL_H2_PROXY_H
#define HEADER_CURL_H2_PROXY_H
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
#অন্তর্ভাৱ "curl_setup.h"

#যদি defined(USE_NGHTTP2) && !defined(CURL_DISABLE_PROXY)

CURLcode Curl_cf_h2_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা Curl_peer *peer,
                                       সংরচনা Curl_peer *dest,
                                       bool udp_tunnel);

বাহ্য সংরচনা Curl_cftype Curl_cft_h2_proxy;

#endif /* USE_NGHTTP2 && !CURL_DISABLE_PROXY */

#endif /* HEADER_CURL_H2_PROXY_H */
