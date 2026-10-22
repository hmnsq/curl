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
#অন্তর্ভাব "curl_setup.h"

কাঠামো Curl_dns_entry;
কাঠামো ip_quadruple;
কাঠামো Curl_peer;
কাঠামো Curl_str;

CURLcode Curl_cf_setup_add(কাঠামো Curl_easy *তথ্য,
                           কাঠামো connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           পূর্ণসংখ্যা ssl_mode);

CURLcode Curl_cf_setup_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                    কাঠামো Curl_easy *তথ্য,
                                    uint8_t transport,
                                    পূর্ণসংখ্যা ssl_mode);

বাহ্যিক কাঠামো Curl_cftype Curl_cft_setup;

#endif /* HEADER_CURL_CF_SETUP_H */
