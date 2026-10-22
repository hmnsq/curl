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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

ഘടന ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#define SHORTPAIR(x)   ((പൂർണ്ണസംഖ്യ)((x) & 0xff)), ((പൂർണ്ണസംഖ്യ)(((x) >> 8) & 0xff))
#define LONGQUARTET(x) ((പൂർണ്ണസംഖ്യ)((x) & 0xff)), ((പൂർണ്ണസംഖ്യ)(((x) >> 8) & 0xff)), \
  ((പൂർണ്ണസംഖ്യ)(((x) >> 16) & 0xff)), ((പൂർണ്ണസംഖ്യ)(((x) >> 24) & 0xff))

ശൂന്യം Curl_ntlm_core_lm_resp(സ്ഥിരം unsigned അക്ഷരം *താക്കോലുകൾ,
                            സ്ഥിരം unsigned അക്ഷരം *plaintext,
                            unsigned അക്ഷരം *results);

CURLcode Curl_ntlm_core_mk_lm_hash(സ്ഥിരം അക്ഷരം *password,
                                   unsigned അക്ഷരം *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(സ്ഥിരം അക്ഷരം *password,
                                   unsigned അക്ഷരം *ntbuffer /* 21 bytes */);

#ifndef USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ, unsigned പൂർണ്ണസംഖ്യ keylen,
                       സ്ഥിരം unsigned അക്ഷരം *data, unsigned പൂർണ്ണസംഖ്യ datalen,
                       unsigned അക്ഷരം *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(സ്ഥിരം അക്ഷരം *user, size_t userlen,
                                       സ്ഥിരം അക്ഷരം *domain, size_t domlen,
                                       unsigned അക്ഷരം *ntlmhash,
                                       unsigned അക്ഷരം *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(സ്ഥിരം unsigned അക്ഷരം *ntlmv2hash,
                                       സ്ഥിരം unsigned അക്ഷരം *challenge_client,
                                       സ്ഥിരം ഘടന ntlmdata *ntlm,
                                       unsigned അക്ഷരം **ntresp,
                                       unsigned പൂർണ്ണസംഖ്യ *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(സ്ഥിരം unsigned അക്ഷരം *ntlmv2hash,
                                     സ്ഥിരം unsigned അക്ഷരം *challenge_client,
                                     സ്ഥിരം unsigned അക്ഷരം *challenge_server,
                                     unsigned അക്ഷരം *lmresp);

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */

#endif /* HEADER_CURL_NTLM_CORE_H */
