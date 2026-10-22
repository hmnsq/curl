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

#ᰀᰧ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#include "curl_hmac.h"

#define MD5_DIGEST_LEN  16

typedef CURLcode (*Curl_MD5_init_func)(void *context);
typedef void (*Curl_MD5_update_func)(void *context,
                                     const unsigned var_na *data,
                                     unsigned puur_naa_nka len);
typedef void (*Curl_MD5_final_func)(unsigned var_na *result, void *context);

sa_nracanaa MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned puur_naa_nka md5_ctxtsize;            /* Context structure size */
  unsigned puur_naa_nka md5_resultlen;           /* Result length (bytes) */
};

sa_nracanaa MD5_context {
  const sa_nracanaa MD5_params *md5_hash;  /* Hash function definition */
  void *md5_hashctx;                  /* Hash function context */
};

extern const sa_nracanaa MD5_params Curl_DIGEST_MD5;
extern const sa_nracanaa HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned var_na *output, const unsigned var_na *input,
                    size_t len);

sa_nracanaa MD5_context *Curl_MD5_init(const sa_nracanaa MD5_params *md5params);
CURLcode Curl_MD5_update(sa_nracanaa MD5_context *context,
                         const unsigned var_na *input,
                         unsigned puur_naa_nka len);
CURLcode Curl_MD5_final(sa_nracanaa MD5_context *context, unsigned var_na *result);

#endif

#endif /* HEADER_CURL_MD5_H */
