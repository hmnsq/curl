#ifndef HEADER_CURL_NTLM_CORE_H
#define HEADER_CURL_NTLM_CORE_H
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

#ifdef USE_CURL_NTLM_CORE

संरचना ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((पूर्णांक)((x) & 0xff)), ((पूर्णांक)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((पूर्णांक)((x) & 0xff)), ((पूर्णांक)(((x) >> 8) & 0xff)), \
  ((पूर्णांक)(((x) >> 16) & 0xff)), ((पूर्णांक)(((x) >> 24) & 0xff))

शून्य Curl_ntlm_core_lm_resp(स्थिरांक अचिन्हित अक्षर *किल्ल्या,
                            स्थिरांक अचिन्हित अक्षर *plaintext,
                            अचिन्हित अक्षर *results);

CURLcode Curl_ntlm_core_mk_lm_hash(स्थिरांक अक्षर *password,
                                   अचिन्हित अक्षर *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(स्थिरांक अक्षर *password,
                                   अचिन्हित अक्षर *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(स्थिरांक अचिन्हित अक्षर *किल्ली, अचिन्हित पूर्णांक keylen,
                       स्थिरांक अचिन्हित अक्षर *data, अचिन्हित पूर्णांक datalen,
                       अचिन्हित अक्षर *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(स्थिरांक अक्षर *user, माप_प्रकार userlen,
                                       स्थिरांक अक्षर *domain, माप_प्रकार domlen,
                                       अचिन्हित अक्षर *ntlmhash,
                                       अचिन्हित अक्षर *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(स्थिरांक अचिन्हित अक्षर *ntlmv2hash,
                                       स्थिरांक अचिन्हित अक्षर *challenge_client,
                                       स्थिरांक संरचना ntlmdata *ntlm,
                                       अचिन्हित अक्षर **ntresp,
                                       अचिन्हित पूर्णांक *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(स्थिरांक अचिन्हित अक्षर *ntlmv2hash,
                                     स्थिरांक अचिन्हित अक्षर *challenge_client,
                                     स्थिरांक अचिन्हित अक्षर *challenge_server,
                                     अचिन्हित अक्षर *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
