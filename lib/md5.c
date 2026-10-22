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

#ਜੇ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_md5.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_hmac.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/opensslconf.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WOLFSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MBEDTLS
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/version.h>
#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x03020000
#ਗਲਤੀ "mbedTLS 3.2.0 or greater required"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <psa/crypto_config.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GNUTLS
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/md5.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/version.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ md5_ctx my_md5_ctx;

ਸਥਿਰ CURLcode my_md5_init(ਖਾਲੀ *ctx)
{
  md5_init(ctx);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_md5_update(ਖਾਲੀ *ctx,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  md5_update(ctx, len, ਨਿਵੇਸ਼);
}

ਸਥਿਰ ਖਾਲੀ my_md5_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ctx)
{
#ਜੇ NETTLE_ਸੰਸਕਰਣ_MAJOR >= 4
  md5_digest(ctx, digest);
#ਨਹੀਂਤਾਂ
  md5_digest(ctx, MD5_DIGEST_LEN, digest);
#ਜੇ_ਅੰਤ
}

#ਜਾਂਜੇ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD5)
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/evp.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ EVP_MD_CTX *my_md5_ctx;

ਸਥਿਰ CURLcode my_md5_init(ਖਾਲੀ *ਵਿੱਚ)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)ਵਿੱਚ;
  *ctx = EVP_MD_CTX_new();
  ਜੇ(!*ctx)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!EVP_DigestInit_ex(*ctx, EVP_md5(), ਨਿਰਾ)) {
    EVP_MD_CTX_free(*ctx);
    *ctx = ਨਿਰਾ;
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_md5_update(ਖਾਲੀ *ਵਿੱਚ,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)ਵਿੱਚ;
  (ਖਾਲੀ)EVP_DigestUpdate(*ctx, ਨਿਵੇਸ਼, len);
}

ਸਥਿਰ ਖਾਲੀ my_md5_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ਵਿੱਚ)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)ਵਿੱਚ;
  (ਖਾਲੀ)EVP_DigestFinal_ex(*ctx, digest, ਨਿਰਾ);
  EVP_MD_CTX_free(*ctx);
  *ctx = ਨਿਰਾ;
}

#ਜਾਂਜੇ defined(USE_WOLFSSL) && !defined(NO_MD5)
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/wolfcrypt/md5.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ wc_Md5 my_md5_ctx;

ਸਥਿਰ CURLcode my_md5_init(ਖਾਲੀ *ctx)
{
  ਜੇ(wc_InitMd5(ctx))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_md5_update(ਖਾਲੀ *ctx,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  (ਖਾਲੀ)wc_Md5Update(ctx, ਨਿਵੇਸ਼, (word32)len);
}

ਸਥਿਰ ਖਾਲੀ my_md5_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ctx)
{
  (ਖਾਲੀ)wc_Md5Final(ctx, digest);
}

#ਜਾਂਜੇ defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_MD5) && PSA_WANT_ALG_MD5
#ਅਨ੍ਤਰ੍ਭਾਵ <psa/crypto.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ psa_hash_operation_t my_md5_ctx;

