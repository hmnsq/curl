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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

#অন্তর্ভাব "curl_md4.h"

#ifdef USE_OPENSSL
#অন্তর্ভাব <openssl/opensslv.h>
#যদি OPENSSL_VERSION_সংখ্যা >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#define OPENSSL_NO_MD4
#অন্যথা
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#অন্তর্ভাব <openssl/opensslconf.h>
#endif
#endif /* USE_OPENSSL */

#ifdef USE_WOLFSSL
#অন্তর্ভাব <wolfssl/options.h>
#endif

#যদি defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#অন্তর্ভাব <openssl/evp.h>

প্রকারলেখ EVP_MD_CTX *my_md4_ctx;

স্থির পূর্ণসংখ্যা my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  যদি(!*ctx)
    ফেরত 0;

  যদি(!EVP_DigestInit_ex(*ctx, EVP_md4(), ফাঁকা)) {
    EVP_MD_CTX_free(*ctx);
    ফেরত 0;
  }
  ফেরত 1;
}

স্থির শূন্য my_md4_update(my_md4_ctx *ctx,
                          ধ্রুবক unsigned অক্ষর *প্রবেশ, unsigned পূর্ণসংখ্যা len)
{
  (শূন্য)EVP_DigestUpdate(*ctx, প্রবেশ, len);
}

স্থির শূন্য my_md4_final(unsigned অক্ষর *digest, my_md4_ctx *ctx)
{
  (শূন্য)EVP_DigestFinal_ex(*ctx, digest, ফাঁকা);
  EVP_MD_CTX_free(*ctx);
}

#নতুবা_যদি defined(USE_WOLFSSL) && !defined(NO_MD4)
#অন্তর্ভাব <wolfssl/wolfcrypt/md4.h>
#অন্তর্ভাব <wolfssl/version.h>

#যদি LIBWOLFSSL_VERSION_HEX >= 0x05007006
প্রকারলেখ wc_Md4 my_md4_ctx;
#অন্যথা
প্রকারলেখ Md4 my_md4_ctx;
#endif

স্থির পূর্ণসংখ্যা my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  ফেরত 1;
}

স্থির শূন্য my_md4_update(my_md4_ctx *ctx,
                          ধ্রুবক শূন্য *প্রবেশ, unsigned long len)
{
  wc_Md4Update(ctx, প্রবেশ, (word32)len);
}

