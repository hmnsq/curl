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

शून्य Curl_ntlm_core_lm_resp(अचर अचिन्हित वर्ण *साँचोहरू,
                            अचर अचिन्हित वर्ण *plaintext,
                            अचिन्हित वर्ण *results);

CURLcode Curl_ntlm_core_mk_lm_hash(अचर वर्ण *password,
                                   अचिन्हित वर्ण *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(अचर वर्ण *password,
                                   अचिन्हित वर्ण *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(अचर अचिन्हित वर्ण *साँचो, अचिन्हित पूर्णाङ्क keylen,
                       अचर अचिन्हित वर्ण *data, अचिन्हित पूर्णाङ्क datalen,
                       अचिन्हित वर्ण *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(अचर वर्ण *user, माप_प्रकार userlen,
                                       अचर वर्ण *domain, माप_प्रकार domlen,
                                       अचिन्हित वर्ण *ntlmhash,
                                       अचिन्हित वर्ण *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(अचर अचिन्हित वर्ण *ntlmv2hash,
                                       अचर अचिन्हित वर्ण *challenge_client,
                                       अचर संरचना ntlmdata *ntlm,
                                       अचिन्हित वर्ण **ntresp,
                                       अचिन्हित पूर्णाङ्क *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(अचर अचिन्हित वर्ण *ntlmv2hash,
                                     अचर अचिन्हित वर्ण *challenge_client,
                                     अचर अचिन्हित वर्ण *challenge_server,
                                     अचिन्हित वर्ण *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
