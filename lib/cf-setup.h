#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CF_ਤਿਆਰੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CF_ਤਿਆਰੀ_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ;
ਬਣਤਰ ip_quadruple;
ਬਣਤਰ Curl_peer;
ਬਣਤਰ Curl_str;

CURLcode Curl_cf_ਤਿਆਰੀ_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           ਪੂਰਨ ssl_ਬਹੁਲਕ);

CURLcode Curl_cf_ਤਿਆਰੀ_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    uint8_t transport,
                                    ਪੂਰਨ ssl_ਬਹੁਲਕ);

ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_ਤਿਆਰੀ;

#ਜੇ_ਅੰਤ /* HEADER_CURL_CF_SETUP_H */
