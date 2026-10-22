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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#ଅନ୍ତର୍ଭାଵ "curl_ed25519.h"

#ifdef USE_WOLFSSL
#ଅନ୍ତର୍ଭାଵ <wolfssl/options.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#ଅନ୍ତର୍ଭାଵ <openssl/evp.h>

CURLcode Curl_ed25519_sign(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି, size_t keylen,
                           ଅଚଳ unsigned ଅକ୍ଷର *ସନ୍ଦେଶ, size_t msglen,
                           unsigned ଅକ୍ଷର *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(keylen != CURL_ED25519_KEYLEN)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_ଚାବି(EVP_PKEY_ED25519, ଖାଲି, ଚାବି, keylen);
  ଯଦି(!pkey)
    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  ଯଦି(!mdctx) {
    EVP_PKEY_free(pkey);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, ଖାଲି, ଖାଲି, ଖାଲି, pkey);
  ଯଦି(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, ସନ୍ଦେଶ, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  ଯଦି(rc != 1)
    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;

  *siglen = slen;
  ଫେରନ୍ତୁ CURLE_OK;
}

#ନଚେତଯଦି defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_ଚାବି_IMPORT) && defined(HAVE_ED25519_SIGN)
#ଅନ୍ତର୍ଭାଵ <wolfssl/wolfcrypt/ed25519.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/wolfcrypt/error-crypt.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି, size_t keylen,
                           ଅଚଳ unsigned ଅକ୍ଷର *ସନ୍ଦେଶ, size_t msglen,
                           unsigned ଅକ୍ଷର *sig, size_t *siglen)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
  ed25519_ଚାବି edkey;
  word32 outlen;
  unsigned ଅକ୍ଷର pubkey[ED25519_PUB_ଚାବି_SIZE];

  ଯଦି(keylen != ED25519_ଚାବି_SIZE)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  ଯଦି(ret)
    ଫେରନ୍ତୁ CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(ଚାବି, ED25519_ଚାବି_SIZE, &edkey);
  ଯଦି(ret)
    ଯାଅ fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_ଚାବି_SIZE);
  ଯଦି(ret)
    ଯାଅ fail;

  ret = wc_ed25519_import_private_ଚାବି(ଚାବି, ED25519_ଚାବି_SIZE,
                                      pubkey, ED25519_PUB_ଚାବି_SIZE, &edkey);
  ଯଦି(ret)
    ଯାଅ fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_ସନ୍ଦେଶ(ସନ୍ଦେଶ, (word32)msglen, sig, &outlen, &edkey);
  ଯଦି(ret)
    ଯାଅ fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  ଫେରନ୍ତୁ CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  ଫେରନ୍ତୁ CURLE_AUTH_ERROR;
}

#ନଚେତଯଦି defined(USE_GNUTLS)
#ଅନ୍ତର୍ଭାଵ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି, size_t keylen,
                           ଅଚଳ unsigned ଅକ୍ଷର *ସନ୍ଦେଶ, size_t msglen,
                           unsigned ଅକ୍ଷର *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_ଚାବି_SIZE];

  ଯଦି(keylen != ED25519_ଚାବି_SIZE)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_ଚାବି(pubkey, ଚାବି);

  nettle_ed25519_sha512_sign(pubkey, ଚାବି, msglen, ସନ୍ଦେଶ, sig);
  *siglen = CURL_ED25519_SIGLEN;

  ଫେରନ୍ତୁ CURLE_OK;
}

#ଅନ୍ୟଥା /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି, size_t keylen,
                           ଅଚଳ unsigned ଅକ୍ଷର *ସନ୍ଦେଶ, size_t msglen,
                           unsigned ଅକ୍ଷର *sig, size_t *siglen)
{
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)keylen;
  (ଶୂନ୍ୟ)ସନ୍ଦେଶ;
  (ଶୂନ୍ୟ)msglen;
  (ଶୂନ୍ୟ)sig;
  (ଶୂନ୍ୟ)siglen;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
