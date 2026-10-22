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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#समावेश "curl_ed25519.h"

#अगर_घोषित USE_WOLFSSL
#समावेश <wolfssl/options.h>
#समावेश <wolfssl/wolfcrypt/settings.h>
#पूर्ण_अगर

#अगर_घोषित USE_OPENSSL
#समावेश <openssl/evp.h>

CURLcode Curl_ed25519_sign(स्थिर अचिन्हित अक्षर *कुंजी, माप_प्रकार keylen,
                           स्थिर अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  माप_प्रकार slen;
  पूर्णांक rc;

  अगर(keylen != CURL_ED25519_KEYLEN)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_कुंजी(EVP_PKEY_ED25519, शून्य, कुंजी, keylen);
  अगर(!pkey)
    वापस CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  अगर(!mdctx) {
    EVP_PKEY_free(pkey);
    वापस CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, शून्य, शून्य, शून्य, pkey);
  अगर(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    वापस CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, संदेश, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  अगर(rc != 1)
    वापस CURLE_AUTH_ERROR;

  *siglen = slen;
  वापस CURLE_OK;
}

#या_अगर defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_कुंजी_IMPORT) && defined(HAVE_ED25519_SIGN)
#समावेश <wolfssl/wolfcrypt/ed25519.h>
#समावेश <wolfssl/wolfcrypt/error-crypt.h>
#समावेश <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(स्थिर अचिन्हित अक्षर *कुंजी, माप_प्रकार keylen,
                           स्थिर अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  पूर्णांक ret;
  ed25519_कुंजी edkey;
  word32 outlen;
  अचिन्हित अक्षर pubkey[ED25519_PUB_कुंजी_आकार];

  अगर(keylen != ED25519_कुंजी_आकार)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  अगर(ret)
    वापस CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(कुंजी, ED25519_कुंजी_आकार, &edkey);
  अगर(ret)
    जाओ fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_कुंजी_आकार);
  अगर(ret)
    जाओ fail;

  ret = wc_ed25519_import_private_कुंजी(कुंजी, ED25519_कुंजी_आकार,
                                      pubkey, ED25519_PUB_कुंजी_आकार, &edkey);
  अगर(ret)
    जाओ fail;

  outlen = ED25519_SIG_आकार;
  ret = wc_ed25519_sign_संदेश(संदेश, (word32)msglen, sig, &outlen, &edkey);
  अगर(ret)
    जाओ fail;

  *siglen = (माप_प्रकार)outlen;
  wc_ed25519_free(&edkey);
  वापस CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  वापस CURLE_AUTH_ERROR;
}

#या_अगर defined(USE_GNUTLS)
#समावेश <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(स्थिर अचिन्हित अक्षर *कुंजी, माप_प्रकार keylen,
                           स्थिर अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  uint8_t pubkey[ED25519_कुंजी_आकार];

  अगर(keylen != ED25519_कुंजी_आकार)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_कुंजी(pubkey, कुंजी);

  nettle_ed25519_sha512_sign(pubkey, कुंजी, msglen, संदेश, sig);
  *siglen = CURL_ED25519_SIGLEN;

  वापस CURLE_OK;
}

#अन्यथा /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(स्थिर अचिन्हित अक्षर *कुंजी, माप_प्रकार keylen,
                           स्थिर अचिन्हित अक्षर *संदेश, माप_प्रकार msglen,
                           अचिन्हित अक्षर *sig, माप_प्रकार *siglen)
{
  (व्योम)कुंजी;
  (व्योम)keylen;
  (व्योम)संदेश;
  (व्योम)msglen;
  (व्योम)sig;
  (व्योम)siglen;
  वापस CURLE_NOT_BUILT_IN;
}

#पूर्ण_अगर /* Ed25519 backends */

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
