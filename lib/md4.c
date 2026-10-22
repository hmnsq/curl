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
#ضمن "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

#ضمن "curl_md4.h"

#ifdef USE_OPENSSL
#ضمن <openssl/opensslv.h>
#אם OPENSSL_VERSION_NUMBER >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#define OPENSSL_NO_MD4
#אחרת
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#ضمن <openssl/opensslconf.h>
#endif
#endif /* USE_OPENSSL */

#ifdef USE_WOLFSSL
#ضمن <wolfssl/options.h>
#endif

#אם defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#ضمن <openssl/evp.h>

تعريفنوع EVP_MD_CTX *my_md4_ctx;

ساكن صحيح my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  אם(!*ctx)
    החזר 0;

  אם(!EVP_DigestInit_ex(*ctx, EVP_md4(), عدم)) {
    EVP_MD_CTX_free(*ctx);
    החזר 0;
  }
  החזר 1;
}

ساكن فراغ my_md4_update(my_md4_ctx *ctx,
                          ثابت unsigned حرف *input, unsigned صحيح len)
{
  (فراغ)EVP_DigestUpdate(*ctx, input, len);
}

ساكن فراغ my_md4_final(unsigned حرف *digest, my_md4_ctx *ctx)
{
  (فراغ)EVP_DigestFinal_ex(*ctx, digest, عدم);
  EVP_MD_CTX_free(*ctx);
}

#وإلاإذا defined(USE_WOLFSSL) && !defined(NO_MD4)
#ضمن <wolfssl/wolfcrypt/md4.h>
#ضمن <wolfssl/version.h>

#אם LIBWOLFSSL_VERSION_HEX >= 0x05007006
تعريفنوع wc_Md4 my_md4_ctx;
#אחרת
تعريفنوع Md4 my_md4_ctx;
#endif

ساكن صحيح my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  החזר 1;
}

ساكن فراغ my_md4_update(my_md4_ctx *ctx,
                          ثابت فراغ *input, unsigned long len)
{
  wc_Md4Update(ctx, input, (word32)len);
}

ساكن فراغ my_md4_final(unsigned حرف *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#وإلاإذا (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#ضمن <CommonCrypto/CommonDigest.h>

تعريفنوع CC_MD4_CTX my_md4_ctx;

ساكن صحيح my_md4_init(my_md4_ctx *ctx)
{
  החזר CC_MD4_Init(ctx);  /* always returns 1 */
}

ساكن فراغ my_md4_update(my_md4_ctx *ctx,
                          ثابت فراغ *input, unsigned long len)
{
  (فراغ)CC_MD4_Update(ctx, input, (CC_LONG)len);
}

ساكن فراغ my_md4_final(unsigned حرف *digest, my_md4_ctx *ctx)
{
  (فراغ)CC_MD4_Final(digest, ctx);
}

#وإلاإذا defined(USE_WIN32_CRYPTO)
#ضمن <wincrypt.h>

بنية md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
تعريفنوع بنية md4_ctx my_md4_ctx;

ساكن صحيح my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  אם(!CryptAcquireContext(&ctx->hCryptProv, عدم, عدم, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    החזר 0;

  אם(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    החזר 0;
  }

  החזר 1;
}

ساكن فراغ my_md4_update(my_md4_ctx *ctx,
                          ثابت فراغ *input, unsigned long len)
{
  CryptHashData(ctx->hHash, (ثابت BYTE *)input, (unsigned صحيح)len, 0);
}

ساكن فراغ my_md4_final(unsigned حرف *digest, my_md4_ctx *ctx)
{
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, عدم, &length, 0);
  אם(length == MD4_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  אם(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  אם(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#وإلاإذا defined(USE_GNUTLS)
#ضمن <nettle/md4.h>
#ضمن <nettle/version.h>

تعريفنوع بنية md4_ctx my_md4_ctx;

ساكن صحيح my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  החזר 1;
}

ساكن فراغ my_md4_update(my_md4_ctx *ctx,
                          ثابت فراغ *input, unsigned long len)
{
  md4_update(ctx, len, input);
}

ساكن فراغ my_md4_final(unsigned حرف *digest, my_md4_ctx *ctx)
{
#אם NETTLE_VERSION_MAJOR >= 4
  md4_digest(ctx, digest);
#אחרת
  md4_digest(ctx, MD4_DIGEST_SIZE, digest);
#endif
}

#אחרת
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

بنية md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  unsigned حرف buffer[64];
  uint32_t block[16];
};
تعريفنوع بنية md4_ctx my_md4_ctx;

/*
 * The basic MD4 functions.
 *
 * F and G are optimized compared to their RFC 1320 definitions, with the
 * optimization for F borrowed from Colin Plumb's MD5 implementation.
 */
#define MD4_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#define MD4_G(x, y, z)                  (((x) & ((y) | (z))) | ((y) & (z)))
#define MD4_H(x, y, z)                  ((x) ^ (y) ^ (z))

/*
 * The MD4 transformation for all three rounds.
 */
#define MD4_STEP(f, a, b, c, d, x, s) \
  (a) += f(b, c, d) + (x); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is an optimization. Nothing will break if it does not work.
 */
#אם defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define MD4_SET(n) (*(ثابت uint32_t *)(ثابت فراغ *)&ptr[(n) * 4])
#define MD4_GET(n) MD4_SET(n)
#אחרת
#define MD4_SET(n) (ctx->block[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#define MD4_GET(n) ctx->block[n]
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
ساكن ثابت فراغ *my_md4_body(my_md4_ctx *ctx,
                               ثابت فراغ *input, unsigned long size)
{
  ثابت unsigned حرف *ptr;
  uint32_t a, b, c, d;

  ptr = (ثابت unsigned حرف *)input;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  עשה {
    uint32_t saved_a, saved_b, saved_c, saved_d;

    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(0), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(1), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(2), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(3), 19)
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(4), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(5), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(6), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(7), 19)
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(8), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(9), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(10), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(11), 19)
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(12), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(13), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(14), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(15), 19)

    /* Round 2 */
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(0) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(4) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(8) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(12) + 0x5a827999, 13)
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(1) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(5) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(9) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(13) + 0x5a827999, 13)
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(2) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(6) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(10) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(14) + 0x5a827999, 13)
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(3) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(7) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(11) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(15) + 0x5a827999, 13)

    /* Round 3 */
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(0) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(8) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(4) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(12) + 0x6ed9eba1, 15)
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(2) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(10) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(6) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(14) + 0x6ed9eba1, 15)
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(1) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(9) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(5) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(13) + 0x6ed9eba1, 15)
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(3) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(11) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(7) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(15) + 0x6ed9eba1, 15)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } כלעוד(size -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  החזר ptr;
}

