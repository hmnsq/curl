#جے_بنا_تعریف سرخی_CURL_HTTP_DIGEST_H
#تعریف_کرو سرخی_CURL_HTTP_DIGEST_H
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
CURLcode Curl_داخلہ_digest(بنية Curl_easy *انکڑے,
                           bool proxy, ثابت حرف *سرخی);

/* this is for creating digest header output */
CURLcode Curl_output_digest(بنية Curl_easy *انکڑے,
                            bool proxy,
                            ثابت بنانشانی حرف *request,
                            ثابت بنانشانی حرف *uripath);

فراغ Curl_http_auth_cleanup_digest(بنية Curl_easy *انکڑے);

#جے_انت /* !CURL_DISABLE_HTTP && !CURL_DISABLE_DIGEST_AUTH */

#جے_انت /* HEADER_CURL_HTTP_DIGEST_H */
