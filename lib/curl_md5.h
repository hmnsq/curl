#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_MD5_H
#وصف_ڪريو سرو_CURL_MD5_H
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

#إذا (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#ضمن "curl_hmac.h"

#وصف_ڪريو MD5_DIGEST_LEN  16

تعريفنوع CURLcode (*Curl_MD5_init_func)(فراغ *context);
تعريفنوع فراغ (*Curl_MD5_update_func)(فراغ *context,
                                     ثابت بنانشان حرف *معلومات,
                                     بنانشان صحيح len);
تعريفنوع فراغ (*Curl_MD5_final_func)(بنانشان حرف *نتيجو, فراغ *context);

بنية MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  بنانشان صحيح md5_ctxtsize;            /* Context structure size */
  بنانشان صحيح md5_resultlen;           /* Result length (bytes) */
};

بنية MD5_context {
  ثابت بنية MD5_params *md5_hash;  /* Hash function definition */
  فراغ *md5_hashctx;                  /* Hash function context */
};

خارجي ثابت بنية MD5_params Curl_DIGEST_MD5;
خارجي ثابت بنية HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(بنانشان حرف *output, ثابت بنانشان حرف *داخلا,
                    size_t len);

بنية MD5_context *Curl_MD5_init(ثابت بنية MD5_params *md5params);
CURLcode Curl_MD5_update(بنية MD5_context *context,
                         ثابت بنانشان حرف *داخلا,
                         بنانشان صحيح len);
CURLcode Curl_MD5_final(بنية MD5_context *context, بنانشان حرف *نتيجو);

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_MD5_H */
