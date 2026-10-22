#यदि_अपरिभाषितम् शीर्षकम्_CURL_CF_सज्जता_H
#परिभाषय शीर्षकम्_CURL_CF_सज्जता_H
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

संरचना Curl_dns_प्रविष्टिः;
संरचना ip_quadruple;
संरचना Curl_peer;
संरचना Curl_str;

CURLcode Curl_cf_सज्जता_सङ्कलय(संरचना Curl_easy *दत्त,
                           संरचना connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           पूर्णाङ्क ssl_बहुलकम्);

CURLcode Curl_cf_सज्जता_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                    संरचना Curl_easy *दत्त,
                                    uint8_t transport,
                                    पूर्णाङ्क ssl_बहुलकम्);

बाह्य संरचना Curl_cftype Curl_cft_सज्जता;

#यदि_अन्तः /* HEADER_CURL_CF_SETUP_H */
