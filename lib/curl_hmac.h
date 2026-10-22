#اگر_غیر_تعریف_شدہ سرخی_CURL_HMAC_H
#تعریف_کرو سرخی_CURL_HMAC_H
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

#اگر (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#تعریف_کرو HMAC_MD5_لمبائی 16

قسمتعریف CURLcode (*HMAC_hinit)(خالی *context);
قسمتعریف خالی (*HMAC_hupdate)(خالی *context,
                             ثابت غیرعلامتی حرف *ڈیٹا,
                             غیرعلامتی صحیح len);
قسمتعریف خالی (*HMAC_hfinal)(غیرعلامتی حرف *نتیجہ, خالی *context);

/* Per-hash function HMAC parameters. */
ساخت HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  غیرعلامتی صحیح ctxtsize;   /* Context structure size. */
  غیرعلامتی صحیح maxkeylen;  /* Maximum key length (bytes). */
  غیرعلامتی صحیح resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
ساخت HMAC_context {
  ثابت ساخت HMAC_params *hash; /* Hash function definition. */
  خالی *hashctxt1;         /* Hash function context 1. */
  خالی *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
ساخت HMAC_context *Curl_HMAC_init(ثابت ساخت HMAC_params *hashparams,
                                    ثابت غیرعلامتی حرف *کلید,
                                    غیرعلامتی صحیح keylen);
خالی Curl_HMAC_update(ساخت HMAC_context *ctxt,
                      ثابت غیرعلامتی حرف *ڈیٹا,
                      غیرعلامتی صحیح len);
صحیح Curl_HMAC_final(ساخت HMAC_context *ctxt, غیرعلامتی حرف *output);

CURLcode Curl_hmacit(ثابت ساخت HMAC_params *hashparams,
                     ثابت غیرعلامتی حرف *کلید, ثابت size_t keylen,
                     ثابت غیرعلامتی حرف *ڈیٹا, size_t datalen,
                     غیرعلامتی حرف *output);
#اگر_ختم

#اگر_ختم /* HEADER_CURL_HMAC_H */
