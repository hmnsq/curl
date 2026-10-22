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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

ಸಂರಚನಾ ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((ಪೂರ್ಣಾಙ್ಕ)((x) & 0xff)), ((ಪೂರ್ಣಾಙ್ಕ)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((ಪೂರ್ಣಾಙ್ಕ)((x) & 0xff)), ((ಪೂರ್ಣಾಙ್ಕ)(((x) >> 8) & 0xff)), \
  ((ಪೂರ್ಣಾಙ್ಕ)(((x) >> 16) & 0xff)), ((ಪೂರ್ಣಾಙ್ಕ)(((x) >> 24) & 0xff))

ಶೂನ್ಯ Curl_ntlm_core_lm_resp(ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿಗಳು,
                            ಧ್ರುವ unsigned ವರ್ಣ *plaintext,
                            unsigned ವರ್ಣ *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ಧ್ರುವ ವರ್ಣ *password,
                                   unsigned ವರ್ಣ *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ಧ್ರುವ ವರ್ಣ *password,
                                   unsigned ವರ್ಣ *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ, unsigned ಪೂರ್ಣಾಙ್ಕ keylen,
                       ಧ್ರುವ unsigned ವರ್ಣ *data, unsigned ಪೂರ್ಣಾಙ್ಕ datalen,
                       unsigned ವರ್ಣ *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ಧ್ರುವ ವರ್ಣ *user, size_t userlen,
                                       ಧ್ರುವ ವರ್ಣ *domain, size_t domlen,
                                       unsigned ವರ್ಣ *ntlmhash,
                                       unsigned ವರ್ಣ *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ಧ್ರುವ unsigned ವರ್ಣ *ntlmv2hash,
                                       ಧ್ರುವ unsigned ವರ್ಣ *challenge_client,
                                       ಧ್ರುವ ಸಂರಚನಾ ntlmdata *ntlm,
                                       unsigned ವರ್ಣ **ntresp,
                                       unsigned ಪೂರ್ಣಾಙ್ಕ *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ಧ್ರುವ unsigned ವರ್ಣ *ntlmv2hash,
                                     ಧ್ರುವ unsigned ವರ್ಣ *challenge_client,
                                     ಧ್ರುವ unsigned ವರ್ಣ *challenge_server,
                                     unsigned ವರ್ಣ *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
