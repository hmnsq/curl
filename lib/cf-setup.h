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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ಸಂರಚನಾ Curl_dns_entry;
ಸಂರಚನಾ ip_quadruple;
ಸಂರಚನಾ Curl_peer;
ಸಂರಚನಾ Curl_str;

CURLcode Curl_cf_setup_add(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ಪೂರ್ಣಾಙ್ಕ ssl_mode);

CURLcode Curl_cf_setup_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    uint8_t transport,
                                    ಪೂರ್ಣಾಙ್ಕ ssl_mode);

ಬಾಹ್ಯ ಸಂರಚನಾ Curl_cftype Curl_cft_setup;

#endif /* HEADER_CURL_CF_SETUP_H */
