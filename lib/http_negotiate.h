#اگر_تعریف_نشده سرآیند_CURL_HTTP_NEGOTIATE_H
#تعریف_کن سرآیند_CURL_HTTP_NEGOTIATE_H
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

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

/* this is for Negotiate header input */
CURLcode Curl_درون_داد_negotiate(بنية Curl_easy *داده, بنية connectdata *conn,
                              bool proxy, ثابت حرف *سرآیند);

/* this is for creating Negotiate header output */
CURLcode Curl_output_negotiate(بنية Curl_easy *داده,
                               بنية connectdata *conn, bool proxy);

#پایان_اگر

#پایان_اگر /* HEADER_CURL_HTTP_NEGOTIATE_H */
