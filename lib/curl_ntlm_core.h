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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

संरचना ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((पूर्णाङ्क)((x) & 0xff)), ((पूर्णाङ्क)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((पूर्णाङ्क)((x) & 0xff)), ((पूर्णाङ्क)(((x) >> 8) & 0xff)), \
  ((पूर्णाङ्क)(((x) >> 16) & 0xff)), ((पूर्णाङ्क)(((x) >> 24) & 0xff))

शून्य Curl_ntlm_core_lm_resp(ध्रुव unsigned वर्ण *किल्ल्या,
                            ध्रुव unsigned वर्ण *plaintext,
                            unsigned वर्ण *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ध्रुव वर्ण *password,
                                   unsigned वर्ण *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ध्रुव वर्ण *password,
                                   unsigned वर्ण *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ध्रुव unsigned वर्ण *किल्ली, unsigned पूर्णाङ्क keylen,
                       ध्रुव unsigned वर्ण *data, unsigned पूर्णाङ्क datalen,
                       unsigned वर्ण *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ध्रुव वर्ण *user, size_t userlen,
                                       ध्रुव वर्ण *domain, size_t domlen,
                                       unsigned वर्ण *ntlmhash,
                                       unsigned वर्ण *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ध्रुव unsigned वर्ण *ntlmv2hash,
                                       ध्रुव unsigned वर्ण *challenge_client,
                                       ध्रुव संरचना ntlmdata *ntlm,
                                       unsigned वर्ण **ntresp,
                                       unsigned पूर्णाङ्क *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ध्रुव unsigned वर्ण *ntlmv2hash,
                                     ध्रुव unsigned वर्ण *challenge_client,
                                     ध्रुव unsigned वर्ण *challenge_server,
                                     unsigned वर्ण *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
