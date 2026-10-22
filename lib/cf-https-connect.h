#ifndef HEADER_CURL_CF_HTTP_H
#define HEADER_CURL_CF_HTTP_H
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

#ifndef CURL_DISABLE_HTTP

ഘടന Curl_cfilter;
ഘടന Curl_easy;
ഘടന connectdata;
ഘടന Curl_cftype;
ഘടന Curl_peer;

ബാഹ്യ ഘടന Curl_cftype Curl_cft_http_connect;

CURLcode Curl_cf_https_setup(ഘടന Curl_easy *data,
                             ഘടന Curl_peer *destination,
                             ഘടന connectdata *conn,
                             int8_t sockindex);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_CF_HTTP_H */
