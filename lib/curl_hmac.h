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

#જો (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

પ્રકારવ્યાખ્યા CURLcode (*HMAC_hinit)(શૂન્ય *context);
પ્રકારવ્યાખ્યા શૂન્ય (*HMAC_hupdate)(શૂન્ય *context,
                             અચલ unsigned અક્ષર *data,
                             unsigned પૂર્ણાંક len);
પ્રકારવ્યાખ્યા શૂન્ય (*HMAC_hfinal)(unsigned અક્ષર *result, શૂન્ય *context);

/* Per-hash function HMAC parameters. */
સંરચના HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  unsigned પૂર્ણાંક ctxtsize;   /* Context structure size. */
  unsigned પૂર્ણાંક maxkeylen;  /* Maximum key length (bytes). */
  unsigned પૂર્ણાંક resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
સંરચના HMAC_context {
  અચલ સંરચના HMAC_params *hash; /* Hash function definition. */
  શૂન્ય *hashctxt1;         /* Hash function context 1. */
  શૂન્ય *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
સંરચના HMAC_context *Curl_HMAC_init(અચલ સંરચના HMAC_params *hashparams,
                                    અચલ unsigned અક્ષર *ચાવી,
                                    unsigned પૂર્ણાંક keylen);
શૂન્ય Curl_HMAC_update(સંરચના HMAC_context *ctxt,
                      અચલ unsigned અક્ષર *data,
                      unsigned પૂર્ણાંક len);
પૂર્ણાંક Curl_HMAC_final(સંરચના HMAC_context *ctxt, unsigned અક્ષર *output);

CURLcode Curl_hmacit(અચલ સંરચના HMAC_params *hashparams,
                     અચલ unsigned અક્ષર *ચાવી, અચલ size_t keylen,
                     અચલ unsigned અક્ષર *data, size_t datalen,
                     unsigned અક્ષર *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
