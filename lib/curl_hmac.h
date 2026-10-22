#अगर_अघोषित HEADER_CURL_HMAC_H
#घोषणा HEADER_CURL_HMAC_H
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

#अगर (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#घोषणा HMAC_MD5_लंबाई 16

प्रकार CURLcode (*HMAC_hinit)(व्योम *context);
प्रकार व्योम (*HMAC_hupdate)(व्योम *context,
                             स्थिर अचिन्हित अक्षर *तथ्य,
                             अचिन्हित पूर्णांक len);
प्रकार व्योम (*HMAC_hfinal)(अचिन्हित अक्षर *परिणाम, व्योम *context);

/* Per-hash function HMAC parameters. */
काष्ठा HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  अचिन्हित पूर्णांक ctxtsize;   /* Context structure size. */
  अचिन्हित पूर्णांक maxkeylen;  /* Maximum key length (bytes). */
  अचिन्हित पूर्णांक resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
काष्ठा HMAC_context {
  स्थिर काष्ठा HMAC_params *hash; /* Hash function definition. */
  व्योम *hashctxt1;         /* Hash function context 1. */
  व्योम *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
काष्ठा HMAC_context *Curl_HMAC_init(स्थिर काष्ठा HMAC_params *hashparams,
                                    स्थिर अचिन्हित अक्षर *कुंजी,
                                    अचिन्हित पूर्णांक keylen);
व्योम Curl_HMAC_update(काष्ठा HMAC_context *ctxt,
                      स्थिर अचिन्हित अक्षर *तथ्य,
                      अचिन्हित पूर्णांक len);
पूर्णांक Curl_HMAC_final(काष्ठा HMAC_context *ctxt, अचिन्हित अक्षर *output);

CURLcode Curl_hmacit(स्थिर काष्ठा HMAC_params *hashparams,
                     स्थिर अचिन्हित अक्षर *कुंजी, स्थिर माप_प्रकार keylen,
                     स्थिर अचिन्हित अक्षर *तथ्य, माप_प्रकार datalen,
                     अचिन्हित अक्षर *output);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_HMAC_H */
