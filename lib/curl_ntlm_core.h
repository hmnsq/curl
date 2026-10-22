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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

সংরচনা ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((পূর্ণাঙ্ক)((x) & 0xff)), ((পূর্ণাঙ্ক)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((পূর্ণাঙ্ক)((x) & 0xff)), ((পূর্ণাঙ্ক)(((x) >> 8) & 0xff)), \
  ((পূর্ণাঙ্ক)(((x) >> 16) & 0xff)), ((পূর্ণাঙ্ক)(((x) >> 24) & 0xff))

শূন্য Curl_ntlm_core_lm_resp(ধ্রুব unsigned বর্ণ *চাবিগুলি,
                            ধ্রুব unsigned বর্ণ *plaintext,
                            unsigned বর্ণ *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ধ্রুব বর্ণ *password,
                                   unsigned বর্ণ *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ধ্রুব বর্ণ *password,
                                   unsigned বর্ণ *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ধ্রুব unsigned বর্ণ *চাবি, unsigned পূর্ণাঙ্ক keylen,
                       ধ্রুব unsigned বর্ণ *data, unsigned পূর্ণাঙ্ক datalen,
                       unsigned বর্ণ *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ধ্রুব বর্ণ *user, size_t userlen,
                                       ধ্রুব বর্ণ *domain, size_t domlen,
                                       unsigned বর্ণ *ntlmhash,
                                       unsigned বর্ণ *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ধ্রুব unsigned বর্ণ *ntlmv2hash,
                                       ধ্রুব unsigned বর্ণ *challenge_client,
                                       ধ্রুব সংরচনা ntlmdata *ntlm,
                                       unsigned বর্ণ **ntresp,
                                       unsigned পূর্ণাঙ্ক *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ধ্রুব unsigned বর্ণ *ntlmv2hash,
                                     ধ্রুব unsigned বর্ণ *challenge_client,
                                     ধ্রুব unsigned বর্ণ *challenge_server,
                                     unsigned বর্ণ *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
