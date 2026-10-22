#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HMAC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HMAC_H
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

#ਜੇ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMAC_MD5_ਲੰਮਾਈ 16

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode (*HMAC_hinit)(ਖਾਲੀ *context);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*HMAC_hupdate)(ਖਾਲੀ *context,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*HMAC_hfinal)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਤੀਜਾ, ਖਾਲੀ *context);

/* Per-hash function HMAC parameters. */
ਬਣਤਰ HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ctxtsize;   /* Context structure size. */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ maxkeylen;  /* Maximum key length (bytes). */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
ਬਣਤਰ HMAC_context {
  ਅਚਲ ਬਣਤਰ HMAC_params *hash; /* Hash function definition. */
  ਖਾਲੀ *hashctxt1;         /* Hash function context 1. */
  ਖਾਲੀ *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
ਬਣਤਰ HMAC_context *Curl_HMAC_init(ਅਚਲ ਬਣਤਰ HMAC_params *hashparams,
                                    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keylen);
ਖਾਲੀ Curl_HMAC_update(ਬਣਤਰ HMAC_context *ctxt,
                      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len);
ਪੂਰਨ Curl_HMAC_final(ਬਣਤਰ HMAC_context *ctxt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output);

CURLcode Curl_hmacit(ਅਚਲ ਬਣਤਰ HMAC_params *hashparams,
                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ size_t keylen,
                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ, size_t datalen,
                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_HMAC_H */
