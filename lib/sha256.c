/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Florin Petriuc, <petriuc.florin@gmail.com>
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

#ਜੇ !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MBEDTLS
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/version.h>
#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x03020000
#ਗਲਤੀ "mbedTLS 3.2.0 or greater required"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <psa/crypto_config.h>
#ਜੇ_ਅੰਤ

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 * 5. USE_WIN32_CRYPTO
 * 6. USE_COMMON_CRYPTO
 *
 * This ensures that the same SSL branch gets activated throughout this source
 * file even if multiple backends are enabled at the same time.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/evp.h>

ਬਣਤਰ ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ossl_sha256_ctx my_sha256_ctx;

ਸਥਿਰ CURLcode my_sha256_init(ਖਾਲੀ *ਵਿੱਚ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ਵਿੱਚ;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  ਜੇ(!ctx->openssl_ctx)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), ਨਿਰਾ)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_sha256_update(ਖਾਲੀ *ਵਿੱਚ,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ਵਿੱਚ;
  EVP_DigestUpdate(ctx->openssl_ctx, ਅੰਕੜੇ, ਲੰਮਾਈ);
}

ਸਥਿਰ ਖਾਲੀ my_sha256_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ਵਿੱਚ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ਵਿੱਚ;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, ਨਿਰਾ);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#ਜਾਂਜੇ defined(USE_WOLFSSL)
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/wolfcrypt/sha256.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ wc_Sha256 my_sha256_ctx;

ਸਥਿਰ CURLcode my_sha256_init(ਖਾਲੀ *ਵਿੱਚ)
{
  ਜੇ(wc_InitSha256(ਵਿੱਚ))
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_sha256_update(ਖਾਲੀ *ਵਿੱਚ,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ)
{
  (ਖਾਲੀ)wc_Sha256Update(ਵਿੱਚ, ਅੰਕੜੇ, (word32)ਲੰਮਾਈ);
}

ਸਥਿਰ ਖਾਲੀ my_sha256_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ਵਿੱਚ)
{
  (ਖਾਲੀ)wc_Sha256Final(ਵਿੱਚ, digest);
}

#ਜਾਂਜੇ defined(USE_GNUTLS)
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/sha2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/version.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sha256_ctx my_sha256_ctx;

ਸਥਿਰ CURLcode my_sha256_init(ਖਾਲੀ *ctx)
{
  sha256_init(ctx);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_sha256_update(ਖਾਲੀ *ctx,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ)
{
  sha256_update(ctx, ਲੰਮਾਈ, ਅੰਕੜੇ);
}

ਸਥਿਰ ਖਾਲੀ my_sha256_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ctx)
{
#ਜੇ NETTLE_ਸੰਸਕਰਣ_MAJOR >= 4
  sha256_digest(ctx, digest);
#ਨਹੀਂਤਾਂ
  sha256_digest(ctx, SHA256_DIGEST_ਮਾਪ, digest);
#ਜੇ_ਅੰਤ
}

#ਜਾਂਜੇ defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#ਅਨ੍ਤਰ੍ਭਾਵ <psa/crypto.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ psa_hash_operation_t my_sha256_ctx;

