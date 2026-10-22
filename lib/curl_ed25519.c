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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#ಅನ್ತರ್ಭಾವ "curl_ed25519.h"

#ifdef USE_WOLFSSL
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

CURLcode Curl_ed25519_sign(ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ, size_t keylen,
                           ಧ್ರುವ unsigned ವರ್ಣ *ಸಂದೇಶ, size_t msglen,
                           unsigned ವರ್ಣ *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(keylen != CURL_ED25519_KEYLEN)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_ಕೀಲಿ(EVP_PKEY_ED25519, ರಿಕ್ತ, ಕೀಲಿ, keylen);
  ಯದಿ(!pkey)
    ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  ಯದಿ(!mdctx) {
    EVP_PKEY_free(pkey);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, pkey);
  ಯದಿ(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, ಸಂದೇಶ, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  ಯದಿ(rc != 1)
    ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;

  *siglen = slen;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_ಕೀಲಿ_IMPORT) && defined(HAVE_ED25519_SIGN)
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/ed25519.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/error-crypt.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ, size_t keylen,
                           ಧ್ರುವ unsigned ವರ್ಣ *ಸಂದೇಶ, size_t msglen,
                           unsigned ವರ್ಣ *sig, size_t *siglen)
{
  ಪೂರ್ಣಾಙ್ಕ ret;
  ed25519_ಕೀಲಿ edkey;
  word32 outlen;
  unsigned ವರ್ಣ pubkey[ED25519_PUB_ಕೀಲಿ_SIZE];

  ಯದಿ(keylen != ED25519_ಕೀಲಿ_SIZE)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  ಯದಿ(ret)
    ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(ಕೀಲಿ, ED25519_ಕೀಲಿ_SIZE, &edkey);
  ಯದಿ(ret)
    ಗಚ್ಛ fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_ಕೀಲಿ_SIZE);
  ಯದಿ(ret)
    ಗಚ್ಛ fail;

  ret = wc_ed25519_import_private_ಕೀಲಿ(ಕೀಲಿ, ED25519_ಕೀಲಿ_SIZE,
                                      pubkey, ED25519_PUB_ಕೀಲಿ_SIZE, &edkey);
  ಯದಿ(ret)
    ಗಚ್ಛ fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_ಸಂದೇಶ(ಸಂದೇಶ, (word32)msglen, sig, &outlen, &edkey);
  ಯದಿ(ret)
    ಗಚ್ಛ fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  ಪ್ರತ್ಯಾಗಮ CURLE_AUTH_ERROR;
}

#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ, size_t keylen,
                           ಧ್ರುವ unsigned ವರ್ಣ *ಸಂದೇಶ, size_t msglen,
                           unsigned ವರ್ಣ *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_ಕೀಲಿ_SIZE];

  ಯದಿ(keylen != ED25519_ಕೀಲಿ_SIZE)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_ಕೀಲಿ(pubkey, ಕೀಲಿ);

  nettle_ed25519_sha512_sign(pubkey, ಕೀಲಿ, msglen, ಸಂದೇಶ, sig);
  *siglen = CURL_ED25519_SIGLEN;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಅನ್ಯಥಾ /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ, size_t keylen,
                           ಧ್ರುವ unsigned ವರ್ಣ *ಸಂದೇಶ, size_t msglen,
                           unsigned ವರ್ಣ *sig, size_t *siglen)
{
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)keylen;
  (ಶೂನ್ಯ)ಸಂದೇಶ;
  (ಶೂನ್ಯ)msglen;
  (ಶೂನ್ಯ)sig;
  (ಶೂನ್ಯ)siglen;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
