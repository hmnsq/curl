#ifndef HEADER_CURL_CF_SETUP_H
#define HEADER_CURL_CF_SETUP_H
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

ساختار Curl_dns_entry;
ساختار ip_quadruple;
ساختار Curl_peer;
ساختار Curl_str;

CURLcode Curl_cf_setup_add(ساختار Curl_easy *data,
                           ساختار connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحیح ssl_mode);

CURLcode Curl_cf_setup_درج_after(ساختار Curl_cfilter *cf_at,
                                    ساختار Curl_easy *data,
                                    uint8_t transport,
                                    صحیح ssl_mode);

بیرونی ساختار Curl_cftype Curl_cft_setup;

#endif /* HEADER_CURL_CF_SETUP_H */
