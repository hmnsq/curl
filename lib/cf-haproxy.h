#اگر_تعریف_نشده سرآیند_CURL_CF_HAPROXY_H
#تعریف_کن سرآیند_CURL_CF_HAPROXY_H
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

#ضمن "urldata.h"

#اگر_تعریف_نشده CURL_DISABLE_PROXY

CURLcode Curl_cf_haproxy_درج_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *داده);

خارجي بنية Curl_cftype Curl_cft_haproxy;

#پایان_اگر /* !CURL_DISABLE_PROXY */

#پایان_اگر /* HEADER_CURL_CF_HAPROXY_H */
