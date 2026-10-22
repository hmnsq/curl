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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

સંરચના ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((પૂર્ણાંક)((x) & 0xff)), ((પૂર્ણાંક)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((પૂર્ણાંક)((x) & 0xff)), ((પૂર્ણાંક)(((x) >> 8) & 0xff)), \
  ((પૂર્ણાંક)(((x) >> 16) & 0xff)), ((પૂર્ણાંક)(((x) >> 24) & 0xff))

શૂન્ય Curl_ntlm_core_lm_resp(અચલ unsigned અક્ષર *ચાવીઓ,
                            અચલ unsigned અક્ષર *plaintext,
                            unsigned અક્ષર *results);

CURLcode Curl_ntlm_core_mk_lm_hash(અચલ અક્ષર *password,
                                   unsigned અક્ષર *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(અચલ અક્ષર *password,
                                   unsigned અક્ષર *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(અચલ unsigned અક્ષર *ચાવી, unsigned પૂર્ણાંક keylen,
                       અચલ unsigned અક્ષર *data, unsigned પૂર્ણાંક datalen,
                       unsigned અક્ષર *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(અચલ અક્ષર *user, size_t userlen,
                                       અચલ અક્ષર *domain, size_t domlen,
                                       unsigned અક્ષર *ntlmhash,
                                       unsigned અક્ષર *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(અચલ unsigned અક્ષર *ntlmv2hash,
                                       અચલ unsigned અક્ષર *challenge_client,
                                       અચલ સંરચના ntlmdata *ntlm,
                                       unsigned અક્ષર **ntresp,
                                       unsigned પૂર્ણાંક *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(અચલ unsigned અક્ષર *ntlmv2hash,
                                     અચલ unsigned અક્ષર *challenge_client,
                                     અચલ unsigned અક્ષર *challenge_server,
                                     unsigned અક્ષર *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
