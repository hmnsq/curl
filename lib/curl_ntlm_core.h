#اگر_غیر_تعریف_شدہ سرخی_CURL_NTLM_CORE_H
#تعریف_کرو سرخی_CURL_NTLM_CORE_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_CURL_NTLM_CORE

ساخت ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#تعریف_کرو SHORTPAIR(x)   ((صحیح)((x) & 0xff)), ((صحیح)(((x) >> 8) & 0xff))
#تعریف_کرو LONGQUARTET(x) ((صحیح)((x) & 0xff)), ((صحیح)(((x) >> 8) & 0xff)), \
  ((صحیح)(((x) >> 16) & 0xff)), ((صحیح)(((x) >> 24) & 0xff))

خالی Curl_ntlm_core_lm_resp(ثابت غیرعلامتی حرف *کلیدیں,
                            ثابت غیرعلامتی حرف *plaintext,
                            غیرعلامتی حرف *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   غیرعلامتی حرف *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   غیرعلامتی حرف *ntbuffer /* 21 bytes */);

#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ثابت غیرعلامتی حرف *کلید, غیرعلامتی صحیح keylen,
                       ثابت غیرعلامتی حرف *ڈیٹا, غیرعلامتی صحیح datalen,
                       غیرعلامتی حرف *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       غیرعلامتی حرف *ntlmhash,
                                       غیرعلامتی حرف *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت غیرعلامتی حرف *ntlmv2hash,
                                       ثابت غیرعلامتی حرف *challenge_client,
                                       ثابت ساخت ntlmdata *ntlm,
                                       غیرعلامتی حرف **ntresp,
                                       غیرعلامتی صحیح *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت غیرعلامتی حرف *ntlmv2hash,
                                     ثابت غیرعلامتی حرف *challenge_client,
                                     ثابت غیرعلامتی حرف *challenge_server,
                                     غیرعلامتی حرف *lmresp);

#اگر_ختم /* !USE_WINDOWS_SSPI */

#اگر_ختم /* USE_CURL_NTLM_CORE */

#اگر_ختم /* HEADER_CURL_NTLM_CORE_H */
