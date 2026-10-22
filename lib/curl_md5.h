#अगर_अघोषित HEADER_CURL_MD5_H
#घोषणा HEADER_CURL_MD5_H
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

#अगर (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#समावेश "curl_hmac.h"

#घोषणा MD5_DIGEST_LEN  16

प्रकार CURLcode (*Curl_MD5_init_func)(व्योम *context);
प्रकार व्योम (*Curl_MD5_update_func)(व्योम *context,
                                     स्थिर अचिन्हित अक्षर *तथ्य,
                                     अचिन्हित पूर्णांक len);
प्रकार व्योम (*Curl_MD5_final_func)(अचिन्हित अक्षर *परिणाम, व्योम *context);

काष्ठा MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  अचिन्हित पूर्णांक md5_ctxtsize;            /* Context structure size */
  अचिन्हित पूर्णांक md5_resultlen;           /* Result length (bytes) */
};

काष्ठा MD5_context {
  स्थिर काष्ठा MD5_params *md5_hash;  /* Hash function definition */
  व्योम *md5_hashctx;                  /* Hash function context */
};

बाह्य स्थिर काष्ठा MD5_params Curl_DIGEST_MD5;
बाह्य स्थिर काष्ठा HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(अचिन्हित अक्षर *output, स्थिर अचिन्हित अक्षर *निवेश,
                    माप_प्रकार len);

काष्ठा MD5_context *Curl_MD5_init(स्थिर काष्ठा MD5_params *md5params);
CURLcode Curl_MD5_update(काष्ठा MD5_context *context,
                         स्थिर अचिन्हित अक्षर *निवेश,
                         अचिन्हित पूर्णांक len);
CURLcode Curl_MD5_final(काष्ठा MD5_context *context, अचिन्हित अक्षर *परिणाम);

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_MD5_H */
