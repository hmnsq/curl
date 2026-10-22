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

#यदि (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#define HMAC_MD5_LENGTH 16

प्रकारनिर्देश CURLcode (*HMAC_hinit)(शून्य *context);
प्रकारनिर्देश शून्य (*HMAC_hupdate)(शून्य *context,
                             ध्रुव अचिन्हित वर्ण *data,
                             अचिन्हित पूर्णाङ्क len);
प्रकारनिर्देश शून्य (*HMAC_hfinal)(अचिन्हित वर्ण *result, शून्य *context);

/* Per-hash function HMAC parameters. */
संरचना HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  अचिन्हित पूर्णाङ्क ctxtsize;   /* Context structure size. */
  अचिन्हित पूर्णाङ्क maxkeylen;  /* Maximum key length (bytes). */
  अचिन्हित पूर्णाङ्क resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
संरचना HMAC_context {
  ध्रुव संरचना HMAC_params *hash; /* Hash function definition. */
  शून्य *hashctxt1;         /* Hash function context 1. */
  शून्य *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
संरचना HMAC_context *Curl_HMAC_init(ध्रुव संरचना HMAC_params *hashparams,
                                    ध्रुव अचिन्हित वर्ण *कुंजी,
                                    अचिन्हित पूर्णाङ्क keylen);
शून्य Curl_HMAC_update(संरचना HMAC_context *ctxt,
                      ध्रुव अचिन्हित वर्ण *data,
                      अचिन्हित पूर्णाङ्क len);
पूर्णाङ्क Curl_HMAC_final(संरचना HMAC_context *ctxt, अचिन्हित वर्ण *output);

CURLcode Curl_hmacit(ध्रुव संरचना HMAC_params *hashparams,
                     ध्रुव अचिन्हित वर्ण *कुंजी, ध्रुव माप_प्रकार keylen,
                     ध्रुव अचिन्हित वर्ण *data, माप_प्रकार datalen,
                     अचिन्हित वर्ण *output);
#endif

#endif /* HEADER_CURL_HMAC_H */
