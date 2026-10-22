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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

संरचना Curl_cfilter;
संरचना Curl_easy;
संरचना connectdata;
संरचना Curl_cftype;
संरचना Curl_peer;

बाह्य संरचना Curl_cftype Curl_cft_http_connect;

CURLcode Curl_cf_https_setup(संरचना Curl_easy *data,
                             संरचना Curl_peer *destination,
                             संरचना connectdata *conn,
                             int8_t sockindex);

#endif /* !CURL_DISABLE_HTTP */
#endif /* HEADER_CURL_CF_HTTP_H */
