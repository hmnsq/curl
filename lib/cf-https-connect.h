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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

ساختار Curl_cfilter;
ساختار Curl_easy;
ساختار connectdata;
ساختار Curl_cftype;
ساختار Curl_peer;

بیرونی ساختار Curl_cftype Curl_cft_http_connect;

CURLcode Curl_cf_https_setup(ساختار Curl_easy *data,
                             ساختار Curl_peer *destination,
                             ساختار connectdata *conn,
                             int8_t sockindex);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_CF_HTTP_H */
