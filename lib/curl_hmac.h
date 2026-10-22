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

#define HMAC_MD5_দৈর্ঘ্য 16

প্রকারলেখ CURLcode (*HMAC_hinit)(শূন্য *context);
প্রকারলেখ শূন্য (*HMAC_hupdate)(শূন্য *context,
                             ধ্রুবক unsigned অক্ষর *তথ্য,
                             unsigned পূর্ণসংখ্যা len);
প্রকারলেখ শূন্য (*HMAC_hfinal)(unsigned অক্ষর *ফলাফল, শূন্য *context);

/* Per-hash function HMAC parameters. */
কাঠামো HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned পূর্ণসংখ্যা ctxtsize;   /* Context structure size. */
  unsigned পূর্ণসংখ্যা maxkeylen;  /* Maximum key length (bytes). */
  unsigned পূর্ণসংখ্যা resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
কাঠামো HMAC_context {
  ধ্রুবক কাঠামো HMAC_params *hash; /* Hash function definition. */
  শূন্য *hashctxt1;         /* Hash function context 1. */
  শূন্য *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
কাঠামো HMAC_context *Curl_HMAC_init(ধ্রুবক কাঠামো HMAC_params *hashparams,
                                    ধ্রুবক unsigned অক্ষর *চাবি,
                                    unsigned পূর্ণসংখ্যা keylen);
শূন্য Curl_HMAC_update(কাঠামো HMAC_context *ctxt,
                      ধ্রুবক unsigned অক্ষর *তথ্য,
                      unsigned পূর্ণসংখ্যা len);
পূর্ণসংখ্যা Curl_HMAC_final(কাঠামো HMAC_context *ctxt, unsigned অক্ষর *output);

CURLcode Curl_hmacit(ধ্রুবক কাঠামো HMAC_params *hashparams,
                     ধ্রুবক unsigned অক্ষর *চাবি, ধ্রুবক size_t keylen,
                     ধ্রুবক unsigned অক্ষর *তথ্য, size_t datalen,
                     unsigned অক্ষর *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
