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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#અન્તર્ભાવ "curl_ed25519.h"

#ifdef USE_WOLFSSL
#અન્તર્ભાવ <wolfssl/options.h>
#અન્તર્ભાવ <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#અન્તર્ભાવ <openssl/evp.h>

CURLcode Curl_ed25519_sign(અચલ unsigned અક્ષર *ચાવી, size_t keylen,
                           અચલ unsigned અક્ષર *સંદેશ, size_t msglen,
                           unsigned અક્ષર *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  પૂર્ણાંક rc;

  જો(keylen != CURL_ED25519_KEYLEN)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_ચાવી(EVP_PKEY_ED25519, ખાલી, ચાવી, keylen);
  જો(!pkey)
    પાછા CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  જો(!mdctx) {
    EVP_PKEY_free(pkey);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, ખાલી, ખાલી, ખાલી, pkey);
  જો(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    પાછા CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, સંદેશ, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  જો(rc != 1)
    પાછા CURLE_AUTH_ERROR;

  *siglen = slen;
  પાછા CURLE_OK;
}

#અથવાજો defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_ચાવી_IMPORT) && defined(HAVE_ED25519_SIGN)
#અન્તર્ભાવ <wolfssl/wolfcrypt/ed25519.h>
#અન્તર્ભાવ <wolfssl/wolfcrypt/error-crypt.h>
#અન્તર્ભાવ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(અચલ unsigned અક્ષર *ચાવી, size_t keylen,
                           અચલ unsigned અક્ષર *સંદેશ, size_t msglen,
                           unsigned અક્ષર *sig, size_t *siglen)
{
  પૂર્ણાંક ret;
  ed25519_ચાવી edkey;
  word32 outlen;
  unsigned અક્ષર pubkey[ED25519_PUB_ચાવી_SIZE];

  જો(keylen != ED25519_ચાવી_SIZE)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  જો(ret)
    પાછા CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(ચાવી, ED25519_ચાવી_SIZE, &edkey);
  જો(ret)
    જાઓ fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_ચાવી_SIZE);
  જો(ret)
    જાઓ fail;

  ret = wc_ed25519_import_private_ચાવી(ચાવી, ED25519_ચાવી_SIZE,
                                      pubkey, ED25519_PUB_ચાવી_SIZE, &edkey);
  જો(ret)
    જાઓ fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_સંદેશ(સંદેશ, (word32)msglen, sig, &outlen, &edkey);
  જો(ret)
    જાઓ fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  પાછા CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  પાછા CURLE_AUTH_ERROR;
}

#અથવાજો defined(USE_GNUTLS)
#અન્તર્ભાવ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(અચલ unsigned અક્ષર *ચાવી, size_t keylen,
                           અચલ unsigned અક્ષર *સંદેશ, size_t msglen,
                           unsigned અક્ષર *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_ચાવી_SIZE];

  જો(keylen != ED25519_ચાવી_SIZE)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_ચાવી(pubkey, ચાવી);

  nettle_ed25519_sha512_sign(pubkey, ચાવી, msglen, સંદેશ, sig);
  *siglen = CURL_ED25519_SIGLEN;

  પાછા CURLE_OK;
}

#નહીંતર /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(અચલ unsigned અક્ષર *ચાવી, size_t keylen,
                           અચલ unsigned અક્ષર *સંદેશ, size_t msglen,
                           unsigned અક્ષર *sig, size_t *siglen)
{
  (શૂન્ય)ચાવી;
  (શૂન્ય)keylen;
  (શૂન્ય)સંદેશ;
  (શૂન્ય)msglen;
  (શૂન્ય)sig;
  (શૂન્ય)siglen;
  પાછા CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
