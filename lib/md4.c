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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_NTLM_CORE

#ಅನ್ತರ್ಭಾವ "curl_md4.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/opensslv.h>
#ಆದರೆ OPENSSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#ವ್ಯಾಖ್ಯಾನಿಸು OPENSSL_NO_MD4
#ಇಲ್ಲದಿದ್ದರೆ
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#ಅನ್ತರ್ಭಾವ <openssl/opensslconf.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* USE_OPENSSL */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WOLFSSL
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ EVP_MD_CTX *my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  ಆದರೆ(!*ctx)
    ಹಿಂತಿರುಗಿ 0;

  ಆದರೆ(!EVP_DigestInit_ex(*ctx, EVP_md4(), ಖಾಲಿ)) {
    EVP_MD_CTX_free(*ctx);
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  (ಶೂನ್ಯ)EVP_DigestUpdate(*ctx, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, my_md4_ctx *ctx)
{
  (ಶೂನ್ಯ)EVP_DigestFinal_ex(*ctx, digest, ಖಾಲಿ);
  EVP_MD_CTX_free(*ctx);
}

#ಅಥವಾಆದರೆ defined(USE_WOLFSSL) && !defined(NO_MD4)
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/md4.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/version.h>

#ಆದರೆ LIBWOLFSSL_ಆವೃತ್ತಿ_HEX >= 0x05007006
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ wc_Md4 my_md4_ctx;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ Md4 my_md4_ctx;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
  wc_Md4Update(ctx, ಒಳಹರಿವು, (word32)len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CC_MD4_CTX my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ my_md4_init(my_md4_ctx *ctx)
{
  ಹಿಂತಿರುಗಿ CC_MD4_Init(ctx);  /* always returns 1 */
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
  (ಶೂನ್ಯ)CC_MD4_Update(ctx, ಒಳಹರಿವು, (CC_LONG)len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, my_md4_ctx *ctx)
{
  (ಶೂನ್ಯ)CC_MD4_Final(digest, ctx);
}

#ಅಥವಾಆದರೆ defined(USE_WIN32_CRYPTO)
#ಅನ್ತರ್ಭಾವ <wincrypt.h>

ರಚನೆ md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ md4_ctx my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  ಆದರೆ(!CryptAcquireContext(&ctx->hCryptProv, ಖಾಲಿ, ಖಾಲಿ, PROV_RSA_ತುಂಬಿದ,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ಹಿಂತಿರುಗಿ 0;

  ಆದರೆ(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
  Crypt_Hash_ದತ್ತಾಂಶ(ctx->hHash, (ಸ್ಥಿರಾಂಕ BYTE *)ಒಳಹರಿವು, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)len, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, my_md4_ctx *ctx)
{
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಉದ್ದಳತೆ = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ಖಾಲಿ, &ಉದ್ದಳತೆ, 0);
  ಆದರೆ(ಉದ್ದಳತೆ == MD4_DIGEST_ಉದ್ದಳತೆ)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &ಉದ್ದಳತೆ, 0);

  ಆದರೆ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ಆದರೆ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <nettle/md4.h>
#ಅನ್ತರ್ಭಾವ <nettle/version.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ md4_ctx my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
  md4_update(ctx, len, ಒಳಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, my_md4_ctx *ctx)
{
#ಆದರೆ NETTLE_ಆವೃತ್ತಿ_MAJOR >= 4
  md4_digest(ctx, digest);
#ಇಲ್ಲದಿದ್ದರೆ
  md4_digest(ctx, MD4_DIGEST_ಗಾತ್ರ, digest);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಇಲ್ಲದಿದ್ದರೆ
/* When no other crypto library is available, or the crypto library does not
 * support MD4, we use this code segment this implementation of it
 *
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD4 Message-Digest Algorithm (RFC 1320).
 *
 * Homepage:
 * https://openwall.info/wiki/people/solar/software/public-domain-source-code/md4
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

ರಚನೆ md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಬಫರ್[64];
  uint32_t ಖಂಡ[16];
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ md4_ctx my_md4_ctx;

/*
 * The basic MD4 functions.
 *
 * F and G are optimized compared to their RFC 1320 definitions, with the
 * optimization for F borrowed from Colin Plumb's MD5 implementation.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_G(x, y, z)                  (((x) & ((y) | (z))) | ((y) & (z)))
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_H(x, y, z)                  ((x) ^ (y) ^ (z))

/*
 * The MD4 transformation for all three rounds.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_ಹೆಜ್ಜೆ(f, a, b, c, d, x, s) \
  (a) += f(b, c, d) + (x); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is an optimization. Nothing will break if it does not work.
 */
#ಆದರೆ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_ಗಣ(n) (*(ಸ್ಥಿರಾಂಕ uint32_t *)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)&ptr[(n) * 4])
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_GET(n) MD4_ಗಣ(n)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_ಗಣ(n) (ctx->ಖಂಡ[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_GET(n) ctx->ಖಂಡ[n]
#ಆದರೆ_ಅಂತ್ಯ

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *my_md4_body(my_md4_ctx *ctx,
                               ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಗಾತ್ರ_2)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ptr;
  uint32_t a, b, c, d;

  ptr = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಒಳಹರಿವು;

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
    MD4_ಹೆಜ್ಜೆ(MD4_F, a, b, c, d, MD4_ಗಣ(0), 3)
    MD4_ಹೆಜ್ಜೆ(MD4_F, d, a, b, c, MD4_ಗಣ(1), 7)
    MD4_ಹೆಜ್ಜೆ(MD4_F, c, d, a, b, MD4_ಗಣ(2), 11)
    MD4_ಹೆಜ್ಜೆ(MD4_F, b, c, d, a, MD4_ಗಣ(3), 19)
    MD4_ಹೆಜ್ಜೆ(MD4_F, a, b, c, d, MD4_ಗಣ(4), 3)
    MD4_ಹೆಜ್ಜೆ(MD4_F, d, a, b, c, MD4_ಗಣ(5), 7)
    MD4_ಹೆಜ್ಜೆ(MD4_F, c, d, a, b, MD4_ಗಣ(6), 11)
    MD4_ಹೆಜ್ಜೆ(MD4_F, b, c, d, a, MD4_ಗಣ(7), 19)
    MD4_ಹೆಜ್ಜೆ(MD4_F, a, b, c, d, MD4_ಗಣ(8), 3)
    MD4_ಹೆಜ್ಜೆ(MD4_F, d, a, b, c, MD4_ಗಣ(9), 7)
    MD4_ಹೆಜ್ಜೆ(MD4_F, c, d, a, b, MD4_ಗಣ(10), 11)
    MD4_ಹೆಜ್ಜೆ(MD4_F, b, c, d, a, MD4_ಗಣ(11), 19)
    MD4_ಹೆಜ್ಜೆ(MD4_F, a, b, c, d, MD4_ಗಣ(12), 3)
    MD4_ಹೆಜ್ಜೆ(MD4_F, d, a, b, c, MD4_ಗಣ(13), 7)
    MD4_ಹೆಜ್ಜೆ(MD4_F, c, d, a, b, MD4_ಗಣ(14), 11)
    MD4_ಹೆಜ್ಜೆ(MD4_F, b, c, d, a, MD4_ಗಣ(15), 19)

    /* Round 2 */
    MD4_ಹೆಜ್ಜೆ(MD4_G, a, b, c, d, MD4_GET(0) + 0x5a827999, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_G, d, a, b, c, MD4_GET(4) + 0x5a827999, 5)
    MD4_ಹೆಜ್ಜೆ(MD4_G, c, d, a, b, MD4_GET(8) + 0x5a827999, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_G, b, c, d, a, MD4_GET(12) + 0x5a827999, 13)
    MD4_ಹೆಜ್ಜೆ(MD4_G, a, b, c, d, MD4_GET(1) + 0x5a827999, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_G, d, a, b, c, MD4_GET(5) + 0x5a827999, 5)
    MD4_ಹೆಜ್ಜೆ(MD4_G, c, d, a, b, MD4_GET(9) + 0x5a827999, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_G, b, c, d, a, MD4_GET(13) + 0x5a827999, 13)
    MD4_ಹೆಜ್ಜೆ(MD4_G, a, b, c, d, MD4_GET(2) + 0x5a827999, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_G, d, a, b, c, MD4_GET(6) + 0x5a827999, 5)
    MD4_ಹೆಜ್ಜೆ(MD4_G, c, d, a, b, MD4_GET(10) + 0x5a827999, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_G, b, c, d, a, MD4_GET(14) + 0x5a827999, 13)
    MD4_ಹೆಜ್ಜೆ(MD4_G, a, b, c, d, MD4_GET(3) + 0x5a827999, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_G, d, a, b, c, MD4_GET(7) + 0x5a827999, 5)
    MD4_ಹೆಜ್ಜೆ(MD4_G, c, d, a, b, MD4_GET(11) + 0x5a827999, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_G, b, c, d, a, MD4_GET(15) + 0x5a827999, 13)

    /* Round 3 */
    MD4_ಹೆಜ್ಜೆ(MD4_H, a, b, c, d, MD4_GET(0) + 0x6ed9eba1, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_H, d, a, b, c, MD4_GET(8) + 0x6ed9eba1, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_H, c, d, a, b, MD4_GET(4) + 0x6ed9eba1, 11)
    MD4_ಹೆಜ್ಜೆ(MD4_H, b, c, d, a, MD4_GET(12) + 0x6ed9eba1, 15)
    MD4_ಹೆಜ್ಜೆ(MD4_H, a, b, c, d, MD4_GET(2) + 0x6ed9eba1, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_H, d, a, b, c, MD4_GET(10) + 0x6ed9eba1, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_H, c, d, a, b, MD4_GET(6) + 0x6ed9eba1, 11)
    MD4_ಹೆಜ್ಜೆ(MD4_H, b, c, d, a, MD4_GET(14) + 0x6ed9eba1, 15)
    MD4_ಹೆಜ್ಜೆ(MD4_H, a, b, c, d, MD4_GET(1) + 0x6ed9eba1, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_H, d, a, b, c, MD4_GET(9) + 0x6ed9eba1, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_H, c, d, a, b, MD4_GET(5) + 0x6ed9eba1, 11)
    MD4_ಹೆಜ್ಜೆ(MD4_H, b, c, d, a, MD4_GET(13) + 0x6ed9eba1, 15)
    MD4_ಹೆಜ್ಜೆ(MD4_H, a, b, c, d, MD4_GET(3) + 0x6ed9eba1, 3)
    MD4_ಹೆಜ್ಜೆ(MD4_H, d, a, b, c, MD4_GET(11) + 0x6ed9eba1, 9)
    MD4_ಹೆಜ್ಜೆ(MD4_H, c, d, a, b, MD4_GET(7) + 0x6ed9eba1, 11)
    MD4_ಹೆಜ್ಜೆ(MD4_H, b, c, d, a, MD4_GET(15) + 0x6ed9eba1, 15)

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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳಹರಿವು, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
  uint32_t saved_lo;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಳಸಿದ;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ಆದರೆ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  ಬಳಸಿದ = saved_lo & 0x3f;

  ಆದರೆ(ಬಳಸಿದ) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಲಭ್ಯ = 64 - ಬಳಸಿದ;

    ಆದರೆ(len < ಲಭ್ಯ) {
      ಸ್ಮರಣೆ_ನಕಲು(&ctx->ಬಫರ್[ಬಳಸಿದ], ಒಳಹರಿವು, len);
      ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಮರಣೆ_ನಕಲು(&ctx->ಬಫರ್[ಬಳಸಿದ], ಒಳಹರಿವು, ಲಭ್ಯ);
    ಒಳಹರಿವು = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಒಳಹರಿವು + ಲಭ್ಯ;
    len -= ಲಭ್ಯ;
    my_md4_body(ctx, ctx->ಬಫರ್, 64);
  }

  ಆದರೆ(len >= 64) {
    ಒಳಹರಿವು = my_md4_body(ctx, ಒಳಹರಿವು, len & ~(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x3f);
    len &= 0x3f;
  }

  ಸ್ಮರಣೆ_ನಕಲು(ctx->ಬಫರ್, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *digest, my_md4_ctx *ctx)
{
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಳಸಿದ, ಲಭ್ಯ;

  ಬಳಸಿದ = ctx->lo & 0x3f;

  ctx->ಬಫರ್[ಬಳಸಿದ++] = 0x80;

  ಲಭ್ಯ = 64 - ಬಳಸಿದ;

  ಆದರೆ(ಲಭ್ಯ < 8) {
    memset(&ctx->ಬಫರ್[ಬಳಸಿದ], 0, ಲಭ್ಯ);
    my_md4_body(ctx, ctx->ಬಫರ್, 64);
    ಬಳಸಿದ = 0;
    ಲಭ್ಯ = 64;
  }

  memset(&ctx->ಬಫರ್[ಬಳಸಿದ], 0, ಲಭ್ಯ - 8);

  ctx->lo <<= 3;
  ctx->ಬಫರ್[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->ಬಫರ್[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->ಬಫರ್[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->ಬಫರ್[59] = curlx_ultouc((ctx->lo >> 24) & 0xff);
  ctx->ಬಫರ್[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->ಬಫರ್[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->ಬಫರ್[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->ಬಫರ್[63] = curlx_ultouc(ctx->hi >> 24);

  my_md4_body(ctx, ctx->ಬಫರ್, 64);

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

CURLcode Curl_md4it(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output,
                    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ size_t len)
{
  my_md4_ctx ctx;
  ಆದರೆ(!my_md4_init(&ctx))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  my_md4_update(&ctx, ಒಳಹರಿವು, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_NTLM_CORE */
