#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_MD5_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_MD5_H
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

#ಆದರೆ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#ಅನ್ತರ್ಭಾವ "curl_hmac.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MD5_DIGEST_LEN  16

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode (*Curl_MD5_init_func)(ಶೂನ್ಯ *context);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*Curl_MD5_update_func)(ಶೂನ್ಯ *context,
                                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                                     ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*Curl_MD5_final_func)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಫಲಿತಾಂಶ, ಶೂನ್ಯ *context);

ರಚನೆ MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ md5_ctxtsize;            /* Context structure size */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ md5_resultlen;           /* Result length (bytes) */
};

ರಚನೆ MD5_context {
  ಸ್ಥಿರಾಂಕ ರಚನೆ MD5_params *md5_hash;  /* Hash function definition */
  ಶೂನ್ಯ *md5_hashctx;                  /* Hash function context */
};

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ MD5_params Curl_DIGEST_MD5;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                    size_t len);

ರಚನೆ MD5_context *Curl_MD5_init(ಸ್ಥಿರಾಂಕ ರಚನೆ MD5_params *md5params);
CURLcode Curl_MD5_update(ರಚನೆ MD5_context *context,
                         ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                         ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len);
CURLcode Curl_MD5_final(ರಚನೆ MD5_context *context, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಫಲಿತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_MD5_H */
