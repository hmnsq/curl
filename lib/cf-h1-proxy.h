#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_H1_PROXY_H
#وصف_ڪريو سرو_CURL_H1_PROXY_H
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

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

بنية Curl_peer;

CURLcode Curl_cf_h1_proxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       صحيح httpversion,
                                       bool udp_tunnel);

خارجي بنية Curl_cftype Curl_cft_h1_proxy;

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#جيڪڏهن_پورو /* HEADER_CURL_H1_PROXY_H */
