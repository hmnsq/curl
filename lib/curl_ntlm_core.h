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

কাঠামো ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((পূর্ণসংখ্যা)((x) & 0xff)), ((পূর্ণসংখ্যা)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((পূর্ণসংখ্যা)((x) & 0xff)), ((পূর্ণসংখ্যা)(((x) >> 8) & 0xff)), \
  ((পূর্ণসংখ্যা)(((x) >> 16) & 0xff)), ((পূর্ণসংখ্যা)(((x) >> 24) & 0xff))

শূন্য Curl_ntlm_core_lm_resp(ধ্রুবক unsigned অক্ষর *চাবিগুলি,
                            ধ্রুবক unsigned অক্ষর *plaintext,
                            unsigned অক্ষর *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ধ্রুবক অক্ষর *password,
                                   unsigned অক্ষর *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ধ্রুবক অক্ষর *password,
                                   unsigned অক্ষর *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ধ্রুবক unsigned অক্ষর *চাবি, unsigned পূর্ণসংখ্যা keylen,
                       ধ্রুবক unsigned অক্ষর *তথ্য, unsigned পূর্ণসংখ্যা datalen,
                       unsigned অক্ষর *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ধ্রুবক অক্ষর *user, size_t userlen,
                                       ধ্রুবক অক্ষর *domain, size_t domlen,
                                       unsigned অক্ষর *ntlmhash,
                                       unsigned অক্ষর *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ধ্রুবক unsigned অক্ষর *ntlmv2hash,
                                       ধ্রুবক unsigned অক্ষর *challenge_client,
                                       ধ্রুবক কাঠামো ntlmdata *ntlm,
                                       unsigned অক্ষর **ntresp,
                                       unsigned পূর্ণসংখ্যা *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ধ্রুবক unsigned অক্ষর *ntlmv2hash,
                                     ধ্রুবক unsigned অক্ষর *challenge_client,
                                     ধ্রুবক unsigned অক্ষর *challenge_server,
                                     unsigned অক্ষর *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
