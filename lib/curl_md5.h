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

#اگر (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#شامل "curl_hmac.h"

#define MD5_DIGEST_LEN  16

تعریفنوع CURLcode (*Curl_MD5_init_func)(تهی *context);
تعریفنوع تهی (*Curl_MD5_update_func)(تهی *context,
                                     ثابت unsigned نویسه *data,
                                     unsigned صحیح len);
تعریفنوع تهی (*Curl_MD5_final_func)(unsigned نویسه *result, تهی *context);

ساختار MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned صحیح md5_ctxtsize;            /* Context structure size */
  unsigned صحیح md5_resultlen;           /* Result length (bytes) */
};

ساختار MD5_context {
  ثابت ساختار MD5_params *md5_hash;  /* Hash function definition */
  تهی *md5_hashctx;                  /* Hash function context */
};

بیرونی ثابت ساختار MD5_params Curl_DIGEST_MD5;
بیرونی ثابت ساختار HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned نویسه *output, ثابت unsigned نویسه *درون_داد,
                    size_t len);

ساختار MD5_context *Curl_MD5_init(ثابت ساختار MD5_params *md5params);
CURLcode Curl_MD5_update(ساختار MD5_context *context,
                         ثابت unsigned نویسه *درون_داد,
                         unsigned صحیح len);
CURLcode Curl_MD5_final(ساختار MD5_context *context, unsigned نویسه *result);

#endif

#endif /* HEADER_CURL_MD5_H */
