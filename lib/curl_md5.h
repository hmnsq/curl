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

#ଯଦି (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#ଅନ୍ତର୍ଭାଵ "curl_hmac.h"

#define MD5_DIGEST_LEN  16

ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode (*Curl_MD5_init_func)(ଶୂନ୍ୟ *context);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*Curl_MD5_update_func)(ଶୂନ୍ୟ *context,
                                     ଅଚଳ unsigned ଅକ୍ଷର *data,
                                     unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*Curl_MD5_final_func)(unsigned ଅକ୍ଷର *result, ଶୂନ୍ୟ *context);

ସଂରଚନା MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା md5_ctxtsize;            /* Context structure size */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା md5_resultlen;           /* Result length (bytes) */
};

ସଂରଚନା MD5_context {
  ଅଚଳ ସଂରଚନା MD5_params *md5_hash;  /* Hash function definition */
  ଶୂନ୍ୟ *md5_hashctx;                  /* Hash function context */
};

ବାହ୍ୟ ଅଚଳ ସଂରଚନା MD5_params Curl_DIGEST_MD5;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned ଅକ୍ଷର *output, ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ,
                    size_t len);

ସଂରଚନା MD5_context *Curl_MD5_init(ଅଚଳ ସଂରଚନା MD5_params *md5params);
CURLcode Curl_MD5_update(ସଂରଚନା MD5_context *context,
                         ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ,
                         unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
CURLcode Curl_MD5_final(ସଂରଚନା MD5_context *context, unsigned ଅକ୍ଷର *result);

#endif

#endif /* HEADER_CURL_MD5_H */
