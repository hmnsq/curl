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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#অন্তর্ভাৱ "curl_ed25519.h"

#ifdef USE_WOLFSSL
#অন্তর্ভাৱ <wolfssl/options.h>
#অন্তর্ভাৱ <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#অন্তর্ভাৱ <openssl/evp.h>

CURLcode Curl_ed25519_sign(ধ্রুৱ unsigned ৱর্ণ *চাবি, size_t keylen,
                           ধ্রুৱ unsigned ৱর্ণ *বাৰ্তা, size_t msglen,
                           unsigned ৱর্ণ *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  পূর্ণাঙ্ক rc;

  যদি(keylen != CURL_ED25519_KEYLEN)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_চাবি(EVP_PKEY_ED25519, রিক্ত, চাবি, keylen);
  যদি(!pkey)
    ঘূৰাই CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  যদি(!mdctx) {
    EVP_PKEY_free(pkey);
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, রিক্ত, রিক্ত, রিক্ত, pkey);
  যদি(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    ঘূৰাই CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, বাৰ্তা, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  যদি(rc != 1)
    ঘূৰাই CURLE_AUTH_ERROR;

  *siglen = slen;
  ঘূৰাই CURLE_OK;
}

#নোচেদ্যদি defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_চাবি_IMPORT) && defined(HAVE_ED25519_SIGN)
#অন্তর্ভাৱ <wolfssl/wolfcrypt/ed25519.h>
#অন্তর্ভাৱ <wolfssl/wolfcrypt/error-crypt.h>
#অন্তর্ভাৱ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ধ্রুৱ unsigned ৱর্ণ *চাবি, size_t keylen,
                           ধ্রুৱ unsigned ৱর্ণ *বাৰ্তা, size_t msglen,
                           unsigned ৱর্ণ *sig, size_t *siglen)
{
  পূর্ণাঙ্ক ret;
  ed25519_চাবি edkey;
  word32 outlen;
  unsigned ৱর্ণ pubkey[ED25519_PUB_চাবি_SIZE];

  যদি(keylen != ED25519_চাবি_SIZE)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  যদি(ret)
    ঘূৰাই CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(চাবি, ED25519_চাবি_SIZE, &edkey);
  যদি(ret)
    গচ্ছ fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_চাবি_SIZE);
  যদি(ret)
    গচ্ছ fail;

  ret = wc_ed25519_import_private_চাবি(চাবি, ED25519_চাবি_SIZE,
                                      pubkey, ED25519_PUB_চাবি_SIZE, &edkey);
  যদি(ret)
    গচ্ছ fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_বাৰ্তা(বাৰ্তা, (word32)msglen, sig, &outlen, &edkey);
  যদি(ret)
    গচ্ছ fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  ঘূৰাই CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  ঘূৰাই CURLE_AUTH_ERROR;
}

#নোচেদ্যদি defined(USE_GNUTLS)
#অন্তর্ভাৱ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ধ্রুৱ unsigned ৱর্ণ *চাবি, size_t keylen,
                           ধ্রুৱ unsigned ৱর্ণ *বাৰ্তা, size_t msglen,
                           unsigned ৱর্ণ *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_চাবি_SIZE];

  যদি(keylen != ED25519_চাবি_SIZE)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_চাবি(pubkey, চাবি);

  nettle_ed25519_sha512_sign(pubkey, চাবি, msglen, বাৰ্তা, sig);
  *siglen = CURL_ED25519_SIGLEN;

  ঘূৰাই CURLE_OK;
}

#অন্যথা /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ধ্রুৱ unsigned ৱর্ণ *চাবি, size_t keylen,
                           ধ্রুৱ unsigned ৱর্ণ *বাৰ্তা, size_t msglen,
                           unsigned ৱর্ণ *sig, size_t *siglen)
{
  (শূন্য)চাবি;
  (শূন্য)keylen;
  (শূন্য)বাৰ্তা;
  (শূন্য)msglen;
  (শূন্য)sig;
  (শূন্য)siglen;
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
