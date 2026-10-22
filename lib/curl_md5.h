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

#যদি (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#অন্তর্ভাব "curl_hmac.h"

#define MD5_DIGEST_LEN  16

প্রকারলেখ CURLcode (*Curl_MD5_init_func)(শূন্য *context);
প্রকারলেখ শূন্য (*Curl_MD5_update_func)(শূন্য *context,
                                     ধ্রুবক unsigned অক্ষর *তথ্য,
                                     unsigned পূর্ণসংখ্যা len);
প্রকারলেখ শূন্য (*Curl_MD5_final_func)(unsigned অক্ষর *ফলাফল, শূন্য *context);

কাঠামো MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned পূর্ণসংখ্যা md5_ctxtsize;            /* Context structure size */
  unsigned পূর্ণসংখ্যা md5_resultlen;           /* Result length (bytes) */
};

কাঠামো MD5_context {
  ধ্রুবক কাঠামো MD5_params *md5_hash;  /* Hash function definition */
  শূন্য *md5_hashctx;                  /* Hash function context */
};

বাহ্যিক ধ্রুবক কাঠামো MD5_params Curl_DIGEST_MD5;
বাহ্যিক ধ্রুবক কাঠামো HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned অক্ষর *output, ধ্রুবক unsigned অক্ষর *প্রবেশ,
                    size_t len);

কাঠামো MD5_context *Curl_MD5_init(ধ্রুবক কাঠামো MD5_params *md5params);
CURLcode Curl_MD5_update(কাঠামো MD5_context *context,
                         ধ্রুবক unsigned অক্ষর *প্রবেশ,
                         unsigned পূর্ণসংখ্যা len);
CURLcode Curl_MD5_final(কাঠামো MD5_context *context, unsigned অক্ষর *ফলাফল);

#endif

#endif /* HEADER_CURL_MD5_H */
