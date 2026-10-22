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
#شامل "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

ساختار ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((صحیح)((x) & 0xff)), ((صحیح)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((صحیح)((x) & 0xff)), ((صحیح)(((x) >> 8) & 0xff)), \
  ((صحیح)(((x) >> 16) & 0xff)), ((صحیح)(((x) >> 24) & 0xff))

تهی Curl_ntlm_core_lm_resp(ثابت unsigned نویسه *کلیدها,
                            ثابت unsigned نویسه *plaintext,
                            unsigned نویسه *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ثابت نویسه *password,
                                   unsigned نویسه *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ثابت نویسه *password,
                                   unsigned نویسه *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ثابت unsigned نویسه *کلید, unsigned صحیح keylen,
                       ثابت unsigned نویسه *data, unsigned صحیح datalen,
                       unsigned نویسه *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت نویسه *user, size_t userlen,
                                       ثابت نویسه *domain, size_t domlen,
                                       unsigned نویسه *ntlmhash,
                                       unsigned نویسه *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت unsigned نویسه *ntlmv2hash,
                                       ثابت unsigned نویسه *challenge_client,
                                       ثابت ساختار ntlmdata *ntlm,
                                       unsigned نویسه **ntresp,
                                       unsigned صحیح *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت unsigned نویسه *ntlmv2hash,
                                     ثابت unsigned نویسه *challenge_client,
                                     ثابت unsigned نویسه *challenge_server,
                                     unsigned نویسه *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
