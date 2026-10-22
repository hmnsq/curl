#ifndef HEADER_CURL_HMAC_H
#define HEADER_CURL_HMAC_H
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

#ಯದಿ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode (*HMAC_hinit)(ಶೂನ್ಯ *context);
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*HMAC_hupdate)(ಶೂನ್ಯ *context,
                             ಧ್ರುವ unsigned ವರ್ಣ *data,
                             unsigned ಪೂರ್ಣಾಙ್ಕ len);
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*HMAC_hfinal)(unsigned ವರ್ಣ *result, ಶೂನ್ಯ *context);

/* Per-hash function HMAC parameters. */
ಸಂರಚನಾ HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned ಪೂರ್ಣಾಙ್ಕ ctxtsize;   /* Context structure size. */
  unsigned ಪೂರ್ಣಾಙ್ಕ maxkeylen;  /* Maximum key length (bytes). */
  unsigned ಪೂರ್ಣಾಙ್ಕ resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
ಸಂರಚನಾ HMAC_context {
  ಧ್ರುವ ಸಂರಚನಾ HMAC_params *hash; /* Hash function definition. */
  ಶೂನ್ಯ *hashctxt1;         /* Hash function context 1. */
  ಶೂನ್ಯ *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
ಸಂರಚನಾ HMAC_context *Curl_HMAC_init(ಧ್ರುವ ಸಂರಚನಾ HMAC_params *hashparams,
                                    ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ,
                                    unsigned ಪೂರ್ಣಾಙ್ಕ keylen);
ಶೂನ್ಯ Curl_HMAC_update(ಸಂರಚನಾ HMAC_context *ctxt,
                      ಧ್ರುವ unsigned ವರ್ಣ *data,
                      unsigned ಪೂರ್ಣಾಙ್ಕ len);
ಪೂರ್ಣಾಙ್ಕ Curl_HMAC_final(ಸಂರಚನಾ HMAC_context *ctxt, unsigned ವರ್ಣ *output);

CURLcode Curl_hmacit(ಧ್ರುವ ಸಂರಚನಾ HMAC_params *hashparams,
                     ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ size_t keylen,
                     ಧ್ರುವ unsigned ವರ್ಣ *data, size_t datalen,
                     unsigned ವರ್ಣ *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
