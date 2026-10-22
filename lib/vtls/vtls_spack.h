#अगर_अघोषित HEADER_CURL_VTLS_SPACK_H
#घोषणा HEADER_CURL_VTLS_SPACK_H
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

#अगर defined(USE_SSL) && defined(USE_SSLS_EXPORT)

काष्ठा dynbuf;
काष्ठा Curl_ssl_session;

CURLcode Curl_ssl_session_pack(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_ssl_session *s,
                               काष्ठा dynbuf *buf);

CURLcode Curl_ssl_session_unpack(काष्ठा Curl_easy *तथ्य,
                                 स्थिर व्योम *bufv, माप_प्रकार buflen,
                                 काष्ठा Curl_ssl_session **ps);

#पूर्ण_अगर /* USE_SSL && USE_SSLS_EXPORT */

#पूर्ण_अगर /* HEADER_CURL_VTLS_SPACK_H */