ਸਥਿਰ CURLcode my_sha256_init(ਖਾਲੀ *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  ਜੇ(psa_hash_ਤਿਆਰੀ(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_sha256_update(ਖਾਲੀ *ctx,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ)
{
  (ਖਾਲੀ)psa_hash_update(ctx, ਅੰਕੜੇ, ਲੰਮਾਈ);
}

ਸਥਿਰ ਖਾਲੀ my_sha256_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ctx)
{
  size_t actual_ਲੰਮਾਈ;
  (ਖਾਲੀ)psa_hash_ਅੰਤ(ctx, digest, CURL_SHA256_DIGEST_ਲੰਮਾਈ,
                        &actual_ਲੰਮਾਈ);
}

#ਜਾਂਜੇ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#ਅਨ੍ਤਰ੍ਭਾਵ <CommonCrypto/CommonDigest.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CC_SHA256_CTX my_sha256_ctx;

ਸਥਿਰ CURLcode my_sha256_init(ਖਾਲੀ *ctx)
{
  (ਖਾਲੀ)CC_SHA256_Init(ctx);  /* always returns 1 */
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_sha256_update(ਖਾਲੀ *ctx,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ)
{
  (ਖਾਲੀ)CC_SHA256_Update(ctx, ਅੰਕੜੇ, ਲੰਮਾਈ);
}

ਸਥਿਰ ਖਾਲੀ my_sha256_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ctx)
{
  (ਖਾਲੀ)CC_SHA256_Final(digest, ctx);
}

#ਜਾਂਜੇ defined(USE_WIN32_CRYPTO)
#ਅਨ੍ਤਰ੍ਭਾਵ <wincrypt.h>

ਬਣਤਰ sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sha256_ctx my_sha256_ctx;

ਸਥਿਰ CURLcode my_sha256_init(ਖਾਲੀ *ਵਿੱਚ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ਵਿੱਚ;
  ਜੇ(!CryptAcquireContext(&ctx->hCryptProv, ਨਿਰਾ, ਨਿਰਾ, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_sha256_update(ਖਾਲੀ *ਵਿੱਚ,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ਵਿੱਚ;
  Crypt_Hash_ਅੰਕੜੇ(ctx->hHash, (ਅਚਲ BYTE *)ਅੰਕੜੇ, ਲੰਮਾਈ, 0);
}

ਸਥਿਰ ਖਾਲੀ my_sha256_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ਵਿੱਚ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ਵਿੱਚ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾਈ = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ਨਿਰਾ, &ਲੰਮਾਈ, 0);
  ਜੇ(ਲੰਮਾਈ == CURL_SHA256_DIGEST_ਲੰਮਾਈ)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &ਲੰਮਾਈ, 0);

  ਜੇ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ਜੇ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ਨਹੀਂਤਾਂ

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WPA_GET_BE32(a)              \
  ((((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(a)[0]) << 24) | \
   (((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(a)[1]) << 16) | \
   (((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(a)[2]) <<  8) | \
    ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(a)[3]))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WPA_PUT_BE32(a, val)                                         \
  ਕਰੋ {                                                               \
    (a)[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(val)) >> 24) & 0xff); \
    (a)[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(val)) >> 16) & 0xff); \
    (a)[2] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(val)) >>  8) & 0xff); \
    (a)[3] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(val)) & 0xff);         \
  } ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WPA_PUT_BE64(a, val)                            \
  ਕਰੋ {                                                  \
    (a)[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(((uint64_t)(val)) & 0xff); \
  } ਜਦੋਂ(0)

ਬਣਤਰ sha256_state {
  uint64_t ਲੰਮਾਈ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ state[8], curlen;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[64];
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sha256_state my_sha256_ctx;

/* The K array */
ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ K[64] = {
  0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
  0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
  0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
  0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
  0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
  0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
  0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
  0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
  0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
  0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
  0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
  0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
  0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Various logical functions */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RORc(x, y) \
  (((((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(x) & 0xFFFFFFFFUL) >> (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)((y) & 31)) | \
     ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(x) << (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Sha256_S(x, n)      RORc(x, n)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
ਸਥਿਰ ਪੂਰਨ sha256_compress(ਬਣਤਰ sha256_state *md, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ S[8], W[64];
  ਪੂਰਨ i;

  /* Copy state into S */
  ਲਈ(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  ਲਈ(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  ਲਈ(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RND(a, b, c, d, e, f, g, h, i)                                   \
  ਕਰੋ {                                                                   \
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } ਜਦੋਂ(0)

  ਲਈ(i = 0; i < 64; ++i) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ t;
    RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i);
    t = S[7];
    S[7] = S[6];
    S[6] = S[5];
    S[5] = S[4];
    S[4] = S[3];
    S[3] = S[2];
    S[2] = S[1];
    S[1] = S[0];
    S[0] = t;
  }

  /* Feedback */
  ਲਈ(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  ਵਾਪਸ 0;
}

/* Initialize the hash state */
ਸਥਿਰ CURLcode my_sha256_init(ਖਾਲੀ *ਵਿੱਚ)
{
  ਬਣਤਰ sha256_state *md = (ਬਣਤਰ sha256_state *)ਵਿੱਚ;
  md->curlen = 0;
  md->ਲੰਮਾਈ = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;

  ਵਾਪਸ CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
ਸਥਿਰ ਖਾਲੀ my_sha256_update(ਖਾਲੀ *ctx,
                             ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਵਿੱਚ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ inlen = len;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n;
  ਬਣਤਰ sha256_state *md = (ਬਣਤਰ sha256_state *)ctx;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SHA256_ਖੰਡ_ਮਾਪ 64
  ਜੇ(md->curlen > ਆਕਾਰ(md->buf))
    ਵਾਪਸ;
  ਜਦੋਂ(inlen > 0) {
    ਜੇ(md->curlen == 0 && inlen >= CURL_SHA256_ਖੰਡ_ਮਾਪ) {
      ਜੇ(sha256_compress(md, ਵਿੱਚ) < 0)
        ਵਾਪਸ;
      md->ਲੰਮਾਈ += CURL_SHA256_ਖੰਡ_ਮਾਪ * 8;
      ਵਿੱਚ += CURL_SHA256_ਖੰਡ_ਮਾਪ;
      inlen -= CURL_SHA256_ਖੰਡ_ਮਾਪ;
    }
    ਨਹੀਂਤਾਂ {
      n = CURLMIN(inlen, (CURL_SHA256_ਖੰਡ_ਮਾਪ - md->curlen));
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(md->buf + md->curlen, ਵਿੱਚ, n);
      md->curlen += n;
      ਵਿੱਚ += n;
      inlen -= n;
      ਜੇ(md->curlen == CURL_SHA256_ਖੰਡ_ਮਾਪ) {
        ਜੇ(sha256_compress(md, md->buf) < 0)
          ਵਾਪਸ;
        md->ਲੰਮਾਈ += 8 * CURL_SHA256_ਖੰਡ_ਮਾਪ;
        md->curlen = 0;
      }
    }
  }
}

/* Terminate the hash to get the digest
   @param md  The hash state
   @param out [out] The destination of the hash (32 bytes)
   @return 0 if successful
 */
ਸਥਿਰ ਖਾਲੀ my_sha256_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਾਹਰਲਾ, ਖਾਲੀ *ctx)
{
  ਬਣਤਰ sha256_state *md = ctx;
  ਪੂਰਨ i;

  ਜੇ(md->curlen >= ਆਕਾਰ(md->buf))
    ਵਾਪਸ;

  /* Increase the length of the message */
  md->ਲੰਮਾਈ += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  ਜੇ(md->curlen > 56) {
    ਜਦੋਂ(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  ਜਦੋਂ(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->ਲੰਮਾਈ);
  sha256_compress(md, md->buf);

  /* Copy output */
  ਲਈ(i = 0; i < 8; i++)
    WPA_PUT_BE32(ਬਾਹਰਲਾ + (4 * i), md->state[i]);
}

#ਜੇ_ਅੰਤ /* CRYPTO LIBS */

/*
 * Curl_sha256it()
 *
 * Generates a SHA256 hash for the given input data.
 *
 * Parameters:
 *
 * output [in/out] - The output buffer.
 * input  [in]     - The input data.
 * length [in]     - The input length.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_sha256it(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                       size_t len)
{
  CURLcode ਨਤੀਜਾ;
  my_sha256_ctx ctx;

  ਨਤੀਜਾ = my_sha256_init(&ctx);
  ਜੇ(!ਨਤੀਜਾ) {
    ਕਰੋ {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ilen = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, ਨਿਵੇਸ਼, ilen);
      len -= ilen;
      ਨਿਵੇਸ਼ += ilen;
    } ਜਦੋਂ(len);
    my_sha256_final(output, &ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਅਚਲ ਬਣਤਰ HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  ਆਕਾਰ(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#ਜੇ_ਅੰਤ /* AWS, DIGEST, or libssh2 */
