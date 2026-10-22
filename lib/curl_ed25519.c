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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ed25519.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WOLFSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/wolfcrypt/settings.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/evp.h>

CURLcode Curl_ed25519_sign(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ, size_t keylen,
                           ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੁਨੇਹਾ, size_t msglen,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  ਪੂਰਨ rc;

  ਜੇ(keylen != CURL_ED25519_KEYLEN)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  pkey = EVP_PKEY_new_raw_ਨਿੱਜੀ_ਕੁੰਜੀ(EVP_PKEY_ED25519, ਨਿਰਾ, ਕੁੰਜੀ, keylen);
  ਜੇ(!pkey)
    ਵਾਪਸ CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  ਜੇ(!mdctx) {
    EVP_PKEY_free(pkey);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  rc = EVP_DigestSignInit(mdctx, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, pkey);
  ਜੇ(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    ਵਾਪਸ CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, ਸੁਨੇਹਾ, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  ਜੇ(rc != 1)
    ਵਾਪਸ CURLE_AUTH_ERROR;

  *siglen = slen;
  ਵਾਪਸ CURLE_OK;
}

#ਜਾਂਜੇ defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_ਕੁੰਜੀ_IMPORT) && defined(HAVE_ED25519_SIGN)
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/wolfcrypt/ed25519.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/wolfcrypt/error-crypt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ, size_t keylen,
                           ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੁਨੇਹਾ, size_t msglen,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sig, size_t *siglen)
{
  ਪੂਰਨ ret;
  ed25519_ਕੁੰਜੀ edkey;
  word32 outlen;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ pubkey[ED25519_PUB_ਕੁੰਜੀ_ਮਾਪ];

  ਜੇ(keylen != ED25519_ਕੁੰਜੀ_ਮਾਪ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ret = wc_ed25519_init(&edkey);
  ਜੇ(ret)
    ਵਾਪਸ CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_ਨਿੱਜੀ_only(ਕੁੰਜੀ, ED25519_ਕੁੰਜੀ_ਮਾਪ, &edkey);
  ਜੇ(ret)
    ਜਾਓ fail;

  ret = wc_ed25519_make_ਜਨਤਕ(&edkey, pubkey, ED25519_PUB_ਕੁੰਜੀ_ਮਾਪ);
  ਜੇ(ret)
    ਜਾਓ fail;

  ret = wc_ed25519_import_ਨਿੱਜੀ_ਕੁੰਜੀ(ਕੁੰਜੀ, ED25519_ਕੁੰਜੀ_ਮਾਪ,
                                      pubkey, ED25519_PUB_ਕੁੰਜੀ_ਮਾਪ, &edkey);
  ਜੇ(ret)
    ਜਾਓ fail;

  outlen = ED25519_SIG_ਮਾਪ;
  ret = wc_ed25519_sign_ਸੁਨੇਹਾ(ਸੁਨੇਹਾ, (word32)msglen, sig, &outlen, &edkey);
  ਜੇ(ret)
    ਜਾਓ fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  ਵਾਪਸ CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  ਵਾਪਸ CURLE_AUTH_ERROR;
}

#ਜਾਂਜੇ defined(USE_GNUTLS)
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ, size_t keylen,
                           ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੁਨੇਹਾ, size_t msglen,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_ਕੁੰਜੀ_ਮਾਪ];

  ਜੇ(keylen != ED25519_ਕੁੰਜੀ_ਮਾਪ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  nettle_ed25519_sha512_ਜਨਤਕ_ਕੁੰਜੀ(pubkey, ਕੁੰਜੀ);

  nettle_ed25519_sha512_sign(pubkey, ਕੁੰਜੀ, msglen, ਸੁਨੇਹਾ, sig);
  *siglen = CURL_ED25519_SIGLEN;

  ਵਾਪਸ CURLE_OK;
}

#ਨਹੀਂਤਾਂ /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ, size_t keylen,
                           ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੁਨੇਹਾ, size_t msglen,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sig, size_t *siglen)
{
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)keylen;
  (ਖਾਲੀ)ਸੁਨੇਹਾ;
  (ਖਾਲੀ)msglen;
  (ਖਾਲੀ)sig;
  (ਖਾਲੀ)siglen;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

#ਜੇ_ਅੰਤ /* Ed25519 backends */

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
