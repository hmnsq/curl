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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_NTLM_CORE

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_md4.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/opensslv.h>
#ਜੇ OPENSSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENSSL_NO_MD4
#ਨਹੀਂਤਾਂ
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/opensslconf.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* USE_OPENSSL */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WOLFSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/evp.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ EVP_MD_CTX *my_md4_ctx;

ਸਥਿਰ ਪੂਰਨ my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  ਜੇ(!*ctx)
    ਵਾਪਸ 0;

  ਜੇ(!EVP_DigestInit_ex(*ctx, EVP_md4(), ਨਿਰਾ)) {
    EVP_MD_CTX_free(*ctx);
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ my_md4_update(my_md4_ctx *ctx,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  (ਖਾਲੀ)EVP_DigestUpdate(*ctx, ਨਿਵੇਸ਼, len);
}

ਸਥਿਰ ਖਾਲੀ my_md4_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, my_md4_ctx *ctx)
{
  (ਖਾਲੀ)EVP_DigestFinal_ex(*ctx, digest, ਨਿਰਾ);
  EVP_MD_CTX_free(*ctx);
}

#ਜਾਂਜੇ defined(USE_WOLFSSL) && !defined(NO_MD4)
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/wolfcrypt/md4.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/version.h>

#ਜੇ LIBWOLFSSL_ਸੰਸਕਰਣ_HEX >= 0x05007006
ਕਿਸਮਪਰਿਭਾਸ਼ਾ wc_Md4 my_md4_ctx;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ Md4 my_md4_ctx;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ my_md4_update(my_md4_ctx *ctx,
                          ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
  wc_Md4Update(ctx, ਨਿਵੇਸ਼, (word32)len);
}

ਸਥਿਰ ਖਾਲੀ my_md4_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#ਜਾਂਜੇ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#ਅਨ੍ਤਰ੍ਭਾਵ <CommonCrypto/CommonDigest.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CC_MD4_CTX my_md4_ctx;

ਸਥਿਰ ਪੂਰਨ my_md4_init(my_md4_ctx *ctx)
{
  ਵਾਪਸ CC_MD4_Init(ctx);  /* always returns 1 */
}

ਸਥਿਰ ਖਾਲੀ my_md4_update(my_md4_ctx *ctx,
                          ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
  (ਖਾਲੀ)CC_MD4_Update(ctx, ਨਿਵੇਸ਼, (CC_LONG)len);
}

ਸਥਿਰ ਖਾਲੀ my_md4_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, my_md4_ctx *ctx)
{
  (ਖਾਲੀ)CC_MD4_Final(digest, ctx);
}

#ਜਾਂਜੇ defined(USE_WIN32_CRYPTO)
#ਅਨ੍ਤਰ੍ਭਾਵ <wincrypt.h>

ਬਣਤਰ md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ md4_ctx my_md4_ctx;

ਸਥਿਰ ਪੂਰਨ my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  ਜੇ(!CryptAcquireContext(&ctx->hCryptProv, ਨਿਰਾ, ਨਿਰਾ, PROV_RSA_ਭਰਿਆ,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ਵਾਪਸ 0;

  ਜੇ(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ my_md4_update(my_md4_ctx *ctx,
                          ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
  Crypt_Hash_ਅੰਕੜੇ(ctx->hHash, (ਅਚਲ BYTE *)ਨਿਵੇਸ਼, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)len, 0);
}

ਸਥਿਰ ਖਾਲੀ my_md4_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, my_md4_ctx *ctx)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾਈ = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ਨਿਰਾ, &ਲੰਮਾਈ, 0);
  ਜੇ(ਲੰਮਾਈ == MD4_DIGEST_ਲੰਮਾਈ)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &ਲੰਮਾਈ, 0);

  ਜੇ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ਜੇ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ਜਾਂਜੇ defined(USE_GNUTLS)
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/md4.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/version.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ md4_ctx my_md4_ctx;

