#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_HTTP_DIGEST_H
#وصف_ڪريو سرو_CURL_HTTP_DIGEST_H
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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

/* this is for digest header input */
CURLcode Curl_داخلا_digest(بنية Curl_easy *معلومات,
                           bool proxy, ثابت حرف *سرو);

/* this is for creating digest header output */
CURLcode Curl_output_digest(بنية Curl_easy *معلومات,
                            bool proxy,
                            ثابت بنانشان حرف *request,
                            ثابت بنانشان حرف *uripath);

فراغ Curl_http_auth_cleanup_digest(بنية Curl_easy *معلومات);

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && !CURL_DISABLE_DIGEST_AUTH */

#جيڪڏهن_پورو /* HEADER_CURL_HTTP_DIGEST_H */
