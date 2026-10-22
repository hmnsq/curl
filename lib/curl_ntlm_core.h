#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_NTLM_CORE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_NTLM_CORE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_NTLM_CORE

ਬਣਤਰ ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHORTPAIR(x)   ((ਪੂਰਨ)((x) & 0xff)), ((ਪੂਰਨ)(((x) >> 8) & 0xff))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LONGQUARTET(x) ((ਪੂਰਨ)((x) & 0xff)), ((ਪੂਰਨ)(((x) >> 8) & 0xff)), \
  ((ਪੂਰਨ)(((x) >> 16) & 0xff)), ((ਪੂਰਨ)(((x) >> 24) & 0xff))

ਖਾਲੀ Curl_ntlm_core_lm_resp(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀਆਂ,
                            ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *plaintext,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ਅਚਲ ਅੱਖਰ *password,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ਅਚਲ ਅੱਖਰ *password,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntbuffer /* 21 bytes */);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keylen,
                       ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ datalen,
                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ਅਚਲ ਅੱਖਰ *user, size_t userlen,
                                       ਅਚਲ ਅੱਖਰ *domain, size_t domlen,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmhash,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmv2hash,
                                       ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *challenge_client,
                                       ਅਚਲ ਬਣਤਰ ntlmdata *ntlm,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **ntresp,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmv2hash,
                                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *challenge_client,
                                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *challenge_server,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *lmresp);

#ਜੇ_ਅੰਤ /* !USE_WINDOWS_SSPI */

#ਜੇ_ਅੰਤ /* USE_CURL_NTLM_CORE */

#ਜੇ_ਅੰਤ /* HEADER_CURL_NTLM_CORE_H */
