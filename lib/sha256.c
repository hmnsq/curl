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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ಅನ್ತರ್ಭಾವ "curl_sha256.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MBEDTLS
#ಅನ್ತರ್ಭಾವ <mbedtls/version.h>
#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x03020000
#ದೋಷ "mbedTLS 3.2.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <psa/crypto_config.h>
#ಆದರೆ_ಅಂತ್ಯ

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

ರಚನೆ ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ossl_sha256_ctx my_sha256_ctx;

ಸ್ಥಿರ CURLcode my_sha256_init(ಶೂನ್ಯ *ಒಳ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ಒಳ;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  ಆದರೆ(!ctx->openssl_ctx)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), ಖಾಲಿ)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_update(ಶೂನ್ಯ *ಒಳ,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ಒಳ;
  EVP_DigestUpdate(ctx->openssl_ctx, ದತ್ತಾಂಶ, ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ಒಳ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ಒಳ;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, ಖಾಲಿ);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/sha256.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ wc_Sha256 my_sha256_ctx;

ಸ್ಥಿರ CURLcode my_sha256_init(ಶೂನ್ಯ *ಒಳ)
{
  ಆದರೆ(wc_InitSha256(ಒಳ))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_update(ಶೂನ್ಯ *ಒಳ,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  (ಶೂನ್ಯ)wc_Sha256Update(ಒಳ, ದತ್ತಾಂಶ, (word32)ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ಒಳ)
{
  (ಶೂನ್ಯ)wc_Sha256Final(ಒಳ, digest);
}

#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <nettle/sha2.h>
#ಅನ್ತರ್ಭಾವ <nettle/version.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sha256_ctx my_sha256_ctx;

ಸ್ಥಿರ CURLcode my_sha256_init(ಶೂನ್ಯ *ctx)
{
  sha256_init(ctx);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_update(ಶೂನ್ಯ *ctx,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  sha256_update(ctx, ಉದ್ದಳತೆ, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ctx)
{
#ಆದರೆ NETTLE_ಆವೃತ್ತಿ_MAJOR >= 4
  sha256_digest(ctx, digest);
#ಇಲ್ಲದಿದ್ದರೆ
  sha256_digest(ctx, SHA256_DIGEST_ಗಾತ್ರ, digest);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಅಥವಾಆದರೆ defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#ಅನ್ತರ್ಭಾವ <psa/crypto.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ psa_hash_operation_t my_sha256_ctx;

ಸ್ಥಿರ CURLcode my_sha256_init(ಶೂನ್ಯ *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  ಆದರೆ(psa_hash_ಸಿದ್ಧತೆ(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_update(ಶೂನ್ಯ *ctx,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  (ಶೂನ್ಯ)psa_hash_update(ctx, ದತ್ತಾಂಶ, ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ctx)
{
  size_t actual_ಉದ್ದಳತೆ;
  (ಶೂನ್ಯ)psa_hash_ಮುಕ್ತಾಯ(ctx, digest, CURL_SHA256_DIGEST_ಉದ್ದಳತೆ,
                        &actual_ಉದ್ದಳತೆ);
}

#ಅಥವಾಆದರೆ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#ಅನ್ತರ್ಭಾವ <CommonCrypto/CommonDigest.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CC_SHA256_CTX my_sha256_ctx;

ಸ್ಥಿರ CURLcode my_sha256_init(ಶೂನ್ಯ *ctx)
{
  (ಶೂನ್ಯ)CC_SHA256_Init(ctx);  /* always returns 1 */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_update(ಶೂನ್ಯ *ctx,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  (ಶೂನ್ಯ)CC_SHA256_Update(ctx, ದತ್ತಾಂಶ, ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ctx)
{
  (ಶೂನ್ಯ)CC_SHA256_Final(digest, ctx);
}

#ಅಥವಾಆದರೆ defined(USE_WIN32_CRYPTO)
#ಅನ್ತರ್ಭಾವ <wincrypt.h>

ರಚನೆ sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sha256_ctx my_sha256_ctx;

ಸ್ಥಿರ CURLcode my_sha256_init(ಶೂನ್ಯ *ಒಳ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ಒಳ;
  ಆದರೆ(!CryptAcquireContext(&ctx->hCryptProv, ಖಾಲಿ, ಖಾಲಿ, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_update(ಶೂನ್ಯ *ಒಳ,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ಒಳ;
  Crypt_Hash_ದತ್ತಾಂಶ(ctx->hHash, (ಸ್ಥಿರಾಂಕ BYTE *)ದತ್ತಾಂಶ, ಉದ್ದಳತೆ, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ಒಳ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)ಒಳ;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಉದ್ದಳತೆ = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ಖಾಲಿ, &ಉದ್ದಳತೆ, 0);
  ಆದರೆ(ಉದ್ದಳತೆ == CURL_SHA256_DIGEST_ಉದ್ದಳತೆ)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &ಉದ್ದಳತೆ, 0);

  ಆದರೆ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ಆದರೆ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ಇಲ್ಲದಿದ್ದರೆ

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#ವ್ಯಾಖ್ಯಾನಿಸು WPA_GET_BE32(a)              \
  ((((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(a)[0]) << 24) | \
   (((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(a)[1]) << 16) | \
   (((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(a)[2]) <<  8) | \
    ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(a)[3]))
#ವ್ಯಾಖ್ಯಾನಿಸು WPA_PUT_BE32(a, val)                                         \
  ಮಾಡು {                                                               \
    (a)[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(val)) >> 24) & 0xff); \
    (a)[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(val)) >> 16) & 0xff); \
    (a)[2] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(val)) >>  8) & 0xff); \
    (a)[3] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(val)) & 0xff);         \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು WPA_PUT_BE64(a, val)                            \
  ಮಾಡು {                                                  \
    (a)[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(((uint64_t)(val)) & 0xff); \
  } ಆಗ(0)

ರಚನೆ sha256_state {
  uint64_t ಉದ್ದಳತೆ;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ state[8], curlen;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[64];
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sha256_state my_sha256_ctx;

/* The K array */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ K[64] = {
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
#ವ್ಯಾಖ್ಯಾನಿಸು RORc(x, y) \
  (((((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(x) & 0xFFFFFFFFUL) >> (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)((y) & 31)) | \
     ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(x) << (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#ವ್ಯಾಖ್ಯಾನಿಸು Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#ವ್ಯಾಖ್ಯಾನಿಸು Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#ವ್ಯಾಖ್ಯಾನಿಸು Sha256_S(x, n)      RORc(x, n)
#ವ್ಯಾಖ್ಯಾನಿಸು Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#ವ್ಯಾಖ್ಯಾನಿಸು Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#ವ್ಯಾಖ್ಯಾನಿಸು Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#ವ್ಯಾಖ್ಯಾನಿಸು Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#ವ್ಯಾಖ್ಯಾನಿಸು Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sha256_compress(ರಚನೆ sha256_state *md, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf)
{
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ S[8], W[64];
  ಪೂರ್ಣಾಂಕ i;

  /* Copy state into S */
  ಗಾಗಿ(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  ಗಾಗಿ(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  ಗಾಗಿ(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#ವ್ಯಾಖ್ಯಾನಿಸು RND(a, b, c, d, e, f, g, h, i)                                   \
  ಮಾಡು {                                                                   \
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } ಆಗ(0)

  ಗಾಗಿ(i = 0; i < 64; ++i) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ t;
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
  ಗಾಗಿ(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  ಹಿಂತಿರುಗಿ 0;
}

/* Initialize the hash state */
ಸ್ಥಿರ CURLcode my_sha256_init(ಶೂನ್ಯ *ಒಳ)
{
  ರಚನೆ sha256_state *md = (ರಚನೆ sha256_state *)ಒಳ;
  md->curlen = 0;
  md->ಉದ್ದಳತೆ = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_update(ಶೂನ್ಯ *ctx,
                             ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳ,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ inlen = len;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n;
  ರಚನೆ sha256_state *md = (ರಚನೆ sha256_state *)ctx;
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SHA256_ಖಂಡ_ಗಾತ್ರ 64
  ಆದರೆ(md->curlen > ಗಾತ್ರ(md->buf))
    ಹಿಂತಿರುಗಿ;
  ಆಗ(inlen > 0) {
    ಆದರೆ(md->curlen == 0 && inlen >= CURL_SHA256_ಖಂಡ_ಗಾತ್ರ) {
      ಆದರೆ(sha256_compress(md, ಒಳ) < 0)
        ಹಿಂತಿರುಗಿ;
      md->ಉದ್ದಳತೆ += CURL_SHA256_ಖಂಡ_ಗಾತ್ರ * 8;
      ಒಳ += CURL_SHA256_ಖಂಡ_ಗಾತ್ರ;
      inlen -= CURL_SHA256_ಖಂಡ_ಗಾತ್ರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      n = CURLMIN(inlen, (CURL_SHA256_ಖಂಡ_ಗಾತ್ರ - md->curlen));
      ಸ್ಮರಣೆ_ನಕಲು(md->buf + md->curlen, ಒಳ, n);
      md->curlen += n;
      ಒಳ += n;
      inlen -= n;
      ಆದರೆ(md->curlen == CURL_SHA256_ಖಂಡ_ಗಾತ್ರ) {
        ಆದರೆ(sha256_compress(md, md->buf) < 0)
          ಹಿಂತಿರುಗಿ;
        md->ಉದ್ದಳತೆ += 8 * CURL_SHA256_ಖಂಡ_ಗಾತ್ರ;
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
ಸ್ಥಿರ ಶೂನ್ಯ my_sha256_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಹೊರಹೋಗು, ಶೂನ್ಯ *ctx)
{
  ರಚನೆ sha256_state *md = ctx;
  ಪೂರ್ಣಾಂಕ i;

  ಆದರೆ(md->curlen >= ಗಾತ್ರ(md->buf))
    ಹಿಂತಿರುಗಿ;

  /* Increase the length of the message */
  md->ಉದ್ದಳತೆ += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  ಆದರೆ(md->curlen > 56) {
    ಆಗ(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  ಆಗ(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->ಉದ್ದಳತೆ);
  sha256_compress(md, md->buf);

  /* Copy output */
  ಗಾಗಿ(i = 0; i < 8; i++)
    WPA_PUT_BE32(ಹೊರಹೋಗು + (4 * i), md->state[i]);
}

#ಆದರೆ_ಅಂತ್ಯ /* CRYPTO LIBS */

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
CURLcode Curl_sha256it(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                       size_t len)
{
  CURLcode ಫಲಿತಾಂಶ;
  my_sha256_ctx ctx;

  ಫಲಿತಾಂಶ = my_sha256_init(&ctx);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಮಾಡು {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ilen = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, ಒಳಹರಿವು, ilen);
      len -= ilen;
      ಒಳಹರಿವು += ilen;
    } ಆಗ(len);
    my_sha256_final(output, &ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  ಗಾತ್ರ(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#ಆದರೆ_ಅಂತ್ಯ /* AWS, DIGEST, or libssh2 */