স্থির শূন্য my_md4_final(unsigned অক্ষর *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#নতুবা_যদি (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#অন্তর্ভাব <CommonCrypto/CommonDigest.h>

প্রকারলেখ CC_MD4_CTX my_md4_ctx;

স্থির পূর্ণসংখ্যা my_md4_init(my_md4_ctx *ctx)
{
  ফেরত CC_MD4_Init(ctx);  /* always returns 1 */
}

স্থির শূন্য my_md4_update(my_md4_ctx *ctx,
                          ধ্রুবক শূন্য *প্রবেশ, unsigned long len)
{
  (শূন্য)CC_MD4_Update(ctx, প্রবেশ, (CC_LONG)len);
}

স্থির শূন্য my_md4_final(unsigned অক্ষর *digest, my_md4_ctx *ctx)
{
  (শূন্য)CC_MD4_Final(digest, ctx);
}

#নতুবা_যদি defined(USE_WIN32_CRYPTO)
#অন্তর্ভাব <wincrypt.h>

কাঠামো md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
প্রকারলেখ কাঠামো md4_ctx my_md4_ctx;

স্থির পূর্ণসংখ্যা my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  যদি(!CryptAcquireContext(&ctx->hCryptProv, ফাঁকা, ফাঁকা, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ফেরত 0;

  যদি(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ফেরত 0;
  }

  ফেরত 1;
}

স্থির শূন্য my_md4_update(my_md4_ctx *ctx,
                          ধ্রুবক শূন্য *প্রবেশ, unsigned long len)
{
  Crypt_Hash_তথ্য(ctx->hHash, (ধ্রুবক BYTE *)প্রবেশ, (unsigned পূর্ণসংখ্যা)len, 0);
}

স্থির শূন্য my_md4_final(unsigned অক্ষর *digest, my_md4_ctx *ctx)
{
  unsigned long দৈর্ঘ্য = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ফাঁকা, &দৈর্ঘ্য, 0);
  যদি(দৈর্ঘ্য == MD4_DIGEST_দৈর্ঘ্য)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &দৈর্ঘ্য, 0);

  যদি(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  যদি(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#নতুবা_যদি defined(USE_GNUTLS)
#অন্তর্ভাব <nettle/md4.h>
#অন্তর্ভাব <nettle/version.h>

প্রকারলেখ কাঠামো md4_ctx my_md4_ctx;

স্থির পূর্ণসংখ্যা my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  ফেরত 1;
}

স্থির শূন্য my_md4_update(my_md4_ctx *ctx,
                          ধ্রুবক শূন্য *প্রবেশ, unsigned long len)
{
  md4_update(ctx, len, প্রবেশ);
}

স্থির শূন্য my_md4_final(unsigned অক্ষর *digest, my_md4_ctx *ctx)
{
#যদি NETTLE_VERSION_MAJOR >= 4
  md4_digest(ctx, digest);
#অন্যথা
  md4_digest(ctx, MD4_DIGEST_আকার, digest);
#endif
}

#অন্যথা
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

কাঠামো md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  unsigned অক্ষর বাফার[64];
  uint32_t block[16];
};
প্রকারলেখ কাঠামো md4_ctx my_md4_ctx;

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
#যদি defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define MD4_SET(n) (*(ধ্রুবক uint32_t *)(ধ্রুবক শূন্য *)&ptr[(n) * 4])
#define MD4_GET(n) MD4_SET(n)
#অন্যথা
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
স্থির ধ্রুবক শূন্য *my_md4_body(my_md4_ctx *ctx,
                               ধ্রুবক শূন্য *প্রবেশ, unsigned long আকার_2)
{
  ধ্রুবক unsigned অক্ষর *ptr;
  uint32_t a, b, c, d;

  ptr = (ধ্রুবক unsigned অক্ষর *)প্রবেশ;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  করো {
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
  } যখন(আকার_2 -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  ফেরত ptr;
}

স্থির পূর্ণসংখ্যা my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  ফেরত 1;
}

স্থির শূন্য my_md4_update(my_md4_ctx *ctx,
                          ধ্রুবক শূন্য *প্রবেশ, unsigned long len)
{
  uint32_t saved_lo;
  unsigned long used;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  যদি(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  যদি(used) {
    unsigned long available = 64 - used;

    যদি(len < available) {
      memcpy(&ctx->বাফার[used], প্রবেশ, len);
      ফেরত;
    }

    memcpy(&ctx->বাফার[used], প্রবেশ, available);
    প্রবেশ = (ধ্রুবক unsigned অক্ষর *)প্রবেশ + available;
    len -= available;
    my_md4_body(ctx, ctx->বাফার, 64);
  }

  যদি(len >= 64) {
    প্রবেশ = my_md4_body(ctx, প্রবেশ, len & ~(unsigned long)0x3f);
    len &= 0x3f;
  }

  memcpy(ctx->বাফার, প্রবেশ, len);
}

স্থির শূন্য my_md4_final(unsigned অক্ষর *digest, my_md4_ctx *ctx)
{
  unsigned long used, available;

  used = ctx->lo & 0x3f;

  ctx->বাফার[used++] = 0x80;

  available = 64 - used;

  যদি(available < 8) {
    memset(&ctx->বাফার[used], 0, available);
    my_md4_body(ctx, ctx->বাফার, 64);
    used = 0;
    available = 64;
  }

  memset(&ctx->বাফার[used], 0, available - 8);

  ctx->lo <<= 3;
  ctx->বাফার[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->বাফার[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->বাফার[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->বাফার[59] = curlx_ultouc((ctx->lo >> 24) & 0xff);
  ctx->বাফার[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->বাফার[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->বাফার[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->বাফার[63] = curlx_ultouc(ctx->hi >> 24);

  my_md4_body(ctx, ctx->বাফার, 64);

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

  memset(ctx, 0, আকার(*ctx));
}

#endif /* CRYPTO LIBS */

CURLcode Curl_md4it(unsigned অক্ষর *output,
                    ধ্রুবক unsigned অক্ষর *প্রবেশ, ধ্রুবক size_t len)
{
  my_md4_ctx ctx;
  যদি(!my_md4_init(&ctx))
    ফেরত CURLE_FAILED_INIT;
  my_md4_update(&ctx, প্রবেশ, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  ফেরত CURLE_OK;
}

#endif /* USE_CURL_NTLM_CORE */
