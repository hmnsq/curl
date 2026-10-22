#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_MD5_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_MD5_H
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

#ਜੇ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_hmac.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_DIGEST_LEN  16

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode (*Curl_MD5_init_func)(ਖਾਲੀ *context);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*Curl_MD5_update_func)(ਖਾਲੀ *context,
                                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*Curl_MD5_final_func)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਤੀਜਾ, ਖਾਲੀ *context);

ਬਣਤਰ MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ md5_ctxtsize;            /* Context structure size */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ md5_resultlen;           /* Result length (bytes) */
};

ਬਣਤਰ MD5_context {
  ਅਚਲ ਬਣਤਰ MD5_params *md5_hash;  /* Hash function definition */
  ਖਾਲੀ *md5_hashctx;                  /* Hash function context */
};

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ MD5_params Curl_DIGEST_MD5;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                    size_t len);

ਬਣਤਰ MD5_context *Curl_MD5_init(ਅਚਲ ਬਣਤਰ MD5_params *md5params);
CURLcode Curl_MD5_update(ਬਣਤਰ MD5_context *context,
                         ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len);
CURLcode Curl_MD5_final(ਬਣਤਰ MD5_context *context, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਤੀਜਾ);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_MD5_H */
