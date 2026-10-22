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
#અન્તર્ભાવ "curl_setup.h"

સંરચના Curl_dns_entry;
સંરચના ip_quadruple;
સંરચના Curl_peer;
સંરચના Curl_str;

CURLcode Curl_cf_setup_add(સંરચના Curl_easy *data,
                           સંરચના connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           પૂર્ણાંક ssl_mode);

CURLcode Curl_cf_setup_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                    સંરચના Curl_easy *data,
                                    uint8_t transport,
                                    પૂર્ણાંક ssl_mode);

બાહ્ય સંરચના Curl_cftype Curl_cft_setup;

#endif /* HEADER_CURL_CF_SETUP_H */
