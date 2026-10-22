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

#જો (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#અન્તર્ભાવ "curl_hmac.h"

#define MD5_DIGEST_LEN  16

પ્રકારવ્યાખ્યા CURLcode (*Curl_MD5_init_func)(શૂન્ય *context);
પ્રકારવ્યાખ્યા શૂન્ય (*Curl_MD5_update_func)(શૂન્ય *context,
                                     અચલ unsigned અક્ષર *data,
                                     unsigned પૂર્ણાંક len);
પ્રકારવ્યાખ્યા શૂન્ય (*Curl_MD5_final_func)(unsigned અક્ષર *result, શૂન્ય *context);

સંરચના MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  unsigned પૂર્ણાંક md5_ctxtsize;            /* Context structure size */
  unsigned પૂર્ણાંક md5_resultlen;           /* Result length (bytes) */
};

સંરચના MD5_context {
  અચલ સંરચના MD5_params *md5_hash;  /* Hash function definition */
  શૂન્ય *md5_hashctx;                  /* Hash function context */
};

બાહ્ય અચલ સંરચના MD5_params Curl_DIGEST_MD5;
બાહ્ય અચલ સંરચના HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(unsigned અક્ષર *output, અચલ unsigned અક્ષર *આગત,
                    size_t len);

સંરચના MD5_context *Curl_MD5_init(અચલ સંરચના MD5_params *md5params);
CURLcode Curl_MD5_update(સંરચના MD5_context *context,
                         અચલ unsigned અક્ષર *આગત,
                         unsigned પૂર્ણાંક len);
CURLcode Curl_MD5_final(સંરચના MD5_context *context, unsigned અક્ષર *result);

#endif

#endif /* HEADER_CURL_MD5_H */
