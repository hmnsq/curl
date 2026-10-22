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

#ଯଦି (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode (*HMAC_hinit)(ଶୂନ୍ୟ *context);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*HMAC_hupdate)(ଶୂନ୍ୟ *context,
                             ଅଚଳ unsigned ଅକ୍ଷର *data,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*HMAC_hfinal)(unsigned ଅକ୍ଷର *result, ଶୂନ୍ୟ *context);

/* Per-hash function HMAC parameters. */
ସଂରଚନା HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctxtsize;   /* Context structure size. */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxkeylen;  /* Maximum key length (bytes). */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
ସଂରଚନା HMAC_context {
  ଅଚଳ ସଂରଚନା HMAC_params *hash; /* Hash function definition. */
  ଶୂନ୍ୟ *hashctxt1;         /* Hash function context 1. */
  ଶୂନ୍ୟ *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
ସଂରଚନା HMAC_context *Curl_HMAC_init(ଅଚଳ ସଂରଚନା HMAC_params *hashparams,
                                    ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି,
                                    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylen);
ଶୂନ୍ୟ Curl_HMAC_update(ସଂରଚନା HMAC_context *ctxt,
                      ଅଚଳ unsigned ଅକ୍ଷର *data,
                      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_HMAC_final(ସଂରଚନା HMAC_context *ctxt, unsigned ଅକ୍ଷର *output);

CURLcode Curl_hmacit(ଅଚଳ ସଂରଚନା HMAC_params *hashparams,
                     ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି, ଅଚଳ size_t keylen,
                     ଅଚଳ unsigned ଅକ୍ଷର *data, size_t datalen,
                     unsigned ଅକ୍ଷର *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
