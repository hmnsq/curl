#اگر_غیر_تعریف_شدہ سرخی_CURL_H2_PROXY_H
#تعریف_کرو سرخی_CURL_H2_PROXY_H
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

#اگر defined(USE_NGHTTP2) && !defined(CURL_DISABLE_PROXY)

CURLcode Curl_cf_h2_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_peer *peer,
                                       ساخت Curl_peer *dest,
                                       bool udp_tunnel);

خارجی ساخت Curl_cftype Curl_cft_h2_proxy;

#اگر_ختم /* USE_NGHTTP2 && !CURL_DISABLE_PROXY */

#اگر_ختم /* HEADER_CURL_H2_PROXY_H */