ਸਥਿਰ ਪੂਰਨ my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ my_md4_update(my_md4_ctx *ctx,
                          ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
  md4_update(ctx, len, ਨਿਵੇਸ਼);
}

ਸਥਿਰ ਖਾਲੀ my_md4_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, my_md4_ctx *ctx)
{
#ਜੇ NETTLE_ਸੰਸਕਰਣ_MAJOR >= 4
  md4_digest(ctx, digest);
#ਨਹੀਂਤਾਂ
  md4_digest(ctx, MD4_DIGEST_ਮਾਪ, digest);
#ਜੇ_ਅੰਤ
}

#ਨਹੀਂਤਾਂ
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

ਬਣਤਰ md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਬਫਰ[64];
  uint32_t ਖੰਡ[16];
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ md4_ctx my_md4_ctx;

/*
 * The basic MD4 functions.
 *
 * F and G are optimized compared to their RFC 1320 definitions, with the
 * optimization for F borrowed from Colin Plumb's MD5 implementation.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_G(x, y, z)                  (((x) & ((y) | (z))) | ((y) & (z)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_H(x, y, z)                  ((x) ^ (y) ^ (z))

/*
 * The MD4 transformation for all three rounds.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_ਕਦਮ(f, a, b, c, d, x, s) \
  (a) += f(b, c, d) + (x); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is an optimization. Nothing will break if it does not work.
 */
