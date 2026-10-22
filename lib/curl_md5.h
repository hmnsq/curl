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

#অন্তর্ভাৱ "curl_hmac.h"

#define MD5_DIGEST_LEN  16

প্রকারনির্দেশ CURLcode (*Curl_MD5_init_func)(শূন্য *context);
প্রকারনির্দেশ শূন্য (*Curl_MD5_update_func)(শূন্য *context,
                                     ধ্রুৱ unsigned ৱর্ণ *data,
                                     unsigned পূর্ণাঙ্ক len);
প্রকারনির্দেশ শূন্য (*Curl_MD5_final_func)(unsigned ৱর্ণ *result, শূন্য *context);

সংরচনা MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned পূর্ণাঙ্ক md5_ctxtsize;            /* Context structure size */
  unsigned পূর্ণাঙ্ক md5_resultlen;           /* Result length (bytes) */
};

সংরচনা MD5_context {
  ধ্রুৱ সংরচনা MD5_params *md5_hash;  /* Hash function definition */
  শূন্য *md5_hashctx;                  /* Hash function context */
};

বাহ্য ধ্রুৱ সংরচনা MD5_params Curl_DIGEST_MD5;
বাহ্য ধ্রুৱ সংরচনা HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned ৱর্ণ *output, ধ্রুৱ unsigned ৱর্ণ *প্ৰবেশ,
                    size_t len);

সংরচনা MD5_context *Curl_MD5_init(ধ্রুৱ সংরচনা MD5_params *md5params);
CURLcode Curl_MD5_update(সংরচনা MD5_context *context,
                         ধ্রুৱ unsigned ৱর্ণ *প্ৰবেশ,
                         unsigned পূর্ণাঙ্ক len);
CURLcode Curl_MD5_final(সংরচনা MD5_context *context, unsigned ৱর্ণ *result);

#endif

#endif /* HEADER_CURL_MD5_H */
