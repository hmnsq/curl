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
#ضمن "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

بنية ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff)), \
  ((صحيح)(((x) >> 16) & 0xff)), ((صحيح)(((x) >> 24) & 0xff))

فراغ Curl_ntlm_core_lm_resp(ثابت unsigned حرف *ئاچقۇچلار,
                            ثابت unsigned حرف *plaintext,
                            unsigned حرف *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   unsigned حرف *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   unsigned حرف *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ثابت unsigned حرف *ئاچقۇچ, unsigned صحيح keylen,
                       ثابت unsigned حرف *data, unsigned صحيح datalen,
                       unsigned حرف *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       unsigned حرف *ntlmhash,
                                       unsigned حرف *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت unsigned حرف *ntlmv2hash,
                                       ثابت unsigned حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       unsigned حرف **ntresp,
                                       unsigned صحيح *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت unsigned حرف *ntlmv2hash,
                                     ثابت unsigned حرف *challenge_client,
                                     ثابت unsigned حرف *challenge_server,
                                     unsigned حرف *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
