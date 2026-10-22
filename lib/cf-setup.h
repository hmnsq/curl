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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ସଂରଚନା Curl_dns_entry;
ସଂରଚନା ip_quadruple;
ସଂରଚନା Curl_peer;
ସଂରଚନା Curl_str;

CURLcode Curl_cf_setup_add(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_mode);

CURLcode Curl_cf_setup_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                    ସଂରଚନା Curl_easy *data,
                                    uint8_t transport,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_mode);

ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_setup;

#endif /* HEADER_CURL_CF_SETUP_H */
