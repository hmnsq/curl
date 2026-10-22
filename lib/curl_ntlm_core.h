#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_NTLM_CORE_H
#وصف_ڪريو سرو_CURL_NTLM_CORE_H
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

#جيڪڏهن_وصف_ٿيل USE_CURL_NTLM_CORE

بنية ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#وصف_ڪريو SHORTPAIR(x)   ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff))
#وصف_ڪريو LONGQUARTET(x) ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff)), \
  ((صحيح)(((x) >> 16) & 0xff)), ((صحيح)(((x) >> 24) & 0xff))

فراغ Curl_ntlm_core_lm_resp(ثابت بنانشان حرف *ڪنجيون,
                            ثابت بنانشان حرف *plaintext,
                            بنانشان حرف *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بنانشان حرف *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بنانشان حرف *ntbuffer /* 21 bytes */);

#جيڪڏهن_وصف_نه_ٿيل USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ثابت بنانشان حرف *ڪنجي, بنانشان صحيح keylen,
                       ثابت بنانشان حرف *معلومات, بنانشان صحيح datalen,
                       بنانشان حرف *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بنانشان حرف *ntlmhash,
                                       بنانشان حرف *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بنانشان حرف *ntlmv2hash,
                                       ثابت بنانشان حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بنانشان حرف **ntresp,
                                       بنانشان صحيح *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بنانشان حرف *ntlmv2hash,
                                     ثابت بنانشان حرف *challenge_client,
                                     ثابت بنانشان حرف *challenge_server,
                                     بنانشان حرف *lmresp);

#جيڪڏهن_پورو /* !USE_WINDOWS_SSPI */

#جيڪڏهن_پورو /* USE_CURL_NTLM_CORE */

#جيڪڏهن_پورو /* HEADER_CURL_NTLM_CORE_H */
