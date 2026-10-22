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
#अन्तर्भाव "curl_setup.h"

संरचना Curl_dns_entry;
संरचना ip_quadruple;
संरचना Curl_peer;
संरचना Curl_str;

CURLcode Curl_cf_setup_add(संरचना Curl_easy *data,
                           संरचना connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           पूर्णाङ्क ssl_mode);

CURLcode Curl_cf_setup_घुसाउ_after(संरचना Curl_cfilter *cf_at,
                                    संरचना Curl_easy *data,
                                    uint8_t transport,
                                    पूर्णाङ्क ssl_mode);

बाह्य संरचना Curl_cftype Curl_cft_setup;

#endif /* HEADER_CURL_CF_SETUP_H */
