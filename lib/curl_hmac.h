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

#إذا (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

تعريفنوع CURLcode (*HMAC_hinit)(فراغ *context);
تعريفنوع فراغ (*HMAC_hupdate)(فراغ *context,
                             ثابت unsigned حرف *data,
                             unsigned صحيح len);
تعريفنوع فراغ (*HMAC_hfinal)(unsigned حرف *result, فراغ *context);

/* Per-hash function HMAC parameters. */
بنية HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned صحيح ctxtsize;   /* Context structure size. */
  unsigned صحيح maxkeylen;  /* Maximum key length (bytes). */
  unsigned صحيح resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
بنية HMAC_context {
  ثابت بنية HMAC_params *hash; /* Hash function definition. */
  فراغ *hashctxt1;         /* Hash function context 1. */
  فراغ *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
بنية HMAC_context *Curl_HMAC_init(ثابت بنية HMAC_params *hashparams,
                                    ثابت unsigned حرف *مفتاح,
                                    unsigned صحيح keylen);
فراغ Curl_HMAC_update(بنية HMAC_context *ctxt,
                      ثابت unsigned حرف *data,
                      unsigned صحيح len);
صحيح Curl_HMAC_final(بنية HMAC_context *ctxt, unsigned حرف *output);

CURLcode Curl_hmacit(ثابت بنية HMAC_params *hashparams,
                     ثابت unsigned حرف *مفتاح, ثابت size_t keylen,
                     ثابت unsigned حرف *data, size_t datalen,
                     unsigned حرف *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
