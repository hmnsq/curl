#جے_بنا_تعریف سرخی_CURL_HMAC_H
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

#إذا (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#تعریف_کرو HMAC_MD5_لمائی 16

تعريفنوع CURLcode (*HMAC_hinit)(فراغ *context);
تعريفنوع فراغ (*HMAC_hupdate)(فراغ *context,
                             ثابت بنانشانی حرف *انکڑے,
                             بنانشانی صحيح len);
تعريفنوع فراغ (*HMAC_hfinal)(بنانشانی حرف *نتیجہ, فراغ *context);

/* Per-hash function HMAC parameters. */
بنية HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  بنانشانی صحيح ctxtsize;   /* Context structure size. */
  بنانشانی صحيح maxkeylen;  /* Maximum key length (bytes). */
  بنانشانی صحيح resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
بنية HMAC_context {
  ثابت بنية HMAC_params *hash; /* Hash function definition. */
  فراغ *hashctxt1;         /* Hash function context 1. */
  فراغ *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
بنية HMAC_context *Curl_HMAC_init(ثابت بنية HMAC_params *hashparams,
                                    ثابت بنانشانی حرف *کلید,
                                    بنانشانی صحيح keylen);
فراغ Curl_HMAC_update(بنية HMAC_context *ctxt,
                      ثابت بنانشانی حرف *انکڑے,
                      بنانشانی صحيح len);
صحيح Curl_HMAC_final(بنية HMAC_context *ctxt, بنانشانی حرف *output);

CURLcode Curl_hmacit(ثابت بنية HMAC_params *hashparams,
                     ثابت بنانشانی حرف *کلید, ثابت size_t keylen,
                     ثابت بنانشانی حرف *انکڑے, size_t datalen,
                     بنانشانی حرف *output);
#جے_انت

#جے_انت /* HEADER_CURL_HMAC_H */
