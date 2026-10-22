#جے_بنا_تعریف سرخی_CURL_NTLM_CORE_H
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
#ضمن "curl_setup.h"

#جے_تعریف_شدہ USE_CURL_NTLM_CORE

بنية ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#تعریف_کرو SHORTPAIR(x)   ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff))
#تعریف_کرو LONGQUARTET(x) ((صحيح)((x) & 0xff)), ((صحيح)(((x) >> 8) & 0xff)), \
  ((صحيح)(((x) >> 16) & 0xff)), ((صحيح)(((x) >> 24) & 0xff))

فراغ Curl_ntlm_core_lm_resp(ثابت بنانشانی حرف *کلیدیں,
                            ثابت بنانشانی حرف *plaintext,
                            بنانشانی حرف *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بنانشانی حرف *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بنانشانی حرف *ntbuffer /* 21 bytes */);

#جے_بنا_تعریف USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ثابت بنانشانی حرف *کلید, بنانشانی صحيح keylen,
                       ثابت بنانشانی حرف *انکڑے, بنانشانی صحيح datalen,
                       بنانشانی حرف *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بنانشانی حرف *ntlmhash,
                                       بنانشانی حرف *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بنانشانی حرف *ntlmv2hash,
                                       ثابت بنانشانی حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بنانشانی حرف **ntresp,
                                       بنانشانی صحيح *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بنانشانی حرف *ntlmv2hash,
                                     ثابت بنانشانی حرف *challenge_client,
                                     ثابت بنانشانی حرف *challenge_server,
                                     بنانشانی حرف *lmresp);

#جے_انت /* !USE_WINDOWS_SSPI */

#جے_انت /* USE_CURL_NTLM_CORE */

#جے_انت /* HEADER_CURL_NTLM_CORE_H */
