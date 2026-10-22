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

#எனில் (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#அந்தர்பாவ "curl_hmac.h"

#define MD5_DIGEST_LEN  16

வகைவரை CURLcode (*Curl_MD5_init_func)(வெறுமை *context);
வகைவரை வெறுமை (*Curl_MD5_update_func)(வெறுமை *context,
                                     மாறிலி unsigned எழுத்து *data,
                                     unsigned முழு len);
வகைவரை வெறுமை (*Curl_MD5_final_func)(unsigned எழுத்து *result, வெறுமை *context);

கட்டமைப்பு MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned முழு md5_ctxtsize;            /* Context structure size */
  unsigned முழு md5_resultlen;           /* Result length (bytes) */
};

கட்டமைப்பு MD5_context {
  மாறிலி கட்டமைப்பு MD5_params *md5_hash;  /* Hash function definition */
  வெறுமை *md5_hashctx;                  /* Hash function context */
};

வெளி மாறிலி கட்டமைப்பு MD5_params Curl_DIGEST_MD5;
வெளி மாறிலி கட்டமைப்பு HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned எழுத்து *output, மாறிலி unsigned எழுத்து *உள்ளீடு,
                    size_t len);

கட்டமைப்பு MD5_context *Curl_MD5_init(மாறிலி கட்டமைப்பு MD5_params *md5params);
CURLcode Curl_MD5_update(கட்டமைப்பு MD5_context *context,
                         மாறிலி unsigned எழுத்து *உள்ளீடு,
                         unsigned முழு len);
CURLcode Curl_MD5_final(கட்டமைப்பு MD5_context *context, unsigned எழுத்து *result);

#endif

#endif /* HEADER_CURL_MD5_H */
