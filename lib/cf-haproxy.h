#ifndef HEADER_CURL_CF_HAPROXY_H
#define HEADER_CURL_CF_HAPROXY_H
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

#ifndef CURL_DISABLE_PROXY

CURLcode Curl_cf_haproxy_قىستۇرۇش_after(بنية Curl_cfilter *cf_at,
                                      بنية Curl_easy *data);

خارجي بنية Curl_cftype Curl_cft_haproxy;

#endif /* !CURL_DISABLE_PROXY */

#endif /* HEADER_CURL_CF_HAPROXY_H */
