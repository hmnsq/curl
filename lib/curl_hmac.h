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

#ᨀᨘ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

typedef CURLcode (*HMAC_hinit)(void *context);
typedef void (*HMAC_hupdate)(void *context,
                             const unsigned var_na *data,
                             unsigned puur_naa_nka len);
typedef void (*HMAC_hfinal)(unsigned var_na *result, void *context);

/* Per-hash function HMAC parameters. */
sa_nracanaa HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned puur_naa_nka ctxtsize;   /* Context structure size. */
  unsigned puur_naa_nka maxkeylen;  /* Maximum key length (bytes). */
  unsigned puur_naa_nka resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
sa_nracanaa HMAC_context {
  const sa_nracanaa HMAC_params *hash; /* Hash function definition. */
  void *hashctxt1;         /* Hash function context 1. */
  void *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
sa_nracanaa HMAC_context *Curl_HMAC_init(const sa_nracanaa HMAC_params *hashparams,
                                    const unsigned var_na *key,
                                    unsigned puur_naa_nka keylen);
void Curl_HMAC_update(sa_nracanaa HMAC_context *ctxt,
                      const unsigned var_na *data,
                      unsigned puur_naa_nka len);
puur_naa_nka Curl_HMAC_final(sa_nracanaa HMAC_context *ctxt, unsigned var_na *output);

CURLcode Curl_hmacit(const sa_nracanaa HMAC_params *hashparams,
                     const unsigned var_na *key, const size_t keylen,
                     const unsigned var_na *data, size_t datalen,
                     unsigned var_na *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