ساكن صحيح my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  החזר 1;
}

ساكن فراغ my_md4_update(my_md4_ctx *ctx,
                          ثابت فراغ *input, unsigned long len)
{
  uint32_t saved_lo;
  unsigned long used;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  אם(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  אם(used) {
    unsigned long available = 64 - used;

    אם(len < available) {
      memcpy(&ctx->buffer[used], input, len);
      החזר;
    }

    memcpy(&ctx->buffer[used], input, available);
    input = (ثابت unsigned حرف *)input + available;
    len -= available;
    my_md4_body(ctx, ctx->buffer, 64);
  }

  אם(len >= 64) {
    input = my_md4_body(ctx, input, len & ~(unsigned long)0x3f);
    len &= 0x3f;
  }

  memcpy(ctx->buffer, input, len);
}

ساكن فراغ my_md4_final(unsigned حرف *digest, my_md4_ctx *ctx)
{
  unsigned long used, available;

  used = ctx->lo & 0x3f;

  ctx->buffer[used++] = 0x80;

  available = 64 - used;

  אם(available < 8) {
    memset(&ctx->buffer[used], 0, available);
    my_md4_body(ctx, ctx->buffer, 64);
    used = 0;
    available = 64;
  }

  memset(&ctx->buffer[used], 0, available - 8);

  ctx->lo <<= 3;
  ctx->buffer[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->buffer[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->buffer[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->buffer[59] = curlx_ultouc((ctx->lo >> 24) & 0xff);
  ctx->buffer[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->buffer[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->buffer[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->buffer[63] = curlx_ultouc(ctx->hi >> 24);

  my_md4_body(ctx, ctx->buffer, 64);

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

#endif /* CRYPTO LIBS */

CURLcode Curl_md4it(unsigned حرف *output,
                    ثابت unsigned حرف *input, ثابت size_t len)
{
  my_md4_ctx ctx;
  אם(!my_md4_init(&ctx))
    החזר CURLE_FAILED_INIT;
  my_md4_update(&ctx, input, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  החזר CURLE_OK;
}

#endif /* USE_CURL_NTLM_CORE */
