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

#اگر (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#شامل "curl_md5.h"
#شامل "curl_hmac.h"

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/opensslconf.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_WOLFSSL
#شامل <wolfssl/options.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_MBEDTLS
#شامل <mbedtls/version.h>
#اگر MBEDTLS_نسخہ_نمبر < 0x03020000
#غلطی "mbedTLS 3.2.0 or greater required"
#اگر_ختم
#شامل <psa/crypto_config.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_GNUTLS
#شامل <nettle/md5.h>
#شامل <nettle/version.h>

قسمتعریف ساخت md5_ctx my_md5_ctx;

ساکن CURLcode my_md5_init(خالی *ctx)
{
  md5_init(ctx);
  واپس CURLE_OK;
}

ساکن خالی my_md5_update(خالی *ctx,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  md5_update(ctx, len, داخلہ);
}

ساکن خالی my_md5_final(غیرعلامتی حرف *digest, خالی *ctx)
{
#اگر NETTLE_نسخہ_MAJOR >= 4
  md5_digest(ctx, digest);
#ورنہ
  md5_digest(ctx, MD5_DIGEST_LEN, digest);
#اگر_ختم
}

#وگرنہ_اگر defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD5)
#شامل <openssl/evp.h>

قسمتعریف EVP_MD_CTX *my_md5_ctx;

ساکن CURLcode my_md5_init(خالی *اندر)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)اندر;
  *ctx = EVP_MD_CTX_new();
  اگر(!*ctx)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(!EVP_DigestInit_ex(*ctx, EVP_md5(), تہی)) {
    EVP_MD_CTX_free(*ctx);
    *ctx = تہی;
    واپس CURLE_FAILED_INIT;
  }
  واپس CURLE_OK;
}

ساکن خالی my_md5_update(خالی *اندر,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)اندر;
  (خالی)EVP_DigestUpdate(*ctx, داخلہ, len);
}

ساکن خالی my_md5_final(غیرعلامتی حرف *digest, خالی *اندر)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)اندر;
  (خالی)EVP_DigestFinal_ex(*ctx, digest, تہی);
  EVP_MD_CTX_free(*ctx);
  *ctx = تہی;
}

#وگرنہ_اگر defined(USE_WOLFSSL) && !defined(NO_MD5)
#شامل <wolfssl/wolfcrypt/md5.h>

قسمتعریف wc_Md5 my_md5_ctx;

ساکن CURLcode my_md5_init(خالی *ctx)
{
  اگر(wc_InitMd5(ctx))
    واپس CURLE_برآمد_کا_حافظہ;
  واپس CURLE_OK;
}

ساکن خالی my_md5_update(خالی *ctx,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  (خالی)wc_Md5Update(ctx, داخلہ, (word32)len);
}

ساکن خالی my_md5_final(غیرعلامتی حرف *digest, خالی *ctx)
{
  (خالی)wc_Md5Final(ctx, digest);
}

#وگرنہ_اگر defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_MD5) && PSA_WANT_ALG_MD5
#شامل <psa/crypto.h>

قسمتعریف psa_hash_operation_t my_md5_ctx;

ساکن CURLcode my_md5_init(خالی *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  اگر(psa_hash_تیاری(pctx, PSA_ALG_MD5) != PSA_SUCCESS)
    واپس CURLE_برآمد_کا_حافظہ;
  واپس CURLE_OK;
}

ساکن خالی my_md5_update(خالی *ctx,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  (خالی)psa_hash_update(ctx, داخلہ, len);
}

ساکن خالی my_md5_final(غیرعلامتی حرف *digest, خالی *ctx)
{
  size_t actual_لمبائی;
  (خالی)psa_hash_اختتام(ctx, digest, MD5_DIGEST_LEN, &actual_لمبائی);
}

#وگرنہ_اگر (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#شامل <CommonCrypto/CommonDigest.h>

/* For Apple operating systems: CommonCrypto has the functions we need.
   These functions are available on Tiger and later, as well as iOS 2.0
   and later. If you are building for an older cat, well, sorry.

   Declaring the functions as static like this seems to be a bit more
   reliable than defining COMMON_DIGEST_FOR_OPENSSL on older cats. */