#ਜੇ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_ਸੈੱਟ(n) (*(ਅਚਲ uint32_t *)(ਅਚਲ ਖਾਲੀ *)&ptr[(n) * 4])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_GET(n) MD4_ਸੈੱਟ(n)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_ਸੈੱਟ(n) (ctx->ਖੰਡ[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_GET(n) ctx->ਖੰਡ[n]
#ਜੇ_ਅੰਤ

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
ਸਥਿਰ ਅਚਲ ਖਾਲੀ *my_md4_body(my_md4_ctx *ctx,
                               ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ptr;
  uint32_t a, b, c, d;

  ptr = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਨਿਵੇਸ਼;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  ਕਰੋ {
    uint32_t saved_a, saved_b, saved_c, saved_d;

    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    MD4_ਕਦਮ(MD4_F, a, b, c, d, MD4_ਸੈੱਟ(0), 3)
    MD4_ਕਦਮ(MD4_F, d, a, b, c, MD4_ਸੈੱਟ(1), 7)
    MD4_ਕਦਮ(MD4_F, c, d, a, b, MD4_ਸੈੱਟ(2), 11)
    MD4_ਕਦਮ(MD4_F, b, c, d, a, MD4_ਸੈੱਟ(3), 19)
    MD4_ਕਦਮ(MD4_F, a, b, c, d, MD4_ਸੈੱਟ(4), 3)
    MD4_ਕਦਮ(MD4_F, d, a, b, c, MD4_ਸੈੱਟ(5), 7)
    MD4_ਕਦਮ(MD4_F, c, d, a, b, MD4_ਸੈੱਟ(6), 11)
    MD4_ਕਦਮ(MD4_F, b, c, d, a, MD4_ਸੈੱਟ(7), 19)
    MD4_ਕਦਮ(MD4_F, a, b, c, d, MD4_ਸੈੱਟ(8), 3)
    MD4_ਕਦਮ(MD4_F, d, a, b, c, MD4_ਸੈੱਟ(9), 7)
    MD4_ਕਦਮ(MD4_F, c, d, a, b, MD4_ਸੈੱਟ(10), 11)
    MD4_ਕਦਮ(MD4_F, b, c, d, a, MD4_ਸੈੱਟ(11), 19)
    MD4_ਕਦਮ(MD4_F, a, b, c, d, MD4_ਸੈੱਟ(12), 3)
    MD4_ਕਦਮ(MD4_F, d, a, b, c, MD4_ਸੈੱਟ(13), 7)
    MD4_ਕਦਮ(MD4_F, c, d, a, b, MD4_ਸੈੱਟ(14), 11)
    MD4_ਕਦਮ(MD4_F, b, c, d, a, MD4_ਸੈੱਟ(15), 19)

    /* Round 2 */
    MD4_ਕਦਮ(MD4_G, a, b, c, d, MD4_GET(0) + 0x5a827999, 3)
    MD4_ਕਦਮ(MD4_G, d, a, b, c, MD4_GET(4) + 0x5a827999, 5)
    MD4_ਕਦਮ(MD4_G, c, d, a, b, MD4_GET(8) + 0x5a827999, 9)
    MD4_ਕਦਮ(MD4_G, b, c, d, a, MD4_GET(12) + 0x5a827999, 13)
    MD4_ਕਦਮ(MD4_G, a, b, c, d, MD4_GET(1) + 0x5a827999, 3)
    MD4_ਕਦਮ(MD4_G, d, a, b, c, MD4_GET(5) + 0x5a827999, 5)
    MD4_ਕਦਮ(MD4_G, c, d, a, b, MD4_GET(9) + 0x5a827999, 9)
    MD4_ਕਦਮ(MD4_G, b, c, d, a, MD4_GET(13) + 0x5a827999, 13)
    MD4_ਕਦਮ(MD4_G, a, b, c, d, MD4_GET(2) + 0x5a827999, 3)
    MD4_ਕਦਮ(MD4_G, d, a, b, c, MD4_GET(6) + 0x5a827999, 5)
    MD4_ਕਦਮ(MD4_G, c, d, a, b, MD4_GET(10) + 0x5a827999, 9)
    MD4_ਕਦਮ(MD4_G, b, c, d, a, MD4_GET(14) + 0x5a827999, 13)
    MD4_ਕਦਮ(MD4_G, a, b, c, d, MD4_GET(3) + 0x5a827999, 3)
    MD4_ਕਦਮ(MD4_G, d, a, b, c, MD4_GET(7) + 0x5a827999, 5)
    MD4_ਕਦਮ(MD4_G, c, d, a, b, MD4_GET(11) + 0x5a827999, 9)
    MD4_ਕਦਮ(MD4_G, b, c, d, a, MD4_GET(15) + 0x5a827999, 13)

    /* Round 3 */
    MD4_ਕਦਮ(MD4_H, a, b, c, d, MD4_GET(0) + 0x6ed9eba1, 3)
    MD4_ਕਦਮ(MD4_H, d, a, b, c, MD4_GET(8) + 0x6ed9eba1, 9)
    MD4_ਕਦਮ(MD4_H, c, d, a, b, MD4_GET(4) + 0x6ed9eba1, 11)
    MD4_ਕਦਮ(MD4_H, b, c, d, a, MD4_GET(12) + 0x6ed9eba1, 15)
    MD4_ਕਦਮ(MD4_H, a, b, c, d, MD4_GET(2) + 0x6ed9eba1, 3)
    MD4_ਕਦਮ(MD4_H, d, a, b, c, MD4_GET(10) + 0x6ed9eba1, 9)
    MD4_ਕਦਮ(MD4_H, c, d, a, b, MD4_GET(6) + 0x6ed9eba1, 11)
    MD4_ਕਦਮ(MD4_H, b, c, d, a, MD4_GET(14) + 0x6ed9eba1, 15)
    MD4_ਕਦਮ(MD4_H, a, b, c, d, MD4_GET(1) + 0x6ed9eba1, 3)
    MD4_ਕਦਮ(MD4_H, d, a, b, c, MD4_GET(9) + 0x6ed9eba1, 9)
    MD4_ਕਦਮ(MD4_H, c, d, a, b, MD4_GET(5) + 0x6ed9eba1, 11)
    MD4_ਕਦਮ(MD4_H, b, c, d, a, MD4_GET(13) + 0x6ed9eba1, 15)
    MD4_ਕਦਮ(MD4_H, a, b, c, d, MD4_GET(3) + 0x6ed9eba1, 3)
    MD4_ਕਦਮ(MD4_H, d, a, b, c, MD4_GET(11) + 0x6ed9eba1, 9)
    MD4_ਕਦਮ(MD4_H, c, d, a, b, MD4_GET(7) + 0x6ed9eba1, 11)
    MD4_ਕਦਮ(MD4_H, b, c, d, a, MD4_GET(15) + 0x6ed9eba1, 15)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } ਜਦੋਂ(ਮਾਪ -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  ਵਾਪਸ ptr;
}

ਸਥਿਰ ਪੂਰਨ my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ my_md4_update(my_md4_ctx *ctx,
                          ਅਚਲ ਖਾਲੀ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
  uint32_t saved_lo;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਵਰਤਿਆ;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ਜੇ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  ਵਰਤਿਆ = saved_lo & 0x3f;

  ਜੇ(ਵਰਤਿਆ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਉਪਲਬਧ = 64 - ਵਰਤਿਆ;

    ਜੇ(len < ਉਪਲਬਧ) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ctx->ਬਫਰ[ਵਰਤਿਆ], ਨਿਵੇਸ਼, len);
      ਵਾਪਸ;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ctx->ਬਫਰ[ਵਰਤਿਆ], ਨਿਵੇਸ਼, ਉਪਲਬਧ);
    ਨਿਵੇਸ਼ = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਨਿਵੇਸ਼ + ਉਪਲਬਧ;
    len -= ਉਪਲਬਧ;
    my_md4_body(ctx, ctx->ਬਫਰ, 64);
  }

  ਜੇ(len >= 64) {
    ਨਿਵੇਸ਼ = my_md4_body(ctx, ਨਿਵੇਸ਼, len & ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x3f);
    len &= 0x3f;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ctx->ਬਫਰ, ਨਿਵੇਸ਼, len);
}

