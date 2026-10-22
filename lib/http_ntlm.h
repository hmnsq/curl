#که_نه_تعریف_شوی سرلیک_CURL_HTTP_NTLM_H
#تعریف_کړه سرلیک_CURL_HTTP_NTLM_H
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

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/* this is for NTLM header input */
CURLcode Curl_ننوت_ntlm(بنية Curl_easy *معلومات, bool proxy,
                         ثابت حرف *سرلیک);

/* this is for creating NTLM header output */
CURLcode Curl_output_ntlm(بنية Curl_easy *معلومات, bool proxy);

#د_که_پای

#د_که_پای /* HEADER_CURL_HTTP_NTLM_H */
