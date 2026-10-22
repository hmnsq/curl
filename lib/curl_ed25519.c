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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#ಅನ್ತರ್ಭಾವ "curl_ed25519.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WOLFSSL
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/settings.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

CURLcode Curl_ed25519_sign(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ, size_t keylen,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸಂದೇಶ, size_t msglen,
                           ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(keylen != CURL_ED25519_KEYLEN)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  pkey = EVP_PKEY_new_raw_ಖಾಸಗಿ_ಕೀಲಿ(EVP_PKEY_ED25519, ಖಾಲಿ, ಕೀಲಿ, keylen);
  ಆದರೆ(!pkey)
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  ಆದರೆ(!mdctx) {
    EVP_PKEY_free(pkey);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  rc = EVP_DigestSignInit(mdctx, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, pkey);
  ಆದರೆ(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, ಸಂದೇಶ, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  ಆದರೆ(rc != 1)
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;

  *siglen = slen;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಅಥವಾಆದರೆ defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_ಕೀಲಿ_IMPORT) && defined(HAVE_ED25519_SIGN)
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/ed25519.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/error-crypt.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ, size_t keylen,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸಂದೇಶ, size_t msglen,
                           ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sig, size_t *siglen)
{
  ಪೂರ್ಣಾಂಕ ret;
  ed25519_ಕೀಲಿ edkey;
  word32 outlen;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ pubkey[ED25519_PUB_ಕೀಲಿ_ಗಾತ್ರ];

  ಆದರೆ(keylen != ED25519_ಕೀಲಿ_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ret = wc_ed25519_init(&edkey);
  ಆದರೆ(ret)
    ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_ಖಾಸಗಿ_only(ಕೀಲಿ, ED25519_ಕೀಲಿ_ಗಾತ್ರ, &edkey);
  ಆದರೆ(ret)
    ಹೋಗು fail;

  ret = wc_ed25519_make_ಬಹಿರಂಗ(&edkey, pubkey, ED25519_PUB_ಕೀಲಿ_ಗಾತ್ರ);
  ಆದರೆ(ret)
    ಹೋಗು fail;

  ret = wc_ed25519_import_ಖಾಸಗಿ_ಕೀಲಿ(ಕೀಲಿ, ED25519_ಕೀಲಿ_ಗಾತ್ರ,
                                      pubkey, ED25519_PUB_ಕೀಲಿ_ಗಾತ್ರ, &edkey);
  ಆದರೆ(ret)
    ಹೋಗು fail;

  outlen = ED25519_SIG_ಗಾತ್ರ;
  ret = wc_ed25519_sign_ಸಂದೇಶ(ಸಂದೇಶ, (word32)msglen, sig, &outlen, &edkey);
  ಆದರೆ(ret)
    ಹೋಗು fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  ಹಿಂತಿರುಗಿ CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  ಹಿಂತಿರುಗಿ CURLE_AUTH_ERROR;
}

#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ, size_t keylen,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸಂದೇಶ, size_t msglen,
                           ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_ಕೀಲಿ_ಗಾತ್ರ];

  ಆದರೆ(keylen != ED25519_ಕೀಲಿ_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  nettle_ed25519_sha512_ಬಹಿರಂಗ_ಕೀಲಿ(pubkey, ಕೀಲಿ);

  nettle_ed25519_sha512_sign(pubkey, ಕೀಲಿ, msglen, ಸಂದೇಶ, sig);
  *siglen = CURL_ED25519_SIGLEN;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಇಲ್ಲದಿದ್ದರೆ /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ, size_t keylen,
                           ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸಂದೇಶ, size_t msglen,
                           ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sig, size_t *siglen)
{
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)keylen;
  (ಶೂನ್ಯ)ಸಂದೇಶ;
  (ಶೂನ್ಯ)msglen;
  (ಶೂನ್ಯ)sig;
  (ಶೂನ್ಯ)siglen;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

#ಆದರೆ_ಅಂತ್ಯ /* Ed25519 backends */

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
