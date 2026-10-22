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

#எனில் (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

வகைவரை CURLcode (*HMAC_hinit)(வெறுமை *context);
வகைவரை வெறுமை (*HMAC_hupdate)(வெறுமை *context,
                             மாறிலி unsigned எழுத்து *data,
                             unsigned முழு len);
வகைவரை வெறுமை (*HMAC_hfinal)(unsigned எழுத்து *result, வெறுமை *context);

/* Per-hash function HMAC parameters. */
கட்டமைப்பு HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned முழு ctxtsize;   /* Context structure size. */
  unsigned முழு maxkeylen;  /* Maximum key length (bytes). */
  unsigned முழு resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
கட்டமைப்பு HMAC_context {
  மாறிலி கட்டமைப்பு HMAC_params *hash; /* Hash function definition. */
  வெறுமை *hashctxt1;         /* Hash function context 1. */
  வெறுமை *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
கட்டமைப்பு HMAC_context *Curl_HMAC_init(மாறிலி கட்டமைப்பு HMAC_params *hashparams,
                                    மாறிலி unsigned எழுத்து *சாவி,
                                    unsigned முழு keylen);
வெறுமை Curl_HMAC_update(கட்டமைப்பு HMAC_context *ctxt,
                      மாறிலி unsigned எழுத்து *data,
                      unsigned முழு len);
முழு Curl_HMAC_final(கட்டமைப்பு HMAC_context *ctxt, unsigned எழுத்து *output);

CURLcode Curl_hmacit(மாறிலி கட்டமைப்பு HMAC_params *hashparams,
                     மாறிலி unsigned எழுத்து *சாவி, மாறிலி size_t keylen,
                     மாறிலி unsigned எழுத்து *data, size_t datalen,
                     unsigned எழுத்து *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
