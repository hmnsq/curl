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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#ಅನ್ತರ್ಭಾವ "curl_md5.h"
#ಅನ್ತರ್ಭಾವ "curl_hmac.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/opensslconf.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WOLFSSL
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MBEDTLS
#ಅನ್ತರ್ಭಾವ <mbedtls/version.h>
#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x03020000
#ದೋಷ "mbedTLS 3.2.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <psa/crypto_config.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GNUTLS
#ಅನ್ತರ್ಭಾವ <nettle/md5.h>
#ಅನ್ತರ್ಭಾವ <nettle/version.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ md5_ctx my_md5_ctx;

ಸ್ಥಿರ CURLcode my_md5_init(ಶೂನ್ಯ *ctx)
{
  md5_init(ctx);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_update(ಶೂನ್ಯ *ctx,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  md5_update(ctx, len, ಒಳಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ctx)
{
#ಆದರೆ NETTLE_ಆವೃತ್ತಿ_MAJOR >= 4
  md5_digest(ctx, digest);
#ಇಲ್ಲದಿದ್ದರೆ
  md5_digest(ctx, MD5_DIGEST_LEN, digest);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಅಥವಾಆದರೆ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD5)
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ EVP_MD_CTX *my_md5_ctx;

ಸ್ಥಿರ CURLcode my_md5_init(ಶೂನ್ಯ *ಒಳ)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)ಒಳ;
  *ctx = EVP_MD_CTX_new();
  ಆದರೆ(!*ctx)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!EVP_DigestInit_ex(*ctx, EVP_md5(), ಖಾಲಿ)) {
    EVP_MD_CTX_free(*ctx);
    *ctx = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_update(ಶೂನ್ಯ *ಒಳ,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)ಒಳ;
  (ಶೂನ್ಯ)EVP_DigestUpdate(*ctx, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ಒಳ)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)ಒಳ;
  (ಶೂನ್ಯ)EVP_DigestFinal_ex(*ctx, digest, ಖಾಲಿ);
  EVP_MD_CTX_free(*ctx);
  *ctx = ಖಾಲಿ;
}

#ಅಥವಾಆದರೆ defined(USE_WOLFSSL) && !defined(NO_MD5)
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/md5.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ wc_Md5 my_md5_ctx;

