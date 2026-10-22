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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#شامل "curl_ed25519.h"

#ifdef USE_WOLFSSL
#شامل <wolfssl/options.h>
#شامل <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#شامل <openssl/evp.h>

CURLcode Curl_ed25519_sign(ثابت unsigned نویسه *کلید, size_t keylen,
                           ثابت unsigned نویسه *پیام, size_t msglen,
                           unsigned نویسه *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  صحیح rc;

  اگر(keylen != CURL_ED25519_KEYLEN)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_کلید(EVP_PKEY_ED25519, پوچ, کلید, keylen);
  اگر(!pkey)
    بازگشت CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  اگر(!mdctx) {
    EVP_PKEY_free(pkey);
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, پوچ, پوچ, پوچ, pkey);
  اگر(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    بازگشت CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, پیام, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  اگر(rc != 1)
    بازگشت CURLE_AUTH_ERROR;

  *siglen = slen;
  بازگشت CURLE_OK;
}

#وگرنهاگر defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_کلید_IMPORT) && defined(HAVE_ED25519_SIGN)
#شامل <wolfssl/wolfcrypt/ed25519.h>
#شامل <wolfssl/wolfcrypt/error-crypt.h>
#شامل <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ثابت unsigned نویسه *کلید, size_t keylen,
                           ثابت unsigned نویسه *پیام, size_t msglen,
                           unsigned نویسه *sig, size_t *siglen)
{
  صحیح ret;
  ed25519_کلید edkey;
  word32 outlen;
  unsigned نویسه pubkey[ED25519_PUB_کلید_SIZE];

  اگر(keylen != ED25519_کلید_SIZE)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  اگر(ret)
    بازگشت CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(کلید, ED25519_کلید_SIZE, &edkey);
  اگر(ret)
    برو fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_کلید_SIZE);
  اگر(ret)
    برو fail;

  ret = wc_ed25519_import_private_کلید(کلید, ED25519_کلید_SIZE,
                                      pubkey, ED25519_PUB_کلید_SIZE, &edkey);
  اگر(ret)
    برو fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_پیام(پیام, (word32)msglen, sig, &outlen, &edkey);
  اگر(ret)
    برو fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  بازگشت CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  بازگشت CURLE_AUTH_ERROR;
}

#وگرنهاگر defined(USE_GNUTLS)
#شامل <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ثابت unsigned نویسه *کلید, size_t keylen,
                           ثابت unsigned نویسه *پیام, size_t msglen,
                           unsigned نویسه *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_کلید_SIZE];

  اگر(keylen != ED25519_کلید_SIZE)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_کلید(pubkey, کلید);

  nettle_ed25519_sha512_sign(pubkey, کلید, msglen, پیام, sig);
  *siglen = CURL_ED25519_SIGLEN;

  بازگشت CURLE_OK;
}

#وگرنه /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ثابت unsigned نویسه *کلید, size_t keylen,
                           ثابت unsigned نویسه *پیام, size_t msglen,
                           unsigned نویسه *sig, size_t *siglen)
{
  (تهی)کلید;
  (تهی)keylen;
  (تهی)پیام;
  (تهی)msglen;
  (تهی)sig;
  (تهی)siglen;
  بازگشت CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
