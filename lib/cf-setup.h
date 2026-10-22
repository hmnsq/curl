#که_نه_تعریف_شوی سرلیک_CURL_CF_تیاری_H
#تعریف_کړه سرلیک_CURL_CF_تیاری_H
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

بنية Curl_dns_ثبت;
بنية ip_quadruple;
بنية Curl_peer;
بنية Curl_str;

CURLcode Curl_cf_تیاری_جمع_کړه(بنية Curl_easy *معلومات,
                           بنية connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           صحيح ssl_ډېرپېښ);

CURLcode Curl_cf_تیاری_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                    بنية Curl_easy *معلومات,
                                    uint8_t transport,
                                    صحيح ssl_ډېرپېښ);

خارجي بنية Curl_cftype Curl_cft_تیاری;

#د_که_پای /* HEADER_CURL_CF_SETUP_H */
