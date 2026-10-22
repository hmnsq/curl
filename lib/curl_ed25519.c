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
#ضمن "curl_setup.h"

#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#ضمن "curl_ed25519.h"

#ifdef USE_WOLFSSL
#ضمن <wolfssl/options.h>
#ضمن <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#ضمن <openssl/evp.h>

CURLcode Curl_ed25519_sign(ثابت unsigned حرف *key, size_t keylen,
                           ثابت unsigned حرف *msg, size_t msglen,
                           unsigned حرف *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  صحيح rc;

  אם(keylen != CURL_ED25519_KEYLEN)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, عدم, key, keylen);
  אם(!pkey)
    החזר CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  אם(!mdctx) {
    EVP_PKEY_free(pkey);
    החזר CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, عدم, عدم, عدم, pkey);
  אם(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    החזר CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, msg, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  אם(rc != 1)
    החזר CURLE_AUTH_ERROR;

  *siglen = slen;
  החזר CURLE_OK;
}

#وإلاإذا defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_KEY_IMPORT) && defined(HAVE_ED25519_SIGN)
#ضمن <wolfssl/wolfcrypt/ed25519.h>
#ضمن <wolfssl/wolfcrypt/error-crypt.h>
#ضمن <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ثابت unsigned حرف *key, size_t keylen,
                           ثابت unsigned حرف *msg, size_t msglen,
                           unsigned حرف *sig, size_t *siglen)
{
  صحيح ret;
  ed25519_key edkey;
  word32 outlen;
  unsigned حرف pubkey[ED25519_PUB_KEY_SIZE];

  אם(keylen != ED25519_KEY_SIZE)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  אם(ret)
    החזר CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(key, ED25519_KEY_SIZE, &edkey);
  אם(ret)
    اذهب fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_KEY_SIZE);
  אם(ret)
    اذهب fail;

  ret = wc_ed25519_import_private_key(key, ED25519_KEY_SIZE,
                                      pubkey, ED25519_PUB_KEY_SIZE, &edkey);
  אם(ret)
    اذهب fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_msg(msg, (word32)msglen, sig, &outlen, &edkey);
  אם(ret)
    اذهب fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  החזר CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  החזר CURLE_AUTH_ERROR;
}

#وإلاإذا defined(USE_GNUTLS)
#ضمن <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ثابت unsigned حرف *key, size_t keylen,
                           ثابت unsigned حرف *msg, size_t msglen,
                           unsigned حرف *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_KEY_SIZE];

  אם(keylen != ED25519_KEY_SIZE)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_key(pubkey, key);

  nettle_ed25519_sha512_sign(pubkey, key, msglen, msg, sig);
  *siglen = CURL_ED25519_SIGLEN;

  החזר CURLE_OK;
}

#אחרת /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ثابت unsigned حرف *key, size_t keylen,
                           ثابت unsigned حرف *msg, size_t msglen,
                           unsigned حرف *sig, size_t *siglen)
{
  (فراغ)key;
  (فراغ)keylen;
  (فراغ)msg;
  (فراغ)msglen;
  (فراغ)sig;
  (فراغ)siglen;
  החזר CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
