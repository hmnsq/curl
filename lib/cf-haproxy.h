#اگر_غیر_تعریف_شدہ سرخی_CURL_CF_HAPROXY_H
#تعریف_کرو سرخی_CURL_CF_HAPROXY_H
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

#شامل "urldata.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

CURLcode Curl_cf_haproxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                      ساخت Curl_easy *ڈیٹا);

خارجی ساخت Curl_cftype Curl_cft_haproxy;

#اگر_ختم /* !CURL_DISABLE_PROXY */

#اگر_ختم /* HEADER_CURL_CF_HAPROXY_H */
