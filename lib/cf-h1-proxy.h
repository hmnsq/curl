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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

ഘടന Curl_peer;

CURLcode Curl_cf_h1_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                       ഘടന Curl_easy *data,
                                       ഘടന Curl_peer *peer,
                                       ഘടന Curl_peer *dest,
                                       പൂർണ്ണസംഖ്യ httpversion,
                                       bool udp_tunnel);

ബാഹ്യ ഘടന Curl_cftype Curl_cft_h1_proxy;

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */

#endif /* HEADER_CURL_H1_PROXY_H */
