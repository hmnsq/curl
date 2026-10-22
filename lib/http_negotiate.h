#ifndef HEADER_CURL_HTTP_NEGOTIATE_H
#define HEADER_CURL_HTTP_NEGOTIATE_H
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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

/* this is for Negotiate header input */
CURLcode Curl_നിവേശം_negotiate(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                              bool proxy, സ്ഥിരം അക്ഷരം *header);

/* this is for creating Negotiate header output */
CURLcode Curl_output_negotiate(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn, bool proxy);

#endif

#endif /* HEADER_CURL_HTTP_NEGOTIATE_H */
