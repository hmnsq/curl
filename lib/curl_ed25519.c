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
#अन्तर्भाव "curl_setup.h"

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#अन्तर्भाव "curl_ed25519.h"

#ifdef USE_WOLFSSL
#अन्तर्भाव <wolfssl/options.h>
#अन्तर्भाव <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#अन्तर्भाव <openssl/evp.h>

CURLcode Curl_ed25519_sign(स्थिरांक अचिन्हित अक्षर *किल्ली, माप_प्रकार keylen,
                           स्थिरांक अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  माप_प्रकार slen;
  पूर्णांक rc;

  जर(keylen != CURL_ED25519_KEYLEN)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_किल्ली(EVP_PKEY_ED25519, रिक्त, किल्ली, keylen);
  जर(!pkey)
    परत CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  जर(!mdctx) {
    EVP_PKEY_free(pkey);
    परत CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, रिक्त, रिक्त, रिक्त, pkey);
  जर(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    परत CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, संदेश, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  जर(rc != 1)
    परत CURLE_AUTH_ERROR;

  *siglen = slen;
  परत CURLE_OK;
}

#किंवाजर defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_किल्ली_IMPORT) && defined(HAVE_ED25519_SIGN)
#अन्तर्भाव <wolfssl/wolfcrypt/ed25519.h>
#अन्तर्भाव <wolfssl/wolfcrypt/error-crypt.h>
#अन्तर्भाव <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(स्थिरांक अचिन्हित अक्षर *किल्ली, माप_प्रकार keylen,
                           स्थिरांक अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  पूर्णांक ret;
  ed25519_किल्ली edkey;
  word32 outlen;
  अचिन्हित अक्षर pubkey[ED25519_PUB_किल्ली_SIZE];

  जर(keylen != ED25519_किल्ली_SIZE)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  जर(ret)
    परत CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(किल्ली, ED25519_किल्ली_SIZE, &edkey);
  जर(ret)
    जा fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_किल्ली_SIZE);
  जर(ret)
    जा fail;

  ret = wc_ed25519_import_private_किल्ली(किल्ली, ED25519_किल्ली_SIZE,
                                      pubkey, ED25519_PUB_किल्ली_SIZE, &edkey);
  जर(ret)
    जा fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_संदेश(संदेश, (word32)msglen, sig, &outlen, &edkey);
  जर(ret)
    जा fail;

  *siglen = (माप_प्रकार)outlen;
  wc_ed25519_free(&edkey);
  परत CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  परत CURLE_AUTH_ERROR;
}

#किंवाजर defined(USE_GNUTLS)
#अन्तर्भाव <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(स्थिरांक अचिन्हित अक्षर *किल्ली, माप_प्रकार keylen,
                           स्थिरांक अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  uint8_t pubkey[ED25519_किल्ली_SIZE];

  जर(keylen != ED25519_किल्ली_SIZE)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_किल्ली(pubkey, किल्ली);

  nettle_ed25519_sha512_sign(pubkey, किल्ली, msglen, संदेश, sig);
  *siglen = CURL_ED25519_SIGLEN;

  परत CURLE_OK;
}

#अन्यथा /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(स्थिरांक अचिन्हित अक्षर *किल्ली, माप_प्रकार keylen,
                           स्थिरांक अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  (शून्य)किल्ली;
  (शून्य)keylen;
  (शून्य)संदेश;
  (शून्य)msglen;
  (शून्य)sig;
  (शून्य)siglen;
  परत CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
