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

#اگر_تعریف_شدہ USE_WOLFSSL
#شامل <wolfssl/options.h>
#شامل <wolfssl/wolfcrypt/settings.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/evp.h>

CURLcode Curl_ed25519_sign(ثابت غیرعلامتی حرف *کلید, size_t keylen,
                           ثابت غیرعلامتی حرف *پیغام, size_t msglen,
                           غیرعلامتی حرف *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  صحیح rc;

  اگر(keylen != CURL_ED25519_KEYLEN)
    واپس CURLE_BAD_FUNCTION_دلیل;

  pkey = EVP_PKEY_new_raw_نجی_کلید(EVP_PKEY_ED25519, تہی, کلید, keylen);
  اگر(!pkey)
    واپس CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  اگر(!mdctx) {
    EVP_PKEY_free(pkey);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  rc = EVP_DigestSignInit(mdctx, تہی, تہی, تہی, pkey);
  اگر(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    واپس CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, پیغام, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  اگر(rc != 1)
    واپس CURLE_AUTH_ERROR;

  *siglen = slen;
  واپس CURLE_OK;
}

#وگرنہ_اگر defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_کلید_IMPORT) && defined(HAVE_ED25519_SIGN)
#شامل <wolfssl/wolfcrypt/ed25519.h>
#شامل <wolfssl/wolfcrypt/error-crypt.h>
#شامل <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ثابت غیرعلامتی حرف *کلید, size_t keylen,
                           ثابت غیرعلامتی حرف *پیغام, size_t msglen,
                           غیرعلامتی حرف *sig, size_t *siglen)
{
  صحیح ret;
  ed25519_کلید edkey;
  word32 outlen;
  غیرعلامتی حرف pubkey[ED25519_PUB_کلید_سائز];

  اگر(keylen != ED25519_کلید_سائز)
    واپس CURLE_BAD_FUNCTION_دلیل;

  ret = wc_ed25519_init(&edkey);
  اگر(ret)
    واپس CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_نجی_only(کلید, ED25519_کلید_سائز, &edkey);
  اگر(ret)
    جاؤ fail;

  ret = wc_ed25519_make_عوامی(&edkey, pubkey, ED25519_PUB_کلید_سائز);
  اگر(ret)
    جاؤ fail;

  ret = wc_ed25519_import_نجی_کلید(کلید, ED25519_کلید_سائز,
                                      pubkey, ED25519_PUB_کلید_سائز, &edkey);
  اگر(ret)
    جاؤ fail;

  outlen = ED25519_SIG_سائز;
  ret = wc_ed25519_sign_پیغام(پیغام, (word32)msglen, sig, &outlen, &edkey);
  اگر(ret)
    جاؤ fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  واپس CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  واپس CURLE_AUTH_ERROR;
}

#وگرنہ_اگر defined(USE_GNUTLS)
#شامل <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ثابت غیرعلامتی حرف *کلید, size_t keylen,
                           ثابت غیرعلامتی حرف *پیغام, size_t msglen,
                           غیرعلامتی حرف *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_کلید_سائز];

  اگر(keylen != ED25519_کلید_سائز)
    واپس CURLE_BAD_FUNCTION_دلیل;

  nettle_ed25519_sha512_عوامی_کلید(pubkey, کلید);

  nettle_ed25519_sha512_sign(pubkey, کلید, msglen, پیغام, sig);
  *siglen = CURL_ED25519_SIGLEN;

  واپس CURLE_OK;
}

#ورنہ /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ثابت غیرعلامتی حرف *کلید, size_t keylen,
                           ثابت غیرعلامتی حرف *پیغام, size_t msglen,
                           غیرعلامتی حرف *sig, size_t *siglen)
{
  (خالی)کلید;
  (خالی)keylen;
  (خالی)پیغام;
  (خالی)msglen;
  (خالی)sig;
  (خالی)siglen;
  واپس CURLE_NOT_BUILT_اندر;
}

#اگر_ختم /* Ed25519 backends */

#اگر_ختم /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
