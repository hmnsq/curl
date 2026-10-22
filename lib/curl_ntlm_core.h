#که_نه_تعریف_شوی سرلیک_CURL_NTLM_CORE_H
#تعریف_کړه سرلیک_CURL_NTLM_CORE_H
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

#که_تعریف_شوی USE_CURL_NTLM_CORE

بنية ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#تعریف_کړه SHORTPAIR(x)   ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff))
#تعریف_کړه LONGQUARTET(x) ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff)), \
  ((صحيح)(((x) >> 16) & 0xff)), ((صحيح)(((x) >> 24) & 0xff))

فراغ Curl_ntlm_core_lm_resp(ثابت بینښه حرف *کلیګانې,
                            ثابت بینښه حرف *plaintext,
                            بینښه حرف *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بینښه حرف *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بینښه حرف *ntbuffer /* 21 bytes */);

#که_نه_تعریف_شوی USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ثابت بینښه حرف *کلی, بینښه صحيح keylen,
                       ثابت بینښه حرف *معلومات, بینښه صحيح datalen,
                       بینښه حرف *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بینښه حرف *ntlmhash,
                                       بینښه حرف *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بینښه حرف *ntlmv2hash,
                                       ثابت بینښه حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بینښه حرف **ntresp,
                                       بینښه صحيح *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بینښه حرف *ntlmv2hash,
                                     ثابت بینښه حرف *challenge_client,
                                     ثابت بینښه حرف *challenge_server,
                                     بینښه حرف *lmresp);

#د_که_پای /* !USE_WINDOWS_SSPI */

#د_که_پای /* USE_CURL_NTLM_CORE */

#د_که_پای /* HEADER_CURL_NTLM_CORE_H */
