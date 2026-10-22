#اگر_تعریف_نشده سرآیند_CURL_NTLM_CORE_H
#تعریف_کن سرآیند_CURL_NTLM_CORE_H
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

#اگر_تعریف_شده USE_CURL_NTLM_CORE

بنية ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#تعریف_کن SHORTPAIR(x)   ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff))
#تعریف_کن LONGQUARTET(x) ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff)), \
  ((صحيح)(((x) >> 16) & 0xff)), ((صحيح)(((x) >> 24) & 0xff))

فراغ Curl_ntlm_core_lm_resp(ثابت بیعلامت حرف *کلیدها,
                            ثابت بیعلامت حرف *plaintext,
                            بیعلامت حرف *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بیعلامت حرف *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بیعلامت حرف *ntbuffer /* 21 bytes */);

#اگر_تعریف_نشده USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ثابت بیعلامت حرف *کلید, بیعلامت صحيح keylen,
                       ثابت بیعلامت حرف *داده, بیعلامت صحيح datalen,
                       بیعلامت حرف *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بیعلامت حرف *ntlmhash,
                                       بیعلامت حرف *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بیعلامت حرف *ntlmv2hash,
                                       ثابت بیعلامت حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بیعلامت حرف **ntresp,
                                       بیعلامت صحيح *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بیعلامت حرف *ntlmv2hash,
                                     ثابت بیعلامت حرف *challenge_client,
                                     ثابت بیعلامت حرف *challenge_server,
                                     بیعلامت حرف *lmresp);

#پایان_اگر /* !USE_WINDOWS_SSPI */

#پایان_اگر /* USE_CURL_NTLM_CORE */

#پایان_اگر /* HEADER_CURL_NTLM_CORE_H */
