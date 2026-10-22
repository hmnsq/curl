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

#ಯದಿ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#ಅನ್ತರ್ಭಾವ "curl_hmac.h"

#define MD5_DIGEST_LEN  16

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode (*Curl_MD5_init_func)(ಶೂನ್ಯ *context);
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*Curl_MD5_update_func)(ಶೂನ್ಯ *context,
                                     ಧ್ರುವ unsigned ವರ್ಣ *data,
                                     unsigned ಪೂರ್ಣಾಙ್ಕ len);
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*Curl_MD5_final_func)(unsigned ವರ್ಣ *result, ಶೂನ್ಯ *context);

ಸಂರಚನಾ MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned ಪೂರ್ಣಾಙ್ಕ md5_ctxtsize;            /* Context structure size */
  unsigned ಪೂರ್ಣಾಙ್ಕ md5_resultlen;           /* Result length (bytes) */
};

ಸಂರಚನಾ MD5_context {
  ಧ್ರುವ ಸಂರಚನಾ MD5_params *md5_hash;  /* Hash function definition */
  ಶೂನ್ಯ *md5_hashctx;                  /* Hash function context */
};

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ MD5_params Curl_DIGEST_MD5;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned ವರ್ಣ *output, ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು,
                    size_t len);

ಸಂರಚನಾ MD5_context *Curl_MD5_init(ಧ್ರುವ ಸಂರಚನಾ MD5_params *md5params);
CURLcode Curl_MD5_update(ಸಂರಚನಾ MD5_context *context,
                         ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು,
                         unsigned ಪೂರ್ಣಾಙ್ಕ len);
CURLcode Curl_MD5_final(ಸಂರಚನಾ MD5_context *context, unsigned ವರ್ಣ *result);

#endif

#endif /* HEADER_CURL_MD5_H */
