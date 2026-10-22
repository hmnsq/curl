#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HMAC_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HMAC_H
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

#ಆದರೆ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ವ್ಯಾಖ್ಯಾನಿಸು HMAC_MD5_ಉದ್ದಳತೆ 16

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode (*HMAC_hinit)(ಶೂನ್ಯ *context);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*HMAC_hupdate)(ಶೂನ್ಯ *context,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*HMAC_hfinal)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಫಲಿತಾಂಶ, ಶೂನ್ಯ *context);

/* Per-hash function HMAC parameters. */
ರಚನೆ HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ctxtsize;   /* Context structure size. */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ maxkeylen;  /* Maximum key length (bytes). */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
ರಚನೆ HMAC_context {
  ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params *hash; /* Hash function definition. */
  ಶೂನ್ಯ *hashctxt1;         /* Hash function context 1. */
  ಶೂನ್ಯ *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
ರಚನೆ HMAC_context *Curl_HMAC_init(ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params *hashparams,
                                    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ,
                                    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keylen);
ಶೂನ್ಯ Curl_HMAC_update(ರಚನೆ HMAC_context *ctxt,
                      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len);
ಪೂರ್ಣಾಂಕ Curl_HMAC_final(ರಚನೆ HMAC_context *ctxt, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output);

CURLcode Curl_hmacit(ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params *hashparams,
                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ size_t keylen,
                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ, size_t datalen,
                     ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HMAC_H */
