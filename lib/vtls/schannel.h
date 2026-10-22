#अगर_अघोषित HEADER_CURL_SCHANNEL_H
#घोषणा HEADER_CURL_SCHANNEL_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>, et al.
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

#अगर_घोषित USE_SCHANNEL

#समावेश <schannel.h>

#समावेश "curl_sspi.h"
#समावेश "cfilters.h"
#समावेश "urldata.h"

बाह्य स्थिर काष्ठा Curl_ssl Curl_ssl_schannel;

CURLcode Curl_verify_host(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य);

CURLcode Curl_verify_certificate(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य);

#पूर्ण_अगर /* USE_SCHANNEL */
#पूर्ण_अगर /* HEADER_CURL_SCHANNEL_H */
