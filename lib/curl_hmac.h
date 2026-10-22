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

#যদি (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

প্রকারনির্দেশ CURLcode (*HMAC_hinit)(শূন্য *context);
প্রকারনির্দেশ শূন্য (*HMAC_hupdate)(শূন্য *context,
                             ধ্রুৱ unsigned ৱর্ণ *data,
                             unsigned পূর্ণাঙ্ক len);
প্রকারনির্দেশ শূন্য (*HMAC_hfinal)(unsigned ৱর্ণ *result, শূন্য *context);

/* Per-hash function HMAC parameters. */
সংরচনা HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned পূর্ণাঙ্ক ctxtsize;   /* Context structure size. */
  unsigned পূর্ণাঙ্ক maxkeylen;  /* Maximum key length (bytes). */
  unsigned পূর্ণাঙ্ক resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
সংরচনা HMAC_context {
  ধ্রুৱ সংরচনা HMAC_params *hash; /* Hash function definition. */
  শূন্য *hashctxt1;         /* Hash function context 1. */
  শূন্য *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
সংরচনা HMAC_context *Curl_HMAC_init(ধ্রুৱ সংরচনা HMAC_params *hashparams,
                                    ধ্রুৱ unsigned ৱর্ণ *চাবি,
                                    unsigned পূর্ণাঙ্ক keylen);
শূন্য Curl_HMAC_update(সংরচনা HMAC_context *ctxt,
                      ধ্রুৱ unsigned ৱর্ণ *data,
                      unsigned পূর্ণাঙ্ক len);
পূর্ণাঙ্ক Curl_HMAC_final(সংরচনা HMAC_context *ctxt, unsigned ৱর্ণ *output);

CURLcode Curl_hmacit(ধ্রুৱ সংরচনা HMAC_params *hashparams,
                     ধ্রুৱ unsigned ৱর্ণ *চাবি, ধ্রুৱ size_t keylen,
                     ধ্রুৱ unsigned ৱর্ণ *data, size_t datalen,
                     unsigned ৱর্ণ *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
