#اگر_غیر_تعریف_شدہ سرخی_CURL_HTTP_NEGOTIATE_H
#تعریف_کرو سرخی_CURL_HTTP_NEGOTIATE_H
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

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

/* this is for Negotiate header input */
CURLcode Curl_داخلہ_negotiate(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                              bool proxy, ثابت حرف *سرخی);

/* this is for creating Negotiate header output */
CURLcode Curl_output_negotiate(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn, bool proxy);

#اگر_ختم

#اگر_ختم /* HEADER_CURL_HTTP_NEGOTIATE_H */
