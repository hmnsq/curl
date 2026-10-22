#ifndef HEADER_CURL_MD5_H
#define HEADER_CURL_MD5_H
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

#എങ്കിൽ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#അന്തര്ഭാവ "curl_hmac.h"

#define MD5_DIGEST_LEN  16

തരനിർവചനം CURLcode (*Curl_MD5_init_func)(ശൂന്യം *context);
തരനിർവചനം ശൂന്യം (*Curl_MD5_update_func)(ശൂന്യം *context,
                                     സ്ഥിരം unsigned അക്ഷരം *data,
                                     unsigned പൂർണ്ണസംഖ്യ len);
തരനിർവചനം ശൂന്യം (*Curl_MD5_final_func)(unsigned അക്ഷരം *result, ശൂന്യം *context);

ഘടന MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned പൂർണ്ണസംഖ്യ md5_ctxtsize;            /* Context structure size */
  unsigned പൂർണ്ണസംഖ്യ md5_resultlen;           /* Result length (bytes) */
};

ഘടന MD5_context {
  സ്ഥിരം ഘടന MD5_params *md5_hash;  /* Hash function definition */
  ശൂന്യം *md5_hashctx;                  /* Hash function context */
};

ബാഹ്യ സ്ഥിരം ഘടന MD5_params Curl_DIGEST_MD5;
ബാഹ്യ സ്ഥിരം ഘടന HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned അക്ഷരം *output, സ്ഥിരം unsigned അക്ഷരം *നിവേശം,
                    size_t len);

ഘടന MD5_context *Curl_MD5_init(സ്ഥിരം ഘടന MD5_params *md5params);
CURLcode Curl_MD5_update(ഘടന MD5_context *context,
                         സ്ഥിരം unsigned അക്ഷരം *നിവേശം,
                         unsigned പൂർണ്ണസംഖ്യ len);
CURLcode Curl_MD5_final(ഘടന MD5_context *context, unsigned അക്ഷരം *result);

#endif

#endif /* HEADER_CURL_MD5_H */
