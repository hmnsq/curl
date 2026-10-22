#अगर_अघोषित HEADER_CURL_HTTP_NEGOTIATE_H
#घोषणा HEADER_CURL_HTTP_NEGOTIATE_H
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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)

/* this is for Negotiate header input */
CURLcode Curl_निवेश_negotiate(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn,
                              bool proxy, स्थिर अक्षर *header);

/* this is for creating Negotiate header output */
CURLcode Curl_output_negotiate(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn, bool proxy);

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_HTTP_NEGOTIATE_H */