ಸ್ಥಿರ CURLcode my_md5_init(ಶೂನ್ಯ *ctx)
{
  ಆದರೆ(wc_InitMd5(ctx))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_update(ಶೂನ್ಯ *ctx,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  (ಶೂನ್ಯ)wc_Md5Update(ctx, ಒಳಹರಿವು, (word32)len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ctx)
{
  (ಶೂನ್ಯ)wc_Md5Final(ctx, digest);
}

#ಅಥವಾಆದರೆ defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_MD5) && PSA_WANT_ALG_MD5
#ಅನ್ತರ್ಭಾವ <psa/crypto.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ psa_hash_operation_t my_md5_ctx;

ಸ್ಥಿರ CURLcode my_md5_init(ಶೂನ್ಯ *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  ಆದರೆ(psa_hash_ಸಿದ್ಧತೆ(pctx, PSA_ALG_MD5) != PSA_SUCCESS)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_update(ಶೂನ್ಯ *ctx,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  (ಶೂನ್ಯ)psa_hash_update(ctx, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ctx)
{
  size_t actual_ಉದ್ದಳತೆ;
  (ಶೂನ್ಯ)psa_hash_ಮುಕ್ತಾಯ(ctx, digest, MD5_DIGEST_LEN, &actual_ಉದ್ದಳತೆ);
}

#ಅಥವಾಆದರೆ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#ಅನ್ತರ್ಭಾವ <CommonCrypto/CommonDigest.h>

/* For Apple operating systems: CommonCrypto has the functions we need.
   These functions are available on Tiger and later, as well as iOS 2.0
   and later. If you are building for an older cat, well, sorry.

   Declaring the functions as static like this seems to be a bit more
   reliable than defining COMMON_DIGEST_FOR_OPENSSL on older cats. */
#  define my_md5_ctx CC_MD5_CTX

ಸ್ಥಿರ CURLcode my_md5_init(ಶೂನ್ಯ *ctx)
{
  (ಶೂನ್ಯ)CC_MD5_Init(ctx);  /* always returns 1 */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_update(ಶೂನ್ಯ *ctx,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  CC_MD5_Update(ctx, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ctx)
{
  CC_MD5_Final(digest, ctx);
}

#ಅಥವಾಆದರೆ defined(USE_WIN32_CRYPTO)
#ಅನ್ತರ್ಭಾವ <wincrypt.h>

ರಚನೆ md5_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ md5_ctx my_md5_ctx;

ಸ್ಥಿರ CURLcode my_md5_init(ಶೂನ್ಯ *ಒಳ)
{
  my_md5_ctx *ctx = (my_md5_ctx *)ಒಳ;
  ಆದರೆ(!CryptAcquireContext(&ctx->hCryptProv, ಖಾಲಿ, ಖಾಲಿ, PROV_RSA_ತುಂಬಿದ,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!CryptCreateHash(ctx->hCryptProv, CALG_MD5, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_update(ಶೂನ್ಯ *ಒಳ,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  my_md5_ctx *ctx = ಒಳ;
  Crypt_Hash_ದತ್ತಾಂಶ(ctx->hHash, (ಸ್ಥಿರಾಂಕ BYTE *)ಒಳಹರಿವು, len, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ಒಳ)
{
  my_md5_ctx *ctx = (my_md5_ctx *)ಒಳ;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಉದ್ದಳತೆ = 0;
  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ಖಾಲಿ, &ಉದ್ದಳತೆ, 0);
  ಆದರೆ(ಉದ್ದಳತೆ == MD5_DIGEST_LEN)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &ಉದ್ದಳತೆ, 0);
  ಆದರೆ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);
  ಆದರೆ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ಇಲ್ಲದಿದ್ದರೆ

/* When no other crypto library is available we use this code segment */

/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * https://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001. No copyright is
 * claimed, and the software is hereby placed in the public domain. In case
 * this attempt to disclaim copyright and place the software in the public
 * domain is deemed null and void, then the software is Copyright (c) 2001
 * Alexander Peslyak and it is hereby released to the general public under
 * the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There is ABSOLUTELY NO WARRANTY, express or implied.
 *
 * (This is a heavily cut-down "BSD license".)
 */

ರಚನೆ md5_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಬಫರ್[64];
  uint32_t ಖಂಡ[16];
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ md5_ctx my_md5_ctx;

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, like in Colin Plumb's
 * implementation.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_G(x, y, z)                  ((y) ^ ((z) & ((x) ^ (y))))
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_H(x, y, z)                  (((x) ^ (y)) ^ (z))
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_H2(x, y, z)                 ((x) ^ ((y) ^ (z)))
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_I(x, y, z)                  ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_ಹೆಜ್ಜೆ(f, a, b, c, d, x, t, s) \
  (a) += f(b, c, d) + (x) + (t); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
  (a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is an optimization. Nothing will break if it does not work.
 */
#ಆದರೆ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_ಗಣ(n) (*(ಸ್ಥಿರಾಂಕ uint32_t *)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&ptr[(n) * 4])
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_GET(n) MD5_ಗಣ(n)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_ಗಣ(n) (ctx->ಖಂಡ[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_GET(n) ctx->ಖಂಡ[n]
#ಆದರೆ_ಅಂತ್ಯ

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *my_md5_body(my_md5_ctx *ctx,
                               ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ptr;
  uint32_t a, b, c, d;

  ptr = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ದತ್ತಾಂಶ;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  ಮಾಡು {
    uint32_t saved_a, saved_b, saved_c, saved_d;

    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    MD5_ಹೆಜ್ಜೆ(MD5_F, a, b, c, d, MD5_ಗಣ(0), 0xd76aa478, 7)
    MD5_ಹೆಜ್ಜೆ(MD5_F, d, a, b, c, MD5_ಗಣ(1), 0xe8c7b756, 12)
    MD5_ಹೆಜ್ಜೆ(MD5_F, c, d, a, b, MD5_ಗಣ(2), 0x242070db, 17)
    MD5_ಹೆಜ್ಜೆ(MD5_F, b, c, d, a, MD5_ಗಣ(3), 0xc1bdceee, 22)
    MD5_ಹೆಜ್ಜೆ(MD5_F, a, b, c, d, MD5_ಗಣ(4), 0xf57c0faf, 7)
    MD5_ಹೆಜ್ಜೆ(MD5_F, d, a, b, c, MD5_ಗಣ(5), 0x4787c62a, 12)
    MD5_ಹೆಜ್ಜೆ(MD5_F, c, d, a, b, MD5_ಗಣ(6), 0xa8304613, 17)
    MD5_ಹೆಜ್ಜೆ(MD5_F, b, c, d, a, MD5_ಗಣ(7), 0xfd469501, 22)
    MD5_ಹೆಜ್ಜೆ(MD5_F, a, b, c, d, MD5_ಗಣ(8), 0x698098d8, 7)
    MD5_ಹೆಜ್ಜೆ(MD5_F, d, a, b, c, MD5_ಗಣ(9), 0x8b44f7af, 12)
    MD5_ಹೆಜ್ಜೆ(MD5_F, c, d, a, b, MD5_ಗಣ(10), 0xffff5bb1, 17)
    MD5_ಹೆಜ್ಜೆ(MD5_F, b, c, d, a, MD5_ಗಣ(11), 0x895cd7be, 22)
    MD5_ಹೆಜ್ಜೆ(MD5_F, a, b, c, d, MD5_ಗಣ(12), 0x6b901122, 7)
    MD5_ಹೆಜ್ಜೆ(MD5_F, d, a, b, c, MD5_ಗಣ(13), 0xfd987193, 12)
    MD5_ಹೆಜ್ಜೆ(MD5_F, c, d, a, b, MD5_ಗಣ(14), 0xa679438e, 17)
    MD5_ಹೆಜ್ಜೆ(MD5_F, b, c, d, a, MD5_ಗಣ(15), 0x49b40821, 22)

    /* Round 2 */
    MD5_ಹೆಜ್ಜೆ(MD5_G, a, b, c, d, MD5_GET(1), 0xf61e2562, 5)
    MD5_ಹೆಜ್ಜೆ(MD5_G, d, a, b, c, MD5_GET(6), 0xc040b340, 9)
    MD5_ಹೆಜ್ಜೆ(MD5_G, c, d, a, b, MD5_GET(11), 0x265e5a51, 14)
    MD5_ಹೆಜ್ಜೆ(MD5_G, b, c, d, a, MD5_GET(0), 0xe9b6c7aa, 20)
    MD5_ಹೆಜ್ಜೆ(MD5_G, a, b, c, d, MD5_GET(5), 0xd62f105d, 5)
    MD5_ಹೆಜ್ಜೆ(MD5_G, d, a, b, c, MD5_GET(10), 0x02441453, 9)
    MD5_ಹೆಜ್ಜೆ(MD5_G, c, d, a, b, MD5_GET(15), 0xd8a1e681, 14)
    MD5_ಹೆಜ್ಜೆ(MD5_G, b, c, d, a, MD5_GET(4), 0xe7d3fbc8, 20)
    MD5_ಹೆಜ್ಜೆ(MD5_G, a, b, c, d, MD5_GET(9), 0x21e1cde6, 5)
    MD5_ಹೆಜ್ಜೆ(MD5_G, d, a, b, c, MD5_GET(14), 0xc33707d6, 9)
    MD5_ಹೆಜ್ಜೆ(MD5_G, c, d, a, b, MD5_GET(3), 0xf4d50d87, 14)
    MD5_ಹೆಜ್ಜೆ(MD5_G, b, c, d, a, MD5_GET(8), 0x455a14ed, 20)
    MD5_ಹೆಜ್ಜೆ(MD5_G, a, b, c, d, MD5_GET(13), 0xa9e3e905, 5)
    MD5_ಹೆಜ್ಜೆ(MD5_G, d, a, b, c, MD5_GET(2), 0xfcefa3f8, 9)
    MD5_ಹೆಜ್ಜೆ(MD5_G, c, d, a, b, MD5_GET(7), 0x676f02d9, 14)
    MD5_ಹೆಜ್ಜೆ(MD5_G, b, c, d, a, MD5_GET(12), 0x8d2a4c8a, 20)

    /* Round 3 */
    MD5_ಹೆಜ್ಜೆ(MD5_H, a, b, c, d, MD5_GET(5), 0xfffa3942, 4)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, d, a, b, c, MD5_GET(8), 0x8771f681, 11)
    MD5_ಹೆಜ್ಜೆ(MD5_H, c, d, a, b, MD5_GET(11), 0x6d9d6122, 16)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, b, c, d, a, MD5_GET(14), 0xfde5380c, 23)
    MD5_ಹೆಜ್ಜೆ(MD5_H, a, b, c, d, MD5_GET(1), 0xa4beea44, 4)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, d, a, b, c, MD5_GET(4), 0x4bdecfa9, 11)
    MD5_ಹೆಜ್ಜೆ(MD5_H, c, d, a, b, MD5_GET(7), 0xf6bb4b60, 16)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, b, c, d, a, MD5_GET(10), 0xbebfbc70, 23)
    MD5_ಹೆಜ್ಜೆ(MD5_H, a, b, c, d, MD5_GET(13), 0x289b7ec6, 4)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, d, a, b, c, MD5_GET(0), 0xeaa127fa, 11)
    MD5_ಹೆಜ್ಜೆ(MD5_H, c, d, a, b, MD5_GET(3), 0xd4ef3085, 16)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, b, c, d, a, MD5_GET(6), 0x04881d05, 23)
    MD5_ಹೆಜ್ಜೆ(MD5_H, a, b, c, d, MD5_GET(9), 0xd9d4d039, 4)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, d, a, b, c, MD5_GET(12), 0xe6db99e5, 11)
    MD5_ಹೆಜ್ಜೆ(MD5_H, c, d, a, b, MD5_GET(15), 0x1fa27cf8, 16)
    MD5_ಹೆಜ್ಜೆ(MD5_H2, b, c, d, a, MD5_GET(2), 0xc4ac5665, 23)

    /* Round 4 */
    MD5_ಹೆಜ್ಜೆ(MD5_I, a, b, c, d, MD5_GET(0), 0xf4292244, 6)
    MD5_ಹೆಜ್ಜೆ(MD5_I, d, a, b, c, MD5_GET(7), 0x432aff97, 10)
    MD5_ಹೆಜ್ಜೆ(MD5_I, c, d, a, b, MD5_GET(14), 0xab9423a7, 15)
    MD5_ಹೆಜ್ಜೆ(MD5_I, b, c, d, a, MD5_GET(5), 0xfc93a039, 21)
    MD5_ಹೆಜ್ಜೆ(MD5_I, a, b, c, d, MD5_GET(12), 0x655b59c3, 6)
    MD5_ಹೆಜ್ಜೆ(MD5_I, d, a, b, c, MD5_GET(3), 0x8f0ccc92, 10)
    MD5_ಹೆಜ್ಜೆ(MD5_I, c, d, a, b, MD5_GET(10), 0xffeff47d, 15)
    MD5_ಹೆಜ್ಜೆ(MD5_I, b, c, d, a, MD5_GET(1), 0x85845dd1, 21)
    MD5_ಹೆಜ್ಜೆ(MD5_I, a, b, c, d, MD5_GET(8), 0x6fa87e4f, 6)
    MD5_ಹೆಜ್ಜೆ(MD5_I, d, a, b, c, MD5_GET(15), 0xfe2ce6e0, 10)
    MD5_ಹೆಜ್ಜೆ(MD5_I, c, d, a, b, MD5_GET(6), 0xa3014314, 15)
    MD5_ಹೆಜ್ಜೆ(MD5_I, b, c, d, a, MD5_GET(13), 0x4e0811a1, 21)
    MD5_ಹೆಜ್ಜೆ(MD5_I, a, b, c, d, MD5_GET(4), 0xf7537e82, 6)
    MD5_ಹೆಜ್ಜೆ(MD5_I, d, a, b, c, MD5_GET(11), 0xbd3af235, 10)
    MD5_ಹೆಜ್ಜೆ(MD5_I, c, d, a, b, MD5_GET(2), 0x2ad7d2bb, 15)
    MD5_ಹೆಜ್ಜೆ(MD5_I, b, c, d, a, MD5_GET(9), 0xeb86d391, 21)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } ಆಗ(ಗಾತ್ರ_2 -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  ಹಿಂತಿರುಗಿ ptr;
}

ಸ್ಥಿರ CURLcode my_md5_init(ಶೂನ್ಯ *ಒಳ)
{
  my_md5_ctx *ctx = (my_md5_ctx *)ಒಳ;
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_update(ಶೂನ್ಯ *ಒಳ,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  uint32_t saved_lo;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಬಳಸಿದ;
  my_md5_ctx *ctx = (my_md5_ctx *)ಒಳ;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ಆದರೆ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  ಬಳಸಿದ = saved_lo & 0x3f;

  ಆದರೆ(ಬಳಸಿದ) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಲಭ್ಯ = 64 - ಬಳಸಿದ;

    ಆದರೆ(len < ಲಭ್ಯ) {
      ಸ್ಮರಣೆ_ನಕಲು(&ctx->ಬಫರ್[ಬಳಸಿದ], ಒಳಹರಿವು, len);
      ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಮರಣೆ_ನಕಲು(&ctx->ಬಫರ್[ಬಳಸಿದ], ಒಳಹರಿವು, ಲಭ್ಯ);
    ಒಳಹರಿವು = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಒಳಹರಿವು + ಲಭ್ಯ;
    len -= ಲಭ್ಯ;
    my_md5_body(ctx, ctx->ಬಫರ್, 64);
  }

  ಆದರೆ(len >= 64) {
    ಒಳಹರಿವು = my_md5_body(ctx, ಒಳಹರಿವು, len & ~(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x3f);
    len &= 0x3f;
  }

  ಸ್ಮರಣೆ_ನಕಲು(ctx->ಬಫರ್, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md5_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, ಶೂನ್ಯ *ಒಳ)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಬಳಸಿದ, ಲಭ್ಯ;
  my_md5_ctx *ctx = (my_md5_ctx *)ಒಳ;

  ಬಳಸಿದ = ctx->lo & 0x3f;

  ctx->ಬಫರ್[ಬಳಸಿದ++] = 0x80;

  ಲಭ್ಯ = 64 - ಬಳಸಿದ;

  ಆದರೆ(ಲಭ್ಯ < 8) {
    memset(&ctx->ಬಫರ್[ಬಳಸಿದ], 0, ಲಭ್ಯ);
    my_md5_body(ctx, ctx->ಬಫರ್, 64);
    ಬಳಸಿದ = 0;
    ಲಭ್ಯ = 64;
  }

  memset(&ctx->ಬಫರ್[ಬಳಸಿದ], 0, ಲಭ್ಯ - 8);

  ctx->lo <<= 3;
  ctx->ಬಫರ್[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->ಬಫರ್[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->ಬಫರ್[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->ಬಫರ್[59] = curlx_ultouc(ctx->lo >> 24);
  ctx->ಬಫರ್[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->ಬಫರ್[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->ಬಫರ್[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->ಬಫರ್[63] = curlx_ultouc(ctx->hi >> 24);

  my_md5_body(ctx, ctx->ಬಫರ್, 64);

  digest[0] = curlx_ultouc((ctx->a) & 0xff);
  digest[1] = curlx_ultouc((ctx->a >> 8) & 0xff);
  digest[2] = curlx_ultouc((ctx->a >> 16) & 0xff);
  digest[3] = curlx_ultouc(ctx->a >> 24);
  digest[4] = curlx_ultouc((ctx->b) & 0xff);
  digest[5] = curlx_ultouc((ctx->b >> 8) & 0xff);
  digest[6] = curlx_ultouc((ctx->b >> 16) & 0xff);
  digest[7] = curlx_ultouc(ctx->b >> 24);
  digest[8] = curlx_ultouc((ctx->c) & 0xff);
  digest[9] = curlx_ultouc((ctx->c >> 8) & 0xff);
  digest[10] = curlx_ultouc((ctx->c >> 16) & 0xff);
  digest[11] = curlx_ultouc(ctx->c >> 24);
  digest[12] = curlx_ultouc((ctx->d) & 0xff);
  digest[13] = curlx_ultouc((ctx->d >> 8) & 0xff);
  digest[14] = curlx_ultouc((ctx->d >> 16) & 0xff);
  digest[15] = curlx_ultouc(ctx->d >> 24);

  memset(ctx, 0, ಗಾತ್ರ(*ctx));
}

#ಆದರೆ_ಅಂತ್ಯ /* CRYPTO LIBS */

ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params Curl_HMAC_MD5 = {
  my_md5_init,        /* Hash initialization function. */
  my_md5_update,      /* Hash update function. */
  my_md5_final,       /* Hash computation end function. */
  ಗಾತ್ರ(my_md5_ctx), /* Size of hash context structure. */
  64,                 /* Maximum key length. */
  MD5_DIGEST_LEN      /* Result size. */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ MD5_params Curl_DIGEST_MD5 = {
  my_md5_init,        /* Digest initialization function */
  my_md5_update,      /* Digest update function */
  my_md5_final,       /* Digest computation end function */
  ಗಾತ್ರ(my_md5_ctx), /* Size of digest context struct */
  MD5_DIGEST_LEN      /* Result size */
};

/*
 * @unittest: 1601
 * Returns CURLE_OK on success.
 */
CURLcode Curl_md5it(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output,
                    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, size_t len)
{
  CURLcode ಫಲಿತಾಂಶ;
  my_md5_ctx ctx;

  ಫಲಿತಾಂಶ = my_md5_init(&ctx);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಮಾಡು {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ilen = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURLMIN(len, UINT_MAX);
      my_md5_update(&ctx, ಒಳಹರಿವು, ilen);
      ಒಳಹರಿವು += ilen;
      len -= ilen;
    } ಆಗ(len);
    my_md5_final(output, &ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ MD5_context *Curl_MD5_init(ಸ್ಥಿರಾಂಕ ರಚನೆ MD5_params *md5params)
{
  ರಚನೆ MD5_context *ctxt;

  /* Create MD5 context */
  ctxt = curlx_malloc(ಗಾತ್ರ(*ctxt));

  ಆದರೆ(!ctxt)
    ಹಿಂತಿರುಗಿ ctxt;

  ctxt->md5_hashctx = curlx_malloc(md5params->md5_ctxtsize);

  ಆದರೆ(!ctxt->md5_hashctx) {
    curlx_free(ctxt);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  ctxt->md5_hash = md5params;

  ಆದರೆ((*md5params->md5_init_func)(ctxt->md5_hashctx)) {
    curlx_free(ctxt->md5_hashctx);
    curlx_free(ctxt);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ ctxt;
}

CURLcode Curl_MD5_update(ರಚನೆ MD5_context *context,
                         ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  (*context->md5_hash->md5_update_func)(context->md5_hashctx, ಒಳಹರಿವು, len);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_MD5_final(ರಚನೆ MD5_context *context, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಫಲಿತಾಂಶ)
{
  (*context->md5_hash->md5_final_func)(ಫಲಿತಾಂಶ, context->md5_hashctx);

  curlx_free(context->md5_hashctx);
  curlx_free(context);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* Using NTLM (without SSPI) || Digest */
