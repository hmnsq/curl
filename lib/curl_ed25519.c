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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 */

#അന്തര്ഭാവ "curl_ed25519.h"

#ifdef USE_WOLFSSL
#അന്തര്ഭാവ <wolfssl/options.h>
#അന്തര്ഭാവ <wolfssl/wolfcrypt/settings.h>
#endif

#ifdef USE_OPENSSL
#അന്തര്ഭാവ <openssl/evp.h>

CURLcode Curl_ed25519_sign(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ, size_t keylen,
                           സ്ഥിരം unsigned അക്ഷരം *സന്ദേശം, size_t msglen,
                           unsigned അക്ഷരം *sig, size_t *siglen)
{
  EVP_PKEY *pkey;
  EVP_MD_CTX *mdctx;
  size_t slen;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(keylen != CURL_ED25519_KEYLEN)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  pkey = EVP_PKEY_new_raw_private_താക്കോൽ(EVP_PKEY_ED25519, ശൂന്യ, താക്കോൽ, keylen);
  എങ്കിൽ(!pkey)
    തിരികെ CURLE_AUTH_ERROR;

  mdctx = EVP_MD_CTX_new();
  എങ്കിൽ(!mdctx) {
    EVP_PKEY_free(pkey);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  rc = EVP_DigestSignInit(mdctx, ശൂന്യ, ശൂന്യ, ശൂന്യ, pkey);
  എങ്കിൽ(rc != 1) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    തിരികെ CURLE_AUTH_ERROR;
  }

  slen = CURL_ED25519_SIGLEN;
  rc = EVP_DigestSign(mdctx, sig, &slen, സന്ദേശം, msglen);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);

  എങ്കിൽ(rc != 1)
    തിരികെ CURLE_AUTH_ERROR;

  *siglen = slen;
  തിരികെ CURLE_OK;
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL) && \
  (defined(HAVE_ED25519) || defined(WOLFSSL_CURVE25519_USE_ED25519)) && \
  defined(HAVE_ED25519_താക്കോൽ_IMPORT) && defined(HAVE_ED25519_SIGN)
#അന്തര്ഭാവ <wolfssl/wolfcrypt/ed25519.h>
#അന്തര്ഭാവ <wolfssl/wolfcrypt/error-crypt.h>
#അന്തര്ഭാവ <wolfssl/wolfcrypt/random.h>

CURLcode Curl_ed25519_sign(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ, size_t keylen,
                           സ്ഥിരം unsigned അക്ഷരം *സന്ദേശം, size_t msglen,
                           unsigned അക്ഷരം *sig, size_t *siglen)
{
  പൂർണ്ണസംഖ്യ ret;
  ed25519_താക്കോൽ edkey;
  word32 outlen;
  unsigned അക്ഷരം pubkey[ED25519_PUB_താക്കോൽ_SIZE];

  എങ്കിൽ(keylen != ED25519_താക്കോൽ_SIZE)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  ret = wc_ed25519_init(&edkey);
  എങ്കിൽ(ret)
    തിരികെ CURLE_AUTH_ERROR;

  ret = wc_ed25519_import_private_only(താക്കോൽ, ED25519_താക്കോൽ_SIZE, &edkey);
  എങ്കിൽ(ret)
    പോകുക fail;

  ret = wc_ed25519_make_public(&edkey, pubkey, ED25519_PUB_താക്കോൽ_SIZE);
  എങ്കിൽ(ret)
    പോകുക fail;

  ret = wc_ed25519_import_private_താക്കോൽ(താക്കോൽ, ED25519_താക്കോൽ_SIZE,
                                      pubkey, ED25519_PUB_താക്കോൽ_SIZE, &edkey);
  എങ്കിൽ(ret)
    പോകുക fail;

  outlen = ED25519_SIG_SIZE;
  ret = wc_ed25519_sign_സന്ദേശം(സന്ദേശം, (word32)msglen, sig, &outlen, &edkey);
  എങ്കിൽ(ret)
    പോകുക fail;

  *siglen = (size_t)outlen;
  wc_ed25519_free(&edkey);
  തിരികെ CURLE_OK;

fail:
  wc_ed25519_free(&edkey);
  തിരികെ CURLE_AUTH_ERROR;
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
#അന്തര്ഭാവ <nettle/eddsa.h>

CURLcode Curl_ed25519_sign(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ, size_t keylen,
                           സ്ഥിരം unsigned അക്ഷരം *സന്ദേശം, size_t msglen,
                           unsigned അക്ഷരം *sig, size_t *siglen)
{
  uint8_t pubkey[ED25519_താക്കോൽ_SIZE];

  എങ്കിൽ(keylen != ED25519_താക്കോൽ_SIZE)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  nettle_ed25519_sha512_public_താക്കോൽ(pubkey, താക്കോൽ);

  nettle_ed25519_sha512_sign(pubkey, താക്കോൽ, msglen, സന്ദേശം, sig);
  *siglen = CURL_ED25519_SIGLEN;

  തിരികെ CURLE_OK;
}

#അല്ലെങ്കിൽ /* no Ed25519-capable backend */

CURLcode Curl_ed25519_sign(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ, size_t keylen,
                           സ്ഥിരം unsigned അക്ഷരം *സന്ദേശം, size_t msglen,
                           unsigned അക്ഷരം *sig, size_t *siglen)
{
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)keylen;
  (ശൂന്യം)സന്ദേശം;
  (ശൂന്യം)msglen;
  (ശൂന്യം)sig;
  (ശൂന്യം)siglen;
  തിരികെ CURLE_NOT_BUILT_IN;
}

#endif /* Ed25519 backends */

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
