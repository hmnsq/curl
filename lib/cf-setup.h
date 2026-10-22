#اگر_غیر_تعریف_شدہ سرخی_CURL_CF_تیاری_H
#تعریف_کرو سرخی_CURL_CF_تیاری_H
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

ساخت Curl_dns_اندراج;
ساخت ip_quadruple;
ساخت Curl_peer;
ساخت Curl_str;

CURLcode Curl_cf_تیاری_جوڑو(ساخت Curl_easy *ڈیٹا,
                           ساخت connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحیح ssl_منوال);

CURLcode Curl_cf_تیاری_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                    ساخت Curl_easy *ڈیٹا,
                                    uint8_t transport,
                                    صحیح ssl_منوال);

خارجی ساخت Curl_cftype Curl_cft_تیاری;

#اگر_ختم /* HEADER_CURL_CF_SETUP_H */