ਸਥਿਰ ਖਾਲੀ my_md4_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, my_md4_ctx *ctx)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਵਰਤਿਆ, ਉਪਲਬਧ;

  ਵਰਤਿਆ = ctx->lo & 0x3f;

  ctx->ਬਫਰ[ਵਰਤਿਆ++] = 0x80;

  ਉਪਲਬਧ = 64 - ਵਰਤਿਆ;

  ਜੇ(ਉਪਲਬਧ < 8) {
    memset(&ctx->ਬਫਰ[ਵਰਤਿਆ], 0, ਉਪਲਬਧ);
    my_md4_body(ctx, ctx->ਬਫਰ, 64);
    ਵਰਤਿਆ = 0;
    ਉਪਲਬਧ = 64;
  }

  memset(&ctx->ਬਫਰ[ਵਰਤਿਆ], 0, ਉਪਲਬਧ - 8);

  ctx->lo <<= 3;
  ctx->ਬਫਰ[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->ਬਫਰ[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->ਬਫਰ[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->ਬਫਰ[59] = curlx_ultouc((ctx->lo >> 24) & 0xff);
  ctx->ਬਫਰ[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->ਬਫਰ[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->ਬਫਰ[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->ਬਫਰ[63] = curlx_ultouc(ctx->hi >> 24);

  my_md4_body(ctx, ctx->ਬਫਰ, 64);

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

  memset(ctx, 0, ਆਕਾਰ(*ctx));
}

#ਜੇ_ਅੰਤ /* CRYPTO LIBS */

CURLcode Curl_md4it(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output,
                    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਅਚਲ size_t len)
{
  my_md4_ctx ctx;
  ਜੇ(!my_md4_init(&ctx))
    ਵਾਪਸ CURLE_FAILED_INIT;
  my_md4_update(&ctx, ਨਿਵੇਸ਼, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅੰਤ /* USE_CURL_NTLM_CORE */
