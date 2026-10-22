#यदि_अपरिभाषितम् शीर्षकम्_CURL_HMAC_H
#परिभाषय शीर्षकम्_CURL_HMAC_H
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

#यदि (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#परिभाषय HMAC_MD5_दीर्घता 16

प्रकारनिर्देश CURLcode (*HMAC_hinit)(शून्य *context);
प्रकारनिर्देश शून्य (*HMAC_hupdate)(शून्य *context,
                             ध्रुव अचिह्नित वर्ण *दत्त,
                             अचिह्नित पूर्णाङ्क len);
प्रकारनिर्देश शून्य (*HMAC_hfinal)(अचिह्नित वर्ण *परिणाम, शून्य *context);

/* Per-hash function HMAC parameters. */
संरचना HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  अचिह्नित पूर्णाङ्क ctxtsize;   /* Context structure size. */
  अचिह्नित पूर्णाङ्क maxkeylen;  /* Maximum key length (bytes). */
  अचिह्नित पूर्णाङ्क resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
संरचना HMAC_context {
  ध्रुव संरचना HMAC_params *hash; /* Hash function definition. */
  शून्य *hashctxt1;         /* Hash function context 1. */
  शून्य *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
संरचना HMAC_context *Curl_HMAC_init(ध्रुव संरचना HMAC_params *hashparams,
                                    ध्रुव अचिह्नित वर्ण *कुञ्चिका,
                                    अचिह्नित पूर्णाङ्क keylen);
शून्य Curl_HMAC_update(संरचना HMAC_context *ctxt,
                      ध्रुव अचिह्नित वर्ण *दत्त,
                      अचिह्नित पूर्णाङ्क len);
पूर्णाङ्क Curl_HMAC_final(संरचना HMAC_context *ctxt, अचिह्नित वर्ण *output);

CURLcode Curl_hmacit(ध्रुव संरचना HMAC_params *hashparams,
                     ध्रुव अचिह्नित वर्ण *कुञ्चिका, ध्रुव size_t keylen,
                     ध्रुव अचिह्नित वर्ण *दत्त, size_t datalen,
                     अचिह्नित वर्ण *output);
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_HMAC_H */
