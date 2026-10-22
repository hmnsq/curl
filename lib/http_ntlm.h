#यदि_अपरिभाषितम् शीर्षकम्_CURL_HTTP_NTLM_H
#परिभाषय शीर्षकम्_CURL_HTTP_NTLM_H
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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)

/* this is for NTLM header input */
CURLcode Curl_निवेशः_ntlm(संरचना Curl_easy *दत्त, bool proxy,
                         ध्रुव वर्ण *शीर्षकम्);

/* this is for creating NTLM header output */
CURLcode Curl_output_ntlm(संरचना Curl_easy *दत्त, bool proxy);

#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_HTTP_NTLM_H */