ਸਥਿਰ CURLcode my_md5_init(ਖਾਲੀ *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  ਜੇ(psa_hash_ਤਿਆਰੀ(pctx, PSA_ALG_MD5) != PSA_SUCCESS)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_md5_update(ਖਾਲੀ *ctx,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  (ਖਾਲੀ)psa_hash_update(ctx, ਨਿਵੇਸ਼, len);
}

ਸਥਿਰ ਖਾਲੀ my_md5_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ctx)
{
  size_t actual_ਲੰਮਾਈ;
  (ਖਾਲੀ)psa_hash_ਅੰਤ(ctx, digest, MD5_DIGEST_LEN, &actual_ਲੰਮਾਈ);
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

/* For Apple operating systems: CommonCrypto has the functions we need.
   These functions are available on Tiger and later, as well as iOS 2.0
   and later. If you are building for an older cat, well, sorry.

   Declaring the functions as static like this seems to be a bit more
   reliable than defining COMMON_DIGEST_FOR_OPENSSL on older cats. */
#  define my_md5_ctx CC_MD5_CTX

ਸਥਿਰ CURLcode my_md5_init(ਖਾਲੀ *ctx)
{
  (ਖਾਲੀ)CC_MD5_Init(ctx);  /* always returns 1 */
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_md5_update(ਖਾਲੀ *ctx,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  CC_MD5_Update(ctx, ਨਿਵੇਸ਼, len);
}

ਸਥਿਰ ਖਾਲੀ my_md5_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ctx)
{
  CC_MD5_Final(digest, ctx);
}

#ਜਾਂਜੇ defined(USE_WIN32_CRYPTO)
#ਅਨ੍ਤਰ੍ਭਾਵ <wincrypt.h>

ਬਣਤਰ md5_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ md5_ctx my_md5_ctx;

ਸਥਿਰ CURLcode my_md5_init(ਖਾਲੀ *ਵਿੱਚ)
{
  my_md5_ctx *ctx = (my_md5_ctx *)ਵਿੱਚ;
  ਜੇ(!CryptAcquireContext(&ctx->hCryptProv, ਨਿਰਾ, ਨਿਰਾ, PROV_RSA_ਭਰਿਆ,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!CryptCreateHash(ctx->hCryptProv, CALG_MD5, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_md5_update(ਖਾਲੀ *ਵਿੱਚ,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  my_md5_ctx *ctx = ਵਿੱਚ;
  Crypt_Hash_ਅੰਕੜੇ(ctx->hHash, (ਅਚਲ BYTE *)ਨਿਵੇਸ਼, len, 0);
}

ਸਥਿਰ ਖਾਲੀ my_md5_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ਵਿੱਚ)
{
  my_md5_ctx *ctx = (my_md5_ctx *)ਵਿੱਚ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾਈ = 0;
  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ਨਿਰਾ, &ਲੰਮਾਈ, 0);
  ਜੇ(ਲੰਮਾਈ == MD5_DIGEST_LEN)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &ਲੰਮਾਈ, 0);
  ਜੇ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);
  ਜੇ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ਨਹੀਂਤਾਂ

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

ਬਣਤਰ md5_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਬਫਰ[64];
  uint32_t ਖੰਡ[16];
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ md5_ctx my_md5_ctx;

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, like in Colin Plumb's
 * implementation.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_G(x, y, z)                  ((y) ^ ((z) & ((x) ^ (y))))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_H(x, y, z)                  (((x) ^ (y)) ^ (z))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_H2(x, y, z)                 ((x) ^ ((y) ^ (z)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_I(x, y, z)                  ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_ਕਦਮ(f, a, b, c, d, x, t, s) \
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
#ਜੇ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_ਸੈੱਟ(n) (*(ਅਚਲ uint32_t *)(ਅਚਲ ਖਾਲੀ *)&ptr[(n) * 4])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_GET(n) MD5_ਸੈੱਟ(n)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_ਸੈੱਟ(n) (ctx->ਖੰਡ[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_GET(n) ctx->ਖੰਡ[n]
#ਜੇ_ਅੰਤ

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
ਸਥਿਰ ਅਚਲ ਖਾਲੀ *my_md5_body(my_md5_ctx *ctx,
                               ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮਾਪ)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ptr;
  uint32_t a, b, c, d;

  ptr = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਅੰਕੜੇ;

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
    MD5_ਕਦਮ(MD5_F, a, b, c, d, MD5_ਸੈੱਟ(0), 0xd76aa478, 7)
    MD5_ਕਦਮ(MD5_F, d, a, b, c, MD5_ਸੈੱਟ(1), 0xe8c7b756, 12)
    MD5_ਕਦਮ(MD5_F, c, d, a, b, MD5_ਸੈੱਟ(2), 0x242070db, 17)
    MD5_ਕਦਮ(MD5_F, b, c, d, a, MD5_ਸੈੱਟ(3), 0xc1bdceee, 22)
    MD5_ਕਦਮ(MD5_F, a, b, c, d, MD5_ਸੈੱਟ(4), 0xf57c0faf, 7)
    MD5_ਕਦਮ(MD5_F, d, a, b, c, MD5_ਸੈੱਟ(5), 0x4787c62a, 12)
    MD5_ਕਦਮ(MD5_F, c, d, a, b, MD5_ਸੈੱਟ(6), 0xa8304613, 17)
    MD5_ਕਦਮ(MD5_F, b, c, d, a, MD5_ਸੈੱਟ(7), 0xfd469501, 22)
    MD5_ਕਦਮ(MD5_F, a, b, c, d, MD5_ਸੈੱਟ(8), 0x698098d8, 7)
    MD5_ਕਦਮ(MD5_F, d, a, b, c, MD5_ਸੈੱਟ(9), 0x8b44f7af, 12)
    MD5_ਕਦਮ(MD5_F, c, d, a, b, MD5_ਸੈੱਟ(10), 0xffff5bb1, 17)
    MD5_ਕਦਮ(MD5_F, b, c, d, a, MD5_ਸੈੱਟ(11), 0x895cd7be, 22)
    MD5_ਕਦਮ(MD5_F, a, b, c, d, MD5_ਸੈੱਟ(12), 0x6b901122, 7)
    MD5_ਕਦਮ(MD5_F, d, a, b, c, MD5_ਸੈੱਟ(13), 0xfd987193, 12)
    MD5_ਕਦਮ(MD5_F, c, d, a, b, MD5_ਸੈੱਟ(14), 0xa679438e, 17)
    MD5_ਕਦਮ(MD5_F, b, c, d, a, MD5_ਸੈੱਟ(15), 0x49b40821, 22)

    /* Round 2 */
    MD5_ਕਦਮ(MD5_G, a, b, c, d, MD5_GET(1), 0xf61e2562, 5)
    MD5_ਕਦਮ(MD5_G, d, a, b, c, MD5_GET(6), 0xc040b340, 9)
    MD5_ਕਦਮ(MD5_G, c, d, a, b, MD5_GET(11), 0x265e5a51, 14)
    MD5_ਕਦਮ(MD5_G, b, c, d, a, MD5_GET(0), 0xe9b6c7aa, 20)
    MD5_ਕਦਮ(MD5_G, a, b, c, d, MD5_GET(5), 0xd62f105d, 5)
    MD5_ਕਦਮ(MD5_G, d, a, b, c, MD5_GET(10), 0x02441453, 9)
    MD5_ਕਦਮ(MD5_G, c, d, a, b, MD5_GET(15), 0xd8a1e681, 14)
    MD5_ਕਦਮ(MD5_G, b, c, d, a, MD5_GET(4), 0xe7d3fbc8, 20)
    MD5_ਕਦਮ(MD5_G, a, b, c, d, MD5_GET(9), 0x21e1cde6, 5)
    MD5_ਕਦਮ(MD5_G, d, a, b, c, MD5_GET(14), 0xc33707d6, 9)
    MD5_ਕਦਮ(MD5_G, c, d, a, b, MD5_GET(3), 0xf4d50d87, 14)
    MD5_ਕਦਮ(MD5_G, b, c, d, a, MD5_GET(8), 0x455a14ed, 20)
    MD5_ਕਦਮ(MD5_G, a, b, c, d, MD5_GET(13), 0xa9e3e905, 5)
    MD5_ਕਦਮ(MD5_G, d, a, b, c, MD5_GET(2), 0xfcefa3f8, 9)
    MD5_ਕਦਮ(MD5_G, c, d, a, b, MD5_GET(7), 0x676f02d9, 14)
    MD5_ਕਦਮ(MD5_G, b, c, d, a, MD5_GET(12), 0x8d2a4c8a, 20)

    /* Round 3 */
    MD5_ਕਦਮ(MD5_H, a, b, c, d, MD5_GET(5), 0xfffa3942, 4)
    MD5_ਕਦਮ(MD5_H2, d, a, b, c, MD5_GET(8), 0x8771f681, 11)
    MD5_ਕਦਮ(MD5_H, c, d, a, b, MD5_GET(11), 0x6d9d6122, 16)
    MD5_ਕਦਮ(MD5_H2, b, c, d, a, MD5_GET(14), 0xfde5380c, 23)
    MD5_ਕਦਮ(MD5_H, a, b, c, d, MD5_GET(1), 0xa4beea44, 4)
    MD5_ਕਦਮ(MD5_H2, d, a, b, c, MD5_GET(4), 0x4bdecfa9, 11)
    MD5_ਕਦਮ(MD5_H, c, d, a, b, MD5_GET(7), 0xf6bb4b60, 16)
    MD5_ਕਦਮ(MD5_H2, b, c, d, a, MD5_GET(10), 0xbebfbc70, 23)
    MD5_ਕਦਮ(MD5_H, a, b, c, d, MD5_GET(13), 0x289b7ec6, 4)
    MD5_ਕਦਮ(MD5_H2, d, a, b, c, MD5_GET(0), 0xeaa127fa, 11)
    MD5_ਕਦਮ(MD5_H, c, d, a, b, MD5_GET(3), 0xd4ef3085, 16)
    MD5_ਕਦਮ(MD5_H2, b, c, d, a, MD5_GET(6), 0x04881d05, 23)
    MD5_ਕਦਮ(MD5_H, a, b, c, d, MD5_GET(9), 0xd9d4d039, 4)
    MD5_ਕਦਮ(MD5_H2, d, a, b, c, MD5_GET(12), 0xe6db99e5, 11)
    MD5_ਕਦਮ(MD5_H, c, d, a, b, MD5_GET(15), 0x1fa27cf8, 16)
    MD5_ਕਦਮ(MD5_H2, b, c, d, a, MD5_GET(2), 0xc4ac5665, 23)

    /* Round 4 */
    MD5_ਕਦਮ(MD5_I, a, b, c, d, MD5_GET(0), 0xf4292244, 6)
    MD5_ਕਦਮ(MD5_I, d, a, b, c, MD5_GET(7), 0x432aff97, 10)
    MD5_ਕਦਮ(MD5_I, c, d, a, b, MD5_GET(14), 0xab9423a7, 15)
    MD5_ਕਦਮ(MD5_I, b, c, d, a, MD5_GET(5), 0xfc93a039, 21)
    MD5_ਕਦਮ(MD5_I, a, b, c, d, MD5_GET(12), 0x655b59c3, 6)
    MD5_ਕਦਮ(MD5_I, d, a, b, c, MD5_GET(3), 0x8f0ccc92, 10)
    MD5_ਕਦਮ(MD5_I, c, d, a, b, MD5_GET(10), 0xffeff47d, 15)
    MD5_ਕਦਮ(MD5_I, b, c, d, a, MD5_GET(1), 0x85845dd1, 21)
    MD5_ਕਦਮ(MD5_I, a, b, c, d, MD5_GET(8), 0x6fa87e4f, 6)
    MD5_ਕਦਮ(MD5_I, d, a, b, c, MD5_GET(15), 0xfe2ce6e0, 10)
    MD5_ਕਦਮ(MD5_I, c, d, a, b, MD5_GET(6), 0xa3014314, 15)
    MD5_ਕਦਮ(MD5_I, b, c, d, a, MD5_GET(13), 0x4e0811a1, 21)
    MD5_ਕਦਮ(MD5_I, a, b, c, d, MD5_GET(4), 0xf7537e82, 6)
    MD5_ਕਦਮ(MD5_I, d, a, b, c, MD5_GET(11), 0xbd3af235, 10)
    MD5_ਕਦਮ(MD5_I, c, d, a, b, MD5_GET(2), 0x2ad7d2bb, 15)
    MD5_ਕਦਮ(MD5_I, b, c, d, a, MD5_GET(9), 0xeb86d391, 21)

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

ਸਥਿਰ CURLcode my_md5_init(ਖਾਲੀ *ਵਿੱਚ)
{
  my_md5_ctx *ctx = (my_md5_ctx *)ਵਿੱਚ;
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ my_md5_update(ਖਾਲੀ *ਵਿੱਚ,
                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  uint32_t saved_lo;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਰਤਿਆ;
  my_md5_ctx *ctx = (my_md5_ctx *)ਵਿੱਚ;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ਜੇ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  ਵਰਤਿਆ = saved_lo & 0x3f;

  ਜੇ(ਵਰਤਿਆ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਉਪਲਬਧ = 64 - ਵਰਤਿਆ;

    ਜੇ(len < ਉਪਲਬਧ) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ctx->ਬਫਰ[ਵਰਤਿਆ], ਨਿਵੇਸ਼, len);
      ਵਾਪਸ;
    }

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ctx->ਬਫਰ[ਵਰਤਿਆ], ਨਿਵੇਸ਼, ਉਪਲਬਧ);
    ਨਿਵੇਸ਼ = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਨਿਵੇਸ਼ + ਉਪਲਬਧ;
    len -= ਉਪਲਬਧ;
    my_md5_body(ctx, ctx->ਬਫਰ, 64);
  }

  ਜੇ(len >= 64) {
    ਨਿਵੇਸ਼ = my_md5_body(ctx, ਨਿਵੇਸ਼, len & ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x3f);
    len &= 0x3f;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ctx->ਬਫਰ, ਨਿਵੇਸ਼, len);
}

ਸਥਿਰ ਖਾਲੀ my_md5_final(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *digest, ਖਾਲੀ *ਵਿੱਚ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਰਤਿਆ, ਉਪਲਬਧ;
  my_md5_ctx *ctx = (my_md5_ctx *)ਵਿੱਚ;

  ਵਰਤਿਆ = ctx->lo & 0x3f;

  ctx->ਬਫਰ[ਵਰਤਿਆ++] = 0x80;

  ਉਪਲਬਧ = 64 - ਵਰਤਿਆ;

  ਜੇ(ਉਪਲਬਧ < 8) {
    memset(&ctx->ਬਫਰ[ਵਰਤਿਆ], 0, ਉਪਲਬਧ);
    my_md5_body(ctx, ctx->ਬਫਰ, 64);
    ਵਰਤਿਆ = 0;
    ਉਪਲਬਧ = 64;
  }

  memset(&ctx->ਬਫਰ[ਵਰਤਿਆ], 0, ਉਪਲਬਧ - 8);

  ctx->lo <<= 3;
  ctx->ਬਫਰ[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->ਬਫਰ[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->ਬਫਰ[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->ਬਫਰ[59] = curlx_ultouc(ctx->lo >> 24);
  ctx->ਬਫਰ[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->ਬਫਰ[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->ਬਫਰ[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->ਬਫਰ[63] = curlx_ultouc(ctx->hi >> 24);

  my_md5_body(ctx, ctx->ਬਫਰ, 64);

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

ਅਚਲ ਬਣਤਰ HMAC_params Curl_HMAC_MD5 = {
  my_md5_init,        /* Hash initialization function. */
  my_md5_update,      /* Hash update function. */
  my_md5_final,       /* Hash computation end function. */
  ਆਕਾਰ(my_md5_ctx), /* Size of hash context structure. */
  64,                 /* Maximum key length. */
  MD5_DIGEST_LEN      /* Result size. */
};

ਅਚਲ ਬਣਤਰ MD5_params Curl_DIGEST_MD5 = {
  my_md5_init,        /* Digest initialization function */
  my_md5_update,      /* Digest update function */
  my_md5_final,       /* Digest computation end function */
  ਆਕਾਰ(my_md5_ctx), /* Size of digest context struct */
  MD5_DIGEST_LEN      /* Result size */
};

/*
 * @unittest: 1601
 * Returns CURLE_OK on success.
 */
CURLcode Curl_md5it(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output,
                    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, size_t len)
{
  CURLcode ਨਤੀਜਾ;
  my_md5_ctx ctx;

  ਨਤੀਜਾ = my_md5_init(&ctx);
  ਜੇ(!ਨਤੀਜਾ) {
    ਕਰੋ {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ilen = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURLMIN(len, UINT_MAX);
      my_md5_update(&ctx, ਨਿਵੇਸ਼, ilen);
      ਨਿਵੇਸ਼ += ilen;
      len -= ilen;
    } ਜਦੋਂ(len);
    my_md5_final(output, &ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ MD5_context *Curl_MD5_init(ਅਚਲ ਬਣਤਰ MD5_params *md5params)
{
  ਬਣਤਰ MD5_context *ctxt;

  /* Create MD5 context */
  ctxt = curlx_malloc(ਆਕਾਰ(*ctxt));

  ਜੇ(!ctxt)
    ਵਾਪਸ ctxt;

  ctxt->md5_hashctx = curlx_malloc(md5params->md5_ctxtsize);

  ਜੇ(!ctxt->md5_hashctx) {
    curlx_free(ctxt);
    ਵਾਪਸ ਨਿਰਾ;
  }

  ctxt->md5_hash = md5params;

  ਜੇ((*md5params->md5_init_func)(ctxt->md5_hashctx)) {
    curlx_free(ctxt->md5_hashctx);
    curlx_free(ctxt);
    ਵਾਪਸ ਨਿਰਾ;
  }

  ਵਾਪਸ ctxt;
}

CURLcode Curl_MD5_update(ਬਣਤਰ MD5_context *context,
                         ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  (*context->md5_hash->md5_update_func)(context->md5_hashctx, ਨਿਵੇਸ਼, len);

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_MD5_final(ਬਣਤਰ MD5_context *context, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਤੀਜਾ)
{
  (*context->md5_hash->md5_final_func)(ਨਤੀਜਾ, context->md5_hashctx);

  curlx_free(context->md5_hashctx);
  curlx_free(context);

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅੰਤ /* Using NTLM (without SSPI) || Digest */
