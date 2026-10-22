#اگر_غیر_تعریف_شدہ سرخی_CURL_MD5_H
#تعریف_کرو سرخی_CURL_MD5_H
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

#تعریف_کرو MD5_DIGEST_LEN  16

قسمتعریف CURLcode (*Curl_MD5_init_func)(خالی *context);
قسمتعریف خالی (*Curl_MD5_update_func)(خالی *context,
                                     ثابت غیرعلامتی حرف *ڈیٹا,
                                     غیرعلامتی صحیح len);
قسمتعریف خالی (*Curl_MD5_final_func)(غیرعلامتی حرف *نتیجہ, خالی *context);

ساخت MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  غیرعلامتی صحیح md5_ctxtsize;            /* Context structure size */
  غیرعلامتی صحیح md5_resultlen;           /* Result length (bytes) */
};

ساخت MD5_context {
  ثابت ساخت MD5_params *md5_hash;  /* Hash function definition */
  خالی *md5_hashctx;                  /* Hash function context */
};

خارجی ثابت ساخت MD5_params Curl_DIGEST_MD5;
خارجی ثابت ساخت HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(غیرعلامتی حرف *output, ثابت غیرعلامتی حرف *داخلہ,
                    size_t len);

ساخت MD5_context *Curl_MD5_init(ثابت ساخت MD5_params *md5params);
CURLcode Curl_MD5_update(ساخت MD5_context *context,
                         ثابت غیرعلامتی حرف *داخلہ,
                         غیرعلامتی صحیح len);
CURLcode Curl_MD5_final(ساخت MD5_context *context, غیرعلامتی حرف *نتیجہ);

#اگر_ختم

#اگر_ختم /* HEADER_CURL_MD5_H */
