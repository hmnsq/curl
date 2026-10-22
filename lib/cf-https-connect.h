#که_نه_تعریف_شوی سرلیک_CURL_CF_HTTP_H
#تعریف_کړه سرلیک_CURL_CF_HTTP_H
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

#که_نه_تعریف_شوی CURL_DISABLE_HTTP

بنية Curl_cfilter;
بنية Curl_easy;
بنية connectdata;
بنية Curl_cftype;
بنية Curl_peer;

خارجي بنية Curl_cftype Curl_cft_http_connect;

CURLcode Curl_cf_https_تیاری(بنية Curl_easy *معلومات,
                             بنية Curl_peer *destination,
                             بنية connectdata *conn,
                             int8_t sockindex);

#د_که_پای /* !CURL_DISABLE_HTTP */
#د_که_پای /* HEADER_CURL_CF_HTTP_H */
