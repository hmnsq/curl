#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HTTP_NEGOTIATE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HTTP_NEGOTIATE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

/* this is for Negotiate header input */
CURLcode Curl_ਨਿਵੇਸ਼_negotiate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                              bool proxy, ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ);

/* this is for creating Negotiate header output */
CURLcode Curl_output_negotiate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn, bool proxy);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_HTTP_NEGOTIATE_H */
