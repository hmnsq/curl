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
#અન્તર્ભાવ "curl_setup.h"

#જો (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#અન્તર્ભાવ "curl_md5.h"
#અન્તર્ભાવ "curl_hmac.h"

#ifdef USE_OPENSSL
#અન્તર્ભાવ <openssl/opensslconf.h>
#endif

#ifdef USE_WOLFSSL
#અન્તર્ભાવ <wolfssl/options.h>
#endif

#ifdef USE_MBEDTLS
#અન્તર્ભાવ <mbedtls/version.h>
#જો MBEDTLS_VERSION_સંખ્યા < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#અન્તર્ભાવ <psa/crypto_config.h>
#endif

#ifdef USE_GNUTLS
#અન્તર્ભાવ <nettle/md5.h>
#અન્તર્ભાવ <nettle/version.h>

પ્રકારવ્યાખ્યા સંરચના md5_ctx my_md5_ctx;

સ્થિર CURLcode my_md5_init(શૂન્ય *ctx)
{
  md5_init(ctx);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_md5_update(શૂન્ય *ctx,
                          અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  md5_update(ctx, len, આગત);
}

સ્થિર શૂન્ય my_md5_final(unsigned અક્ષર *digest, શૂન્ય *ctx)
{
#જો NETTLE_VERSION_MAJOR >= 4
  md5_digest(ctx, digest);
#નહીંતર
  md5_digest(ctx, MD5_DIGEST_LEN, digest);
#endif
}

#અથવાજો defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD5)
#અન્તર્ભાવ <openssl/evp.h>

પ્રકારવ્યાખ્યા EVP_MD_CTX *my_md5_ctx;

સ્થિર CURLcode my_md5_init(શૂન્ય *in)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)in;
  *ctx = EVP_MD_CTX_new();
  જો(!*ctx)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(!EVP_DigestInit_ex(*ctx, EVP_md5(), ખાલી)) {
    EVP_MD_CTX_free(*ctx);
    *ctx = ખાલી;
    પાછા CURLE_FAILED_INIT;
  }
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_md5_update(શૂન્ય *in,
                          અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)in;
  (શૂન્ય)EVP_DigestUpdate(*ctx, આગત, len);
}

સ્થિર શૂન્ય my_md5_final(unsigned અક્ષર *digest, શૂન્ય *in)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)in;
  (શૂન્ય)EVP_DigestFinal_ex(*ctx, digest, ખાલી);
  EVP_MD_CTX_free(*ctx);
  *ctx = ખાલી;
}

#અથવાજો defined(USE_WOLFSSL) && !defined(NO_MD5)
#અન્તર્ભાવ <wolfssl/wolfcrypt/md5.h>

પ્રકારવ્યાખ્યા wc_Md5 my_md5_ctx;

