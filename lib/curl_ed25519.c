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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#அந்தர்பாவ "curl_ed25519.h"

#ifdef USE_WOLFSSL
#அந்தர்பாவ <wolfssl/options.h>
#அந்தர்பாவ <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#அந்தர்பாவ <openssl/evp.h>

CURLcode Curl_ed25519_sign(மாறிலி unsigned எழுத்து *சாவி, size_t keylen,
                           மாறிலி unsigned எழுத்து *செய்தி, size_t msglen,
                           unsigned எழுத்து *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  முழு rc;

  எனில்(keylen != CURL_ED25519_KEYLEN)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_சாவி(EVP_PKEY_ED25519, வெற்று, சாவி, keylen);
  எனில்(!pkey)
    திரும்பு CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  எனில்(!mdctx) {
    EVP_PKEY_free(pkey);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, வெற்று, வெற்று, வெற்று, pkey);
  எனில்(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    திரும்பு CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, செய்தி, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  எனில்(rc != 1)
    திரும்பு CURLE_AUTH_ERROR;

  *siglen = slen;
  திரும்பு CURLE_OK;
}

#அல்லதெனில் defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_சாவி_IMPORT) && defined(HAVE_ED25519_SIGN)
#அந்தர்பாவ <wolfssl/wolfcrypt/ed25519.h>
#அந்தர்பாவ <wolfssl/wolfcrypt/error-crypt.h>
#அந்தர்பாவ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(மாறிலி unsigned எழுத்து *சாவி, size_t keylen,
                           மாறிலி unsigned எழுத்து *செய்தி, size_t msglen,
                           unsigned எழுத்து *sig, size_t *siglen)
{
  முழு ret;
  ed25519_சாவி edkey;
  word32 outlen;
  unsigned எழுத்து pubkey[ED25519_PUB_சாவி_SIZE];

  எனில்(keylen != ED25519_சாவி_SIZE)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  எனில்(ret)
    திரும்பு CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(சாவி, ED25519_சாவி_SIZE, &edkey);
  எனில்(ret)
    செல் fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_சாவி_SIZE);
  எனில்(ret)
    செல் fail;

  ret = wc_ed25519_import_private_சாவி(சாவி, ED25519_சாவி_SIZE,
                                      pubkey, ED25519_PUB_சாவி_SIZE, &edkey);
  எனில்(ret)
    செல் fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_செய்தி(செய்தி, (word32)msglen, sig, &outlen, &edkey);
  எனில்(ret)
    செல் fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  திரும்பு CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  திரும்பு CURLE_AUTH_ERROR;
}

#அல்லதெனில் defined(USE_GNUTLS)
#அந்தர்பாவ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(மாறிலி unsigned எழுத்து *சாவி, size_t keylen,
                           மாறிலி unsigned எழுத்து *செய்தி, size_t msglen,
                           unsigned எழுத்து *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_சாவி_SIZE];

  எனில்(keylen != ED25519_சாவி_SIZE)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_சாவி(pubkey, சாவி);

  nettle_ed25519_sha512_sign(pubkey, சாவி, msglen, செய்தி, sig);
  *siglen = CURL_ED25519_SIGLEN;

  திரும்பு CURLE_OK;
}

#இல்லையெனில் /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(மாறிலி unsigned எழுத்து *சாவி, size_t keylen,
                           மாறிலி unsigned எழுத்து *செய்தி, size_t msglen,
                           unsigned எழுத்து *sig, size_t *siglen)
{
  (வெறுமை)சாவி;
  (வெறுமை)keylen;
  (வெறுமை)செய்தி;
  (வெறுமை)msglen;
  (வெறுமை)sig;
  (வெறுமை)siglen;
  திரும்பு CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
