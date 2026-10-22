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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

ସଂରଚନା ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((x) & 0xff)), ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((x) & 0xff)), ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(((x) >> 8) & 0xff)), \
  ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(((x) >> 16) & 0xff)), ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(((x) >> 24) & 0xff))

ଶୂନ୍ୟ Curl_ntlm_core_lm_resp(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବିଗୁଡ଼ିକ,
                            ଅଚଳ unsigned ଅକ୍ଷର *plaintext,
                            unsigned ଅକ୍ଷର *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ଅଚଳ ଅକ୍ଷର *password,
                                   unsigned ଅକ୍ଷର *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ଅଚଳ ଅକ୍ଷର *password,
                                   unsigned ଅକ୍ଷର *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylen,
                       ଅଚଳ unsigned ଅକ୍ଷର *data, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା datalen,
                       unsigned ଅକ୍ଷର *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ଅଚଳ ଅକ୍ଷର *user, size_t userlen,
                                       ଅଚଳ ଅକ୍ଷର *domain, size_t domlen,
                                       unsigned ଅକ୍ଷର *ntlmhash,
                                       unsigned ଅକ୍ଷର *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ଅଚଳ unsigned ଅକ୍ଷର *ntlmv2hash,
                                       ଅଚଳ unsigned ଅକ୍ଷର *challenge_client,
                                       ଅଚଳ ସଂରଚନା ntlmdata *ntlm,
                                       unsigned ଅକ୍ଷର **ntresp,
                                       unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ଅଚଳ unsigned ଅକ୍ଷର *ntlmv2hash,
                                     ଅଚଳ unsigned ଅକ୍ଷର *challenge_client,
                                     ଅଚଳ unsigned ଅକ୍ଷର *challenge_server,
                                     unsigned ଅକ୍ଷର *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