સ્થિર CURLcode my_md5_init(શૂન્ય *ctx)
{
  જો(wc_InitMd5(ctx))
    પાછા CURLE_OUT_OF_MEMORY;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_md5_update(શૂન્ય *ctx,
                          અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  (શૂન્ય)wc_Md5Update(ctx, આગત, (word32)len);
}

સ્થિર શૂન્ય my_md5_final(unsigned અક્ષર *digest, શૂન્ય *ctx)
{
  (શૂન્ય)wc_Md5Final(ctx, digest);
}

#અથવાજો defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_MD5) && PSA_WANT_ALG_MD5
#અન્તર્ભાવ <psa/crypto.h>

પ્રકારવ્યાખ્યા psa_hash_operation_t my_md5_ctx;

સ્થિર CURLcode my_md5_init(શૂન્ય *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  જો(psa_hash_setup(pctx, PSA_ALG_MD5) != PSA_SUCCESS)
    પાછા CURLE_OUT_OF_MEMORY;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_md5_update(શૂન્ય *ctx,
                          અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  (શૂન્ય)psa_hash_update(ctx, આગત, len);
}

સ્થિર શૂન્ય my_md5_final(unsigned અક્ષર *digest, શૂન્ય *ctx)
{
  size_t actual_length;
  (શૂન્ય)psa_hash_finish(ctx, digest, MD5_DIGEST_LEN, &actual_length);
}

#અથવાજો (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#અન્તર્ભાવ <CommonCrypto/CommonDigest.h>

/* For Apple operating systems: CommonCrypto has the functions we need.
   These functions are available on Tiger and later, as well as iOS 2.0
   and later. If you are building for an older cat, well, sorry.

   Declaring the functions as static like this seems to be a bit more
   reliable than defining COMMON_DIGEST_FOR_OPENSSL on older cats. */
#  define my_md5_ctx CC_MD5_CTX

સ્થિર CURLcode my_md5_init(શૂન્ય *ctx)
{
  (શૂન્ય)CC_MD5_Init(ctx);  /* always returns 1 */
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_md5_update(શૂન્ય *ctx,
                          અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  CC_MD5_Update(ctx, આગત, len);
}

સ્થિર શૂન્ય my_md5_final(unsigned અક્ષર *digest, શૂન્ય *ctx)
{
  CC_MD5_Final(digest, ctx);
}

#અથવાજો defined(USE_WIN32_CRYPTO)
#અન્તર્ભાવ <wincrypt.h>

સંરચના md5_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
પ્રકારવ્યાખ્યા સંરચના md5_ctx my_md5_ctx;

સ્થિર CURLcode my_md5_init(શૂન્ય *in)
{
  my_md5_ctx *ctx = (my_md5_ctx *)in;
  જો(!CryptAcquireContext(&ctx->hCryptProv, ખાલી, ખાલી, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    પાછા CURLE_OUT_OF_MEMORY;

  જો(!CryptCreateHash(ctx->hCryptProv, CALG_MD5, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    પાછા CURLE_FAILED_INIT;
  }

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_md5_update(શૂન્ય *in,
                          અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  my_md5_ctx *ctx = in;
  CryptHashData(ctx->hHash, (અચલ BYTE *)આગત, len, 0);
}

સ્થિર શૂન્ય my_md5_final(unsigned અક્ષર *digest, શૂન્ય *in)
{
  my_md5_ctx *ctx = (my_md5_ctx *)in;
  unsigned long length = 0;
  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ખાલી, &length, 0);
  જો(length == MD5_DIGEST_LEN)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);
  જો(ctx->hHash)
    CryptDestroyHash(ctx->hHash);
  જો(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#નહીંતર

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

સંરચના md5_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  unsigned અક્ષર buffer[64];
  uint32_t block[16];
};
પ્રકારવ્યાખ્યા સંરચના md5_ctx my_md5_ctx;

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, like in Colin Plumb's
 * implementation.
 */
#define MD5_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#define MD5_G(x, y, z)                  ((y) ^ ((z) & ((x) ^ (y))))
#define MD5_H(x, y, z)                  (((x) ^ (y)) ^ (z))
#define MD5_H2(x, y, z)                 ((x) ^ ((y) ^ (z)))
#define MD5_I(x, y, z)                  ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define MD5_STEP(f, a, b, c, d, x, t, s) \
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
#જો defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define MD5_SET(n) (*(અચલ uint32_t *)(અચલ શૂન્ય *)&ptr[(n) * 4])
#define MD5_GET(n) MD5_SET(n)
#નહીંતર
#define MD5_SET(n) (ctx->block[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#define MD5_GET(n) ctx->block[n]
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
સ્થિર અચલ શૂન્ય *my_md5_body(my_md5_ctx *ctx,
                               અચલ શૂન્ય *data, unsigned long size)
{
  અચલ unsigned અક્ષર *ptr;
  uint32_t a, b, c, d;

  ptr = (અચલ unsigned અક્ષર *)data;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  કરો {
    uint32_t saved_a, saved_b, saved_c, saved_d;

    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    MD5_STEP(MD5_F, a, b, c, d, MD5_SET(0), 0xd76aa478, 7)
    MD5_STEP(MD5_F, d, a, b, c, MD5_SET(1), 0xe8c7b756, 12)
    MD5_STEP(MD5_F, c, d, a, b, MD5_SET(2), 0x242070db, 17)
    MD5_STEP(MD5_F, b, c, d, a, MD5_SET(3), 0xc1bdceee, 22)
    MD5_STEP(MD5_F, a, b, c, d, MD5_SET(4), 0xf57c0faf, 7)
    MD5_STEP(MD5_F, d, a, b, c, MD5_SET(5), 0x4787c62a, 12)
    MD5_STEP(MD5_F, c, d, a, b, MD5_SET(6), 0xa8304613, 17)
    MD5_STEP(MD5_F, b, c, d, a, MD5_SET(7), 0xfd469501, 22)
    MD5_STEP(MD5_F, a, b, c, d, MD5_SET(8), 0x698098d8, 7)
    MD5_STEP(MD5_F, d, a, b, c, MD5_SET(9), 0x8b44f7af, 12)
    MD5_STEP(MD5_F, c, d, a, b, MD5_SET(10), 0xffff5bb1, 17)
    MD5_STEP(MD5_F, b, c, d, a, MD5_SET(11), 0x895cd7be, 22)
    MD5_STEP(MD5_F, a, b, c, d, MD5_SET(12), 0x6b901122, 7)
    MD5_STEP(MD5_F, d, a, b, c, MD5_SET(13), 0xfd987193, 12)
    MD5_STEP(MD5_F, c, d, a, b, MD5_SET(14), 0xa679438e, 17)
    MD5_STEP(MD5_F, b, c, d, a, MD5_SET(15), 0x49b40821, 22)

    /* Round 2 */
    MD5_STEP(MD5_G, a, b, c, d, MD5_GET(1), 0xf61e2562, 5)
    MD5_STEP(MD5_G, d, a, b, c, MD5_GET(6), 0xc040b340, 9)
    MD5_STEP(MD5_G, c, d, a, b, MD5_GET(11), 0x265e5a51, 14)
    MD5_STEP(MD5_G, b, c, d, a, MD5_GET(0), 0xe9b6c7aa, 20)
    MD5_STEP(MD5_G, a, b, c, d, MD5_GET(5), 0xd62f105d, 5)
    MD5_STEP(MD5_G, d, a, b, c, MD5_GET(10), 0x02441453, 9)
    MD5_STEP(MD5_G, c, d, a, b, MD5_GET(15), 0xd8a1e681, 14)
    MD5_STEP(MD5_G, b, c, d, a, MD5_GET(4), 0xe7d3fbc8, 20)
    MD5_STEP(MD5_G, a, b, c, d, MD5_GET(9), 0x21e1cde6, 5)
    MD5_STEP(MD5_G, d, a, b, c, MD5_GET(14), 0xc33707d6, 9)
    MD5_STEP(MD5_G, c, d, a, b, MD5_GET(3), 0xf4d50d87, 14)
    MD5_STEP(MD5_G, b, c, d, a, MD5_GET(8), 0x455a14ed, 20)
    MD5_STEP(MD5_G, a, b, c, d, MD5_GET(13), 0xa9e3e905, 5)
    MD5_STEP(MD5_G, d, a, b, c, MD5_GET(2), 0xfcefa3f8, 9)
    MD5_STEP(MD5_G, c, d, a, b, MD5_GET(7), 0x676f02d9, 14)
    MD5_STEP(MD5_G, b, c, d, a, MD5_GET(12), 0x8d2a4c8a, 20)

    /* Round 3 */
    MD5_STEP(MD5_H, a, b, c, d, MD5_GET(5), 0xfffa3942, 4)
    MD5_STEP(MD5_H2, d, a, b, c, MD5_GET(8), 0x8771f681, 11)
    MD5_STEP(MD5_H, c, d, a, b, MD5_GET(11), 0x6d9d6122, 16)
    MD5_STEP(MD5_H2, b, c, d, a, MD5_GET(14), 0xfde5380c, 23)
    MD5_STEP(MD5_H, a, b, c, d, MD5_GET(1), 0xa4beea44, 4)
    MD5_STEP(MD5_H2, d, a, b, c, MD5_GET(4), 0x4bdecfa9, 11)
    MD5_STEP(MD5_H, c, d, a, b, MD5_GET(7), 0xf6bb4b60, 16)
    MD5_STEP(MD5_H2, b, c, d, a, MD5_GET(10), 0xbebfbc70, 23)
    MD5_STEP(MD5_H, a, b, c, d, MD5_GET(13), 0x289b7ec6, 4)
    MD5_STEP(MD5_H2, d, a, b, c, MD5_GET(0), 0xeaa127fa, 11)
    MD5_STEP(MD5_H, c, d, a, b, MD5_GET(3), 0xd4ef3085, 16)
    MD5_STEP(MD5_H2, b, c, d, a, MD5_GET(6), 0x04881d05, 23)
    MD5_STEP(MD5_H, a, b, c, d, MD5_GET(9), 0xd9d4d039, 4)
    MD5_STEP(MD5_H2, d, a, b, c, MD5_GET(12), 0xe6db99e5, 11)
    MD5_STEP(MD5_H, c, d, a, b, MD5_GET(15), 0x1fa27cf8, 16)
    MD5_STEP(MD5_H2, b, c, d, a, MD5_GET(2), 0xc4ac5665, 23)

    /* Round 4 */
    MD5_STEP(MD5_I, a, b, c, d, MD5_GET(0), 0xf4292244, 6)
    MD5_STEP(MD5_I, d, a, b, c, MD5_GET(7), 0x432aff97, 10)
    MD5_STEP(MD5_I, c, d, a, b, MD5_GET(14), 0xab9423a7, 15)
    MD5_STEP(MD5_I, b, c, d, a, MD5_GET(5), 0xfc93a039, 21)
    MD5_STEP(MD5_I, a, b, c, d, MD5_GET(12), 0x655b59c3, 6)
    MD5_STEP(MD5_I, d, a, b, c, MD5_GET(3), 0x8f0ccc92, 10)
    MD5_STEP(MD5_I, c, d, a, b, MD5_GET(10), 0xffeff47d, 15)
    MD5_STEP(MD5_I, b, c, d, a, MD5_GET(1), 0x85845dd1, 21)
    MD5_STEP(MD5_I, a, b, c, d, MD5_GET(8), 0x6fa87e4f, 6)
    MD5_STEP(MD5_I, d, a, b, c, MD5_GET(15), 0xfe2ce6e0, 10)
    MD5_STEP(MD5_I, c, d, a, b, MD5_GET(6), 0xa3014314, 15)
    MD5_STEP(MD5_I, b, c, d, a, MD5_GET(13), 0x4e0811a1, 21)
    MD5_STEP(MD5_I, a, b, c, d, MD5_GET(4), 0xf7537e82, 6)
    MD5_STEP(MD5_I, d, a, b, c, MD5_GET(11), 0xbd3af235, 10)
    MD5_STEP(MD5_I, c, d, a, b, MD5_GET(2), 0x2ad7d2bb, 15)
    MD5_STEP(MD5_I, b, c, d, a, MD5_GET(9), 0xeb86d391, 21)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } જ્યારે(size -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  પાછા ptr;
}

સ્થિર CURLcode my_md5_init(શૂન્ય *in)
{
  my_md5_ctx *ctx = (my_md5_ctx *)in;
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_md5_update(શૂન્ય *in,
                          અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  uint32_t saved_lo;
  unsigned પૂર્ણાંક used;
  my_md5_ctx *ctx = (my_md5_ctx *)in;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  જો(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  જો(used) {
    unsigned પૂર્ણાંક available = 64 - used;

    જો(len < available) {
      memcpy(&ctx->buffer[used], આગત, len);
      પાછા;
    }

    memcpy(&ctx->buffer[used], આગત, available);
    આગત = (અચલ unsigned અક્ષર *)આગત + available;
    len -= available;
    my_md5_body(ctx, ctx->buffer, 64);
  }

  જો(len >= 64) {
    આગત = my_md5_body(ctx, આગત, len & ~(unsigned long)0x3f);
    len &= 0x3f;
  }

  memcpy(ctx->buffer, આગત, len);
}

સ્થિર શૂન્ય my_md5_final(unsigned અક્ષર *digest, શૂન્ય *in)
{
  unsigned પૂર્ણાંક used, available;
  my_md5_ctx *ctx = (my_md5_ctx *)in;

  used = ctx->lo & 0x3f;

  ctx->buffer[used++] = 0x80;

  available = 64 - used;

  જો(available < 8) {
    memset(&ctx->buffer[used], 0, available);
    my_md5_body(ctx, ctx->buffer, 64);
    used = 0;
    available = 64;
  }

  memset(&ctx->buffer[used], 0, available - 8);

  ctx->lo <<= 3;
  ctx->buffer[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->buffer[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->buffer[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->buffer[59] = curlx_ultouc(ctx->lo >> 24);
  ctx->buffer[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->buffer[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->buffer[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->buffer[63] = curlx_ultouc(ctx->hi >> 24);

  my_md5_body(ctx, ctx->buffer, 64);

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

  memset(ctx, 0, કદ(*ctx));
}

#endif /* CRYPTO LIBS */

અચલ સંરચના HMAC_params Curl_HMAC_MD5 = {
  my_md5_init,        /* Hash initialization function. */
  my_md5_update,      /* Hash update function. */
  my_md5_final,       /* Hash computation end function. */
  કદ(my_md5_ctx), /* Size of hash context structure. */
  64,                 /* Maximum key length. */
  MD5_DIGEST_LEN      /* Result size. */
};

અચલ સંરચના MD5_params Curl_DIGEST_MD5 = {
  my_md5_init,        /* Digest initialization function */
  my_md5_update,      /* Digest update function */
  my_md5_final,       /* Digest computation end function */
  કદ(my_md5_ctx), /* Size of digest context struct */
  MD5_DIGEST_LEN      /* Result size */
};

/*
 * @unittest: 1601
 * Returns CURLE_OK on success.
 */
CURLcode Curl_md5it(unsigned અક્ષર *output,
                    અચલ unsigned અક્ષર *આગત, size_t len)
{
  CURLcode result;
  my_md5_ctx ctx;

  result = my_md5_init(&ctx);
  જો(!result) {
    કરો {
      unsigned પૂર્ણાંક ilen = (unsigned પૂર્ણાંક)CURLMIN(len, UINT_MAX);
      my_md5_update(&ctx, આગત, ilen);
      આગત += ilen;
      len -= ilen;
    } જ્યારે(len);
    my_md5_final(output, &ctx);
  }
  પાછા result;
}

સંરચના MD5_context *Curl_MD5_init(અચલ સંરચના MD5_params *md5params)
{
  સંરચના MD5_context *ctxt;

  /* Create MD5 context */
  ctxt = curlx_malloc(કદ(*ctxt));

  જો(!ctxt)
    પાછા ctxt;

  ctxt->md5_hashctx = curlx_malloc(md5params->md5_ctxtsize);

  જો(!ctxt->md5_hashctx) {
    curlx_free(ctxt);
    પાછા ખાલી;
  }

  ctxt->md5_hash = md5params;

  જો((*md5params->md5_init_func)(ctxt->md5_hashctx)) {
    curlx_free(ctxt->md5_hashctx);
    curlx_free(ctxt);
    પાછા ખાલી;
  }

  પાછા ctxt;
}

CURLcode Curl_MD5_update(સંરચના MD5_context *context,
                         અચલ unsigned અક્ષર *આગત, unsigned પૂર્ણાંક len)
{
  (*context->md5_hash->md5_update_func)(context->md5_hashctx, આગત, len);

  પાછા CURLE_OK;
}

CURLcode Curl_MD5_final(સંરચના MD5_context *context, unsigned અક્ષર *result)
{
  (*context->md5_hash->md5_final_func)(result, context->md5_hashctx);

  curlx_free(context->md5_hashctx);
  curlx_free(context);

  પાછા CURLE_OK;
}

#endif /* Using NTLM (without SSPI) || Digest */
