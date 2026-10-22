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
#अन्तर्भाव "curl_setup.h"

#जइ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#अन्तर्भाव "curl_ed25519.h"

#ifdef USE_WOLFSSL
#अन्तर्भाव <wolfssl/options.h>
#अन्तर्भाव <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#अन्तर्भाव <openssl/evp.h>

CURLcode Curl_ed25519_sign(ध्रुव unsigned वर्ण *कुञ्चिका, size_t keylen,
                           ध्रुव unsigned वर्ण *सन्देशः, size_t msglen,
                           unsigned वर्ण *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  पूर्णाङ्क rc;

  जइ(keylen != CURL_ED25519_KEYLEN)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_कुञ्चिका(EVP_PKEY_ED25519, रिक्त, कुञ्चिका, keylen);
  जइ(!pkey)
    प्रत्यागम CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  जइ(!mdctx) {
    EVP_PKEY_free(pkey);
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, रिक्त, रिक्त, रिक्त, pkey);
  जइ(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    प्रत्यागम CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, सन्देशः, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  जइ(rc != 1)
    प्रत्यागम CURLE_AUTH_ERROR;

  *siglen = slen;
  प्रत्यागम CURLE_OK;
}

#नोचेद्यदि defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_कुञ्चिका_IMPORT) && defined(HAVE_ED25519_SIGN)
#अन्तर्भाव <wolfssl/wolfcrypt/ed25519.h>
#अन्तर्भाव <wolfssl/wolfcrypt/error-crypt.h>
#अन्तर्भाव <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(ध्रुव unsigned वर्ण *कुञ्चिका, size_t keylen,
                           ध्रुव unsigned वर्ण *सन्देशः, size_t msglen,
                           unsigned वर्ण *sig, size_t *siglen)
{
  पूर्णाङ्क ret;
  ed25519_कुञ्चिका edkey;
  word32 outlen;
  unsigned वर्ण pubkey[ED25519_PUB_कुञ्चिका_SIZE];

  जइ(keylen != ED25519_कुञ्चिका_SIZE)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  जइ(ret)
    प्रत्यागम CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(कुञ्चिका, ED25519_कुञ्चिका_SIZE, &edkey);
  जइ(ret)
    गच्छ fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_कुञ्चिका_SIZE);
  जइ(ret)
    गच्छ fail;

  ret = wc_ed25519_import_private_कुञ्चिका(कुञ्चिका, ED25519_कुञ्चिका_SIZE,
                                      pubkey, ED25519_PUB_कुञ्चिका_SIZE, &edkey);
  जइ(ret)
    गच्छ fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_सन्देशः(सन्देशः, (word32)msglen, sig, &outlen, &edkey);
  जइ(ret)
    गच्छ fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  प्रत्यागम CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  प्रत्यागम CURLE_AUTH_ERROR;
}

#नोचेद्यदि defined(USE_GNUTLS)
#अन्तर्भाव <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(ध्रुव unsigned वर्ण *कुञ्चिका, size_t keylen,
                           ध्रुव unsigned वर्ण *सन्देशः, size_t msglen,
                           unsigned वर्ण *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_कुञ्चिका_SIZE];

  जइ(keylen != ED25519_कुञ्चिका_SIZE)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_कुञ्चिका(pubkey, कुञ्चिका);

  nettle_ed25519_sha512_sign(pubkey, कुञ्चिका, msglen, सन्देशः, sig);
  *siglen = CURL_ED25519_SIGLEN;

  प्रत्यागम CURLE_OK;
}

#अण्णहा /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(ध्रुव unsigned वर्ण *कुञ्चिका, size_t keylen,
                           ध्रुव unsigned वर्ण *सन्देशः, size_t msglen,
                           unsigned वर्ण *sig, size_t *siglen)
{
  (शून्य)कुञ्चिका;
  (शून्य)keylen;
  (शून्य)सन्देशः;
  (शून्य)msglen;
  (शून्य)sig;
  (शून्य)siglen;
  प्रत्यागम CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