#  define my_md5_ctx CC_MD5_CTX

ساکن CURLcode my_md5_init(خالی *ctx)
{
  (خالی)CC_MD5_Init(ctx);  /* always returns 1 */
  واپس CURLE_OK;
}

ساکن خالی my_md5_update(خالی *ctx,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  CC_MD5_Update(ctx, داخلہ, len);
}

ساکن خالی my_md5_final(غیرعلامتی حرف *digest, خالی *ctx)
{
  CC_MD5_Final(digest, ctx);
}

#وگرنہ_اگر defined(USE_WIN32_CRYPTO)
#شامل <wincrypt.h>

ساخت md5_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
قسمتعریف ساخت md5_ctx my_md5_ctx;

ساکن CURLcode my_md5_init(خالی *اندر)
{
  my_md5_ctx *ctx = (my_md5_ctx *)اندر;
  اگر(!CryptAcquireContext(&ctx->hCryptProv, تہی, تہی, PROV_RSA_بھرا,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(!CryptCreateHash(ctx->hCryptProv, CALG_MD5, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    واپس CURLE_FAILED_INIT;
  }

  واپس CURLE_OK;
}

ساکن خالی my_md5_update(خالی *اندر,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  my_md5_ctx *ctx = اندر;
  Crypt_Hash_ڈیٹا(ctx->hHash, (ثابت BYTE *)داخلہ, len, 0);
}

ساکن خالی my_md5_final(غیرعلامتی حرف *digest, خالی *اندر)
{
  my_md5_ctx *ctx = (my_md5_ctx *)اندر;
  غیرعلامتی طویل لمبائی = 0;
  CryptGetHashParam(ctx->hHash, HP_HASHVAL, تہی, &لمبائی, 0);
  اگر(لمبائی == MD5_DIGEST_LEN)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &لمبائی, 0);
  اگر(ctx->hHash)
    CryptDestroyHash(ctx->hHash);
  اگر(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ورنہ

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

ساخت md5_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  غیرعلامتی حرف بفر[64];
  uint32_t بلاک[16];
};
قسمتعریف ساخت md5_ctx my_md5_ctx;

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, like in Colin Plumb's
 * implementation.
 */
#تعریف_کرو MD5_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#تعریف_کرو MD5_G(x, y, z)                  ((y) ^ ((z) & ((x) ^ (y))))
#تعریف_کرو MD5_H(x, y, z)                  (((x) ^ (y)) ^ (z))
#تعریف_کرو MD5_H2(x, y, z)                 ((x) ^ ((y) ^ (z)))
#تعریف_کرو MD5_I(x, y, z)                  ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#تعریف_کرو MD5_قدم(f, a, b, c, d, x, t, s) \
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
#اگر defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#تعریف_کرو MD5_سیٹ(n) (*(ثابت uint32_t *)(ثابت خالی *)&ptr[(n) * 4])
#تعریف_کرو MD5_GET(n) MD5_سیٹ(n)
#ورنہ
#تعریف_کرو MD5_سیٹ(n) (ctx->بلاک[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#تعریف_کرو MD5_GET(n) ctx->بلاک[n]
#اگر_ختم

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
ساکن ثابت خالی *my_md5_body(my_md5_ctx *ctx,
                               ثابت خالی *ڈیٹا, غیرعلامتی طویل سائز)
{
  ثابت غیرعلامتی حرف *ptr;
  uint32_t a, b, c, d;

  ptr = (ثابت غیرعلامتی حرف *)ڈیٹا;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  کرو {
    uint32_t saved_a, saved_b, saved_c, saved_d;

    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    MD5_قدم(MD5_F, a, b, c, d, MD5_سیٹ(0), 0xd76aa478, 7)
    MD5_قدم(MD5_F, d, a, b, c, MD5_سیٹ(1), 0xe8c7b756, 12)
    MD5_قدم(MD5_F, c, d, a, b, MD5_سیٹ(2), 0x242070db, 17)
    MD5_قدم(MD5_F, b, c, d, a, MD5_سیٹ(3), 0xc1bdceee, 22)
    MD5_قدم(MD5_F, a, b, c, d, MD5_سیٹ(4), 0xf57c0faf, 7)
    MD5_قدم(MD5_F, d, a, b, c, MD5_سیٹ(5), 0x4787c62a, 12)
    MD5_قدم(MD5_F, c, d, a, b, MD5_سیٹ(6), 0xa8304613, 17)
    MD5_قدم(MD5_F, b, c, d, a, MD5_سیٹ(7), 0xfd469501, 22)
    MD5_قدم(MD5_F, a, b, c, d, MD5_سیٹ(8), 0x698098d8, 7)
    MD5_قدم(MD5_F, d, a, b, c, MD5_سیٹ(9), 0x8b44f7af, 12)
    MD5_قدم(MD5_F, c, d, a, b, MD5_سیٹ(10), 0xffff5bb1, 17)
    MD5_قدم(MD5_F, b, c, d, a, MD5_سیٹ(11), 0x895cd7be, 22)
    MD5_قدم(MD5_F, a, b, c, d, MD5_سیٹ(12), 0x6b901122, 7)
    MD5_قدم(MD5_F, d, a, b, c, MD5_سیٹ(13), 0xfd987193, 12)
    MD5_قدم(MD5_F, c, d, a, b, MD5_سیٹ(14), 0xa679438e, 17)
    MD5_قدم(MD5_F, b, c, d, a, MD5_سیٹ(15), 0x49b40821, 22)

    /* Round 2 */
    MD5_قدم(MD5_G, a, b, c, d, MD5_GET(1), 0xf61e2562, 5)
    MD5_قدم(MD5_G, d, a, b, c, MD5_GET(6), 0xc040b340, 9)
    MD5_قدم(MD5_G, c, d, a, b, MD5_GET(11), 0x265e5a51, 14)
    MD5_قدم(MD5_G, b, c, d, a, MD5_GET(0), 0xe9b6c7aa, 20)
    MD5_قدم(MD5_G, a, b, c, d, MD5_GET(5), 0xd62f105d, 5)
    MD5_قدم(MD5_G, d, a, b, c, MD5_GET(10), 0x02441453, 9)
    MD5_قدم(MD5_G, c, d, a, b, MD5_GET(15), 0xd8a1e681, 14)
    MD5_قدم(MD5_G, b, c, d, a, MD5_GET(4), 0xe7d3fbc8, 20)
    MD5_قدم(MD5_G, a, b, c, d, MD5_GET(9), 0x21e1cde6, 5)
    MD5_قدم(MD5_G, d, a, b, c, MD5_GET(14), 0xc33707d6, 9)
    MD5_قدم(MD5_G, c, d, a, b, MD5_GET(3), 0xf4d50d87, 14)
    MD5_قدم(MD5_G, b, c, d, a, MD5_GET(8), 0x455a14ed, 20)
    MD5_قدم(MD5_G, a, b, c, d, MD5_GET(13), 0xa9e3e905, 5)
    MD5_قدم(MD5_G, d, a, b, c, MD5_GET(2), 0xfcefa3f8, 9)
    MD5_قدم(MD5_G, c, d, a, b, MD5_GET(7), 0x676f02d9, 14)
    MD5_قدم(MD5_G, b, c, d, a, MD5_GET(12), 0x8d2a4c8a, 20)

    /* Round 3 */
    MD5_قدم(MD5_H, a, b, c, d, MD5_GET(5), 0xfffa3942, 4)
    MD5_قدم(MD5_H2, d, a, b, c, MD5_GET(8), 0x8771f681, 11)
    MD5_قدم(MD5_H, c, d, a, b, MD5_GET(11), 0x6d9d6122, 16)
    MD5_قدم(MD5_H2, b, c, d, a, MD5_GET(14), 0xfde5380c, 23)
    MD5_قدم(MD5_H, a, b, c, d, MD5_GET(1), 0xa4beea44, 4)
    MD5_قدم(MD5_H2, d, a, b, c, MD5_GET(4), 0x4bdecfa9, 11)
    MD5_قدم(MD5_H, c, d, a, b, MD5_GET(7), 0xf6bb4b60, 16)
    MD5_قدم(MD5_H2, b, c, d, a, MD5_GET(10), 0xbebfbc70, 23)
    MD5_قدم(MD5_H, a, b, c, d, MD5_GET(13), 0x289b7ec6, 4)
    MD5_قدم(MD5_H2, d, a, b, c, MD5_GET(0), 0xeaa127fa, 11)
    MD5_قدم(MD5_H, c, d, a, b, MD5_GET(3), 0xd4ef3085, 16)
    MD5_قدم(MD5_H2, b, c, d, a, MD5_GET(6), 0x04881d05, 23)
    MD5_قدم(MD5_H, a, b, c, d, MD5_GET(9), 0xd9d4d039, 4)
    MD5_قدم(MD5_H2, d, a, b, c, MD5_GET(12), 0xe6db99e5, 11)
    MD5_قدم(MD5_H, c, d, a, b, MD5_GET(15), 0x1fa27cf8, 16)
    MD5_قدم(MD5_H2, b, c, d, a, MD5_GET(2), 0xc4ac5665, 23)

    /* Round 4 */
    MD5_قدم(MD5_I, a, b, c, d, MD5_GET(0), 0xf4292244, 6)
    MD5_قدم(MD5_I, d, a, b, c, MD5_GET(7), 0x432aff97, 10)
    MD5_قدم(MD5_I, c, d, a, b, MD5_GET(14), 0xab9423a7, 15)
    MD5_قدم(MD5_I, b, c, d, a, MD5_GET(5), 0xfc93a039, 21)
    MD5_قدم(MD5_I, a, b, c, d, MD5_GET(12), 0x655b59c3, 6)
    MD5_قدم(MD5_I, d, a, b, c, MD5_GET(3), 0x8f0ccc92, 10)
    MD5_قدم(MD5_I, c, d, a, b, MD5_GET(10), 0xffeff47d, 15)
    MD5_قدم(MD5_I, b, c, d, a, MD5_GET(1), 0x85845dd1, 21)
    MD5_قدم(MD5_I, a, b, c, d, MD5_GET(8), 0x6fa87e4f, 6)
    MD5_قدم(MD5_I, d, a, b, c, MD5_GET(15), 0xfe2ce6e0, 10)
    MD5_قدم(MD5_I, c, d, a, b, MD5_GET(6), 0xa3014314, 15)
    MD5_قدم(MD5_I, b, c, d, a, MD5_GET(13), 0x4e0811a1, 21)
    MD5_قدم(MD5_I, a, b, c, d, MD5_GET(4), 0xf7537e82, 6)
    MD5_قدم(MD5_I, d, a, b, c, MD5_GET(11), 0xbd3af235, 10)
    MD5_قدم(MD5_I, c, d, a, b, MD5_GET(2), 0x2ad7d2bb, 15)
    MD5_قدم(MD5_I, b, c, d, a, MD5_GET(9), 0xeb86d391, 21)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } جب(سائز -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  واپس ptr;
}

ساکن CURLcode my_md5_init(خالی *اندر)
{
  my_md5_ctx *ctx = (my_md5_ctx *)اندر;
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  واپس CURLE_OK;
}

ساکن خالی my_md5_update(خالی *اندر,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  uint32_t saved_lo;
  غیرعلامتی صحیح مستعمل;
  my_md5_ctx *ctx = (my_md5_ctx *)اندر;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  اگر(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  مستعمل = saved_lo & 0x3f;

  اگر(مستعمل) {
    غیرعلامتی صحیح دستیاب = 64 - مستعمل;

    اگر(len < دستیاب) {
      حافظہ_نقل(&ctx->بفر[مستعمل], داخلہ, len);
      واپس;
    }

    حافظہ_نقل(&ctx->بفر[مستعمل], داخلہ, دستیاب);
    داخلہ = (ثابت غیرعلامتی حرف *)داخلہ + دستیاب;
    len -= دستیاب;
    my_md5_body(ctx, ctx->بفر, 64);
  }

  اگر(len >= 64) {
    داخلہ = my_md5_body(ctx, داخلہ, len & ~(غیرعلامتی طویل)0x3f);
    len &= 0x3f;
  }

  حافظہ_نقل(ctx->بفر, داخلہ, len);
}

ساکن خالی my_md5_final(غیرعلامتی حرف *digest, خالی *اندر)
{
  غیرعلامتی صحیح مستعمل, دستیاب;
  my_md5_ctx *ctx = (my_md5_ctx *)اندر;

  مستعمل = ctx->lo & 0x3f;

  ctx->بفر[مستعمل++] = 0x80;

  دستیاب = 64 - مستعمل;

  اگر(دستیاب < 8) {
    memset(&ctx->بفر[مستعمل], 0, دستیاب);
    my_md5_body(ctx, ctx->بفر, 64);
    مستعمل = 0;
    دستیاب = 64;
  }

  memset(&ctx->بفر[مستعمل], 0, دستیاب - 8);

  ctx->lo <<= 3;
  ctx->بفر[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->بفر[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->بفر[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->بفر[59] = curlx_ultouc(ctx->lo >> 24);
  ctx->بفر[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->بفر[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->بفر[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->بفر[63] = curlx_ultouc(ctx->hi >> 24);

  my_md5_body(ctx, ctx->بفر, 64);

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

  memset(ctx, 0, حجم(*ctx));
}

#اگر_ختم /* CRYPTO LIBS */

ثابت ساخت HMAC_params Curl_HMAC_MD5 = {
  my_md5_init,        /* Hash initialization function. */
  my_md5_update,      /* Hash update function. */
  my_md5_final,       /* Hash computation end function. */
  حجم(my_md5_ctx), /* Size of hash context structure. */
  64,                 /* Maximum key length. */
  MD5_DIGEST_LEN      /* Result size. */
};

ثابت ساخت MD5_params Curl_DIGEST_MD5 = {
  my_md5_init,        /* Digest initialization function */
  my_md5_update,      /* Digest update function */
  my_md5_final,       /* Digest computation end function */
  حجم(my_md5_ctx), /* Size of digest context struct */
  MD5_DIGEST_LEN      /* Result size */
};

/*
 * @unittest: 1601
 * Returns CURLE_OK on success.
 */
CURLcode Curl_md5it(غیرعلامتی حرف *output,
                    ثابت غیرعلامتی حرف *داخلہ, size_t len)
{
  CURLcode نتیجہ;
  my_md5_ctx ctx;

  نتیجہ = my_md5_init(&ctx);
  اگر(!نتیجہ) {
    کرو {
      غیرعلامتی صحیح ilen = (غیرعلامتی صحیح)CURLMIN(len, UINT_MAX);
      my_md5_update(&ctx, داخلہ, ilen);
      داخلہ += ilen;
      len -= ilen;
    } جب(len);
    my_md5_final(output, &ctx);
  }
  واپس نتیجہ;
}

ساخت MD5_context *Curl_MD5_init(ثابت ساخت MD5_params *md5params)
{
  ساخت MD5_context *ctxt;

  /* Create MD5 context */
  ctxt = curlx_malloc(حجم(*ctxt));

  اگر(!ctxt)
    واپس ctxt;

  ctxt->md5_hashctx = curlx_malloc(md5params->md5_ctxtsize);

  اگر(!ctxt->md5_hashctx) {
    curlx_free(ctxt);
    واپس تہی;
  }

  ctxt->md5_hash = md5params;

  اگر((*md5params->md5_init_func)(ctxt->md5_hashctx)) {
    curlx_free(ctxt->md5_hashctx);
    curlx_free(ctxt);
    واپس تہی;
  }

  واپس ctxt;
}

CURLcode Curl_MD5_update(ساخت MD5_context *context,
                         ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  (*context->md5_hash->md5_update_func)(context->md5_hashctx, داخلہ, len);

  واپس CURLE_OK;
}

CURLcode Curl_MD5_final(ساخت MD5_context *context, غیرعلامتی حرف *نتیجہ)
{
  (*context->md5_hash->md5_final_func)(نتیجہ, context->md5_hashctx);

  curlx_free(context->md5_hashctx);
  curlx_free(context);

  واپس CURLE_OK;
}

#اگر_ختم /* Using NTLM (without SSPI) || Digest */
