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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

கட்டமைப்பு ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((முழு)((x) & 0xff)), ((முழு)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((முழு)((x) & 0xff)), ((முழு)(((x) >> 8) & 0xff)), \
  ((முழு)(((x) >> 16) & 0xff)), ((முழு)(((x) >> 24) & 0xff))

வெறுமை Curl_ntlm_core_lm_resp(மாறிலி unsigned எழுத்து *சாவிகள்,
                            மாறிலி unsigned எழுத்து *plaintext,
                            unsigned எழுத்து *results);

CURLcode Curl_ntlm_core_mk_lm_hash(மாறிலி எழுத்து *password,
                                   unsigned எழுத்து *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(மாறிலி எழுத்து *password,
                                   unsigned எழுத்து *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(மாறிலி unsigned எழுத்து *சாவி, unsigned முழு keylen,
                       மாறிலி unsigned எழுத்து *data, unsigned முழு datalen,
                       unsigned எழுத்து *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(மாறிலி எழுத்து *user, size_t userlen,
                                       மாறிலி எழுத்து *domain, size_t domlen,
                                       unsigned எழுத்து *ntlmhash,
                                       unsigned எழுத்து *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(மாறிலி unsigned எழுத்து *ntlmv2hash,
                                       மாறிலி unsigned எழுத்து *challenge_client,
                                       மாறிலி கட்டமைப்பு ntlmdata *ntlm,
                                       unsigned எழுத்து **ntresp,
                                       unsigned முழு *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(மாறிலி unsigned எழுத்து *ntlmv2hash,
                                     மாறிலி unsigned எழுத்து *challenge_client,
                                     மாறிலி unsigned எழுத்து *challenge_server,
                                     unsigned எழுத்து *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
