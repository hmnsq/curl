#ifndef HEADER_CURL_HTTP_DIGEST_H
#define HEADER_CURL_HTTP_DIGEST_H
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

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)

/* this is for digest header input */
CURLcode Curl_প্রবেশ_digest(কাঠামো Curl_easy *তথ্য,
                           bool proxy, ধ্রুবক অক্ষর *header);

/* this is for creating digest header output */
CURLcode Curl_output_digest(কাঠামো Curl_easy *তথ্য,
                            bool proxy,
                            ধ্রুবক unsigned অক্ষর *request,
                            ধ্রুবক unsigned অক্ষর *uripath);

শূন্য Curl_http_auth_cleanup_digest(কাঠামো Curl_easy *তথ্য);

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_DIGEST_AUTH */

#endif /* HEADER_CURL_HTTP_DIGEST_H */
