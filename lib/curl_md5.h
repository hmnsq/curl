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

#यदि (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#अन्तर्भाव "curl_hmac.h"

#define MD5_DIGEST_LEN  16

प्रकारनिर्देश CURLcode (*Curl_MD5_init_func)(शून्य *context);
प्रकारनिर्देश शून्य (*Curl_MD5_update_func)(शून्य *context,
                                     अचर अचिन्हित वर्ण *data,
                                     अचिन्हित पूर्णाङ्क len);
प्रकारनिर्देश शून्य (*Curl_MD5_final_func)(अचिन्हित वर्ण *result, शून्य *context);

संरचना MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  अचिन्हित पूर्णाङ्क md5_ctxtsize;            /* Context structure size */
  अचिन्हित पूर्णाङ्क md5_resultlen;           /* Result length (bytes) */
};

संरचना MD5_context {
  अचर संरचना MD5_params *md5_hash;  /* Hash function definition */
  शून्य *md5_hashctx;                  /* Hash function context */
};

बाह्य अचर संरचना MD5_params Curl_DIGEST_MD5;
बाह्य अचर संरचना HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(अचिन्हित वर्ण *output, अचर अचिन्हित वर्ण *आगत,
                    माप_प्रकार len);

संरचना MD5_context *Curl_MD5_init(अचर संरचना MD5_params *md5params);
CURLcode Curl_MD5_update(संरचना MD5_context *context,
                         अचर अचिन्हित वर्ण *आगत,
                         अचिन्हित पूर्णाङ्क len);
CURLcode Curl_MD5_final(संरचना MD5_context *context, अचिन्हित वर्ण *result);

#endif

#endif /* HEADER_CURL_MD5_H */
