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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#అన్తర్భావ "curl_ed25519.h"

#నిర్వచితమైతే USE_WOLFSSL
#అన్తర్భావ <wolfssl/options.h>
#అన్తర్భావ <wolfssl/wolfcrypt/settings.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_OPENSSL
#అన్తర్భావ <openssl/evp.h>

CURLcode Curl_ed25519_sign(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, size_t keylen,
                           స్థిరరాశి గుర్తులేని అక్షరం *సందేశం, size_t msglen,
                           గుర్తులేని అక్షరం *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  పూర్ణాంకం rc;

  ఒకవేళ(keylen != CURL_ED25519_KEYLEN)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  pkey = EVP_PKEY_new_raw_వ్యక్తిగత_కీలకం(EVP_PKEY_ED25519, ఖాళీ, కీలకం, keylen);
  ఒకవేళ(!pkey)
    తిరిగి CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  ఒకవేళ(!mdctx) {
    EVP_PKEY_free(pkey);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  rc = EVP_DigestSignInit(mdctx, ఖాళీ, ఖాళీ, ఖాళీ, pkey);
  ఒకవేళ(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    తిరిగి CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, సందేశం, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  ఒకవేళ(rc != 1)
    తిరిగి CURLE_AUTH_ERROR;

  *siglen = slen;
  తిరిగి CURLE_OK;
}

#లేదావేళ defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_కీలకం_IMPORT) && defined(HAVE_ED25519_SIGN)
#అన్తర్భావ <wolfssl/wolfcrypt/ed25519.h>
#అన్తర్భావ <wolfssl/wolfcrypt/error-crypt.h>
#అన్తర్భావ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, size_t keylen,
                           స్థిరరాశి గుర్తులేని అక్షరం *సందేశం, size_t msglen,
                           గుర్తులేని అక్షరం *sig, size_t *siglen)
{
  పూర్ణాంకం ret;
  ed25519_కీలకం edkey;
  word32 outlen;
  గుర్తులేని అక్షరం pubkey[ED25519_PUB_కీలకం_కొలత];

  ఒకవేళ(keylen != ED25519_కీలకం_కొలత)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ret = wc_ed25519_init(&edkey);
  ఒకవేళ(ret)
    తిరిగి CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_వ్యక్తిగత_only(కీలకం, ED25519_కీలకం_కొలత, &edkey);
  ఒకవేళ(ret)
    వెళ్ళు fail;

  ret = wc_ed25519_make_ప్రజా(&edkey, pubkey, ED25519_PUB_కీలకం_కొలత);
  ఒకవేళ(ret)
    వెళ్ళు fail;

  ret = wc_ed25519_import_వ్యక్తిగత_కీలకం(కీలకం, ED25519_కీలకం_కొలత,
                                      pubkey, ED25519_PUB_కీలకం_కొలత, &edkey);
  ఒకవేళ(ret)
    వెళ్ళు fail;

  outlen = ED25519_SIG_కొలత;
  ret = wc_ed25519_sign_సందేశం(సందేశం, (word32)msglen, sig, &outlen, &edkey);
  ఒకవేళ(ret)
    వెళ్ళు fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  తిరిగి CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  తిరిగి CURLE_AUTH_ERROR;
}

#లేదావేళ defined(USE_GNUTLS)
#అన్తర్భావ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, size_t keylen,
                           స్థిరరాశి గుర్తులేని అక్షరం *సందేశం, size_t msglen,
                           గుర్తులేని అక్షరం *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_కీలకం_కొలత];

  ఒకవేళ(keylen != ED25519_కీలకం_కొలత)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  nettle_ed25519_sha512_ప్రజా_కీలకం(pubkey, కీలకం);

  nettle_ed25519_sha512_sign(pubkey, కీలకం, msglen, సందేశం, sig);
  *siglen = CURL_ED25519_SIGLEN;

  తిరిగి CURLE_OK;
}

#లేకపోతే /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, size_t keylen,
                           స్థిరరాశి గుర్తులేని అక్షరం *సందేశం, size_t msglen,
                           గుర్తులేని అక్షరం *sig, size_t *siglen)
{
  (శూన్య)కీలకం;
  (శూన్య)keylen;
  (శూన్య)సందేశం;
  (శూన్య)msglen;
  (శూన్య)sig;
  (శూన్య)siglen;
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

#అయితే_ముగింపు /* Ed25519 backends */

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
