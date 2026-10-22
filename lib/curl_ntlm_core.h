#यदि_अपरिभाषितम् शीर्षकम्_CURL_NTLM_CORE_H
#परिभाषय शीर्षकम्_CURL_NTLM_CORE_H
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

#यदि_परिभाषितम् USE_CURL_NTLM_CORE

संरचना ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#परिभाषय SHORTPAIR(x)   ((पूर्णाङ्क)((x) & 0xff)), ((पूर्णाङ्क)(((x) >> 8) & 0xff))
#परिभाषय LONGQUARTET(x) ((पूर्णाङ्क)((x) & 0xff)), ((पूर्णाङ्क)(((x) >> 8) & 0xff)), \
  ((पूर्णाङ्क)(((x) >> 16) & 0xff)), ((पूर्णाङ्क)(((x) >> 24) & 0xff))

शून्य Curl_ntlm_core_lm_resp(ध्रुव अचिह्नित वर्ण *कुञ्चिकाः,
                            ध्रुव अचिह्नित वर्ण *plaintext,
                            अचिह्नित वर्ण *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ध्रुव वर्ण *password,
                                   अचिह्नित वर्ण *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ध्रुव वर्ण *password,
                                   अचिह्नित वर्ण *ntbuffer /* 21 bytes */);

#यदि_अपरिभाषितम् USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ध्रुव अचिह्नित वर्ण *कुञ्चिका, अचिह्नित पूर्णाङ्क keylen,
                       ध्रुव अचिह्नित वर्ण *दत्त, अचिह्नित पूर्णाङ्क datalen,
                       अचिह्नित वर्ण *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ध्रुव वर्ण *user, size_t userlen,
                                       ध्रुव वर्ण *domain, size_t domlen,
                                       अचिह्नित वर्ण *ntlmhash,
                                       अचिह्नित वर्ण *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ध्रुव अचिह्नित वर्ण *ntlmv2hash,
                                       ध्रुव अचिह्नित वर्ण *challenge_client,
                                       ध्रुव संरचना ntlmdata *ntlm,
                                       अचिह्नित वर्ण **ntresp,
                                       अचिह्नित पूर्णाङ्क *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ध्रुव अचिह्नित वर्ण *ntlmv2hash,
                                     ध्रुव अचिह्नित वर्ण *challenge_client,
                                     ध्रुव अचिह्नित वर्ण *challenge_server,
                                     अचिह्नित वर्ण *lmresp);

#यदि_अन्तः /* !USE_WINDOWS_SSPI */

#यदि_अन्तः /* USE_CURL_NTLM_CORE */

#यदि_अन्तः /* HEADER_CURL_NTLM_CORE_H */
