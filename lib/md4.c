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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

#ଅନ୍ତର୍ଭାଵ "curl_md4.h"

#ifdef USE_OPENSSL
#ଅନ୍ତର୍ଭାଵ <openssl/opensslv.h>
#ଯଦି OPENSSL_VERSION_ସଂଖ୍ୟା >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#define OPENSSL_NO_MD4
#ଅନ୍ୟଥା
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#ଅନ୍ତର୍ଭାଵ <openssl/opensslconf.h>
#endif
#endif /* USE_OPENSSL */

#ifdef USE_WOLFSSL
#ଅନ୍ତର୍ଭାଵ <wolfssl/options.h>
#endif

#ଯଦି defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#ଅନ୍ତର୍ଭାଵ <openssl/evp.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା EVP_MD_CTX *my_md4_ctx;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  ଯଦି(!*ctx)
    ଫେରନ୍ତୁ 0;

  ଯଦି(!EVP_DigestInit_ex(*ctx, EVP_md4(), ଖାଲି)) {
    EVP_MD_CTX_free(*ctx);
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_update(my_md4_ctx *ctx,
                          ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
  (ଶୂନ୍ୟ)EVP_DigestUpdate(*ctx, ନିବେଶ, len);
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_final(unsigned ଅକ୍ଷର *digest, my_md4_ctx *ctx)
{
  (ଶୂନ୍ୟ)EVP_DigestFinal_ex(*ctx, digest, ଖାଲି);
  EVP_MD_CTX_free(*ctx);
}

#ନଚେତଯଦି defined(USE_WOLFSSL) && !defined(NO_MD4)
#ଅନ୍ତର୍ଭାଵ <wolfssl/wolfcrypt/md4.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/version.h>

#ଯଦି LIBWOLFSSL_VERSION_HEX >= 0x05007006
ପ୍ରକାରବ୍ୟାଖ୍ୟା wc_Md4 my_md4_ctx;
#ଅନ୍ୟଥା
ପ୍ରକାରବ୍ୟାଖ୍ୟା Md4 my_md4_ctx;
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_update(my_md4_ctx *ctx,
                          ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ, unsigned long len)
{
  wc_Md4Update(ctx, ନିବେଶ, (word32)len);
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_final(unsigned ଅକ୍ଷର *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#ନଚେତଯଦି (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#ଅନ୍ତର୍ଭାଵ <CommonCrypto/CommonDigest.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା CC_MD4_CTX my_md4_ctx;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା my_md4_init(my_md4_ctx *ctx)
{
  ଫେରନ୍ତୁ CC_MD4_Init(ctx);  /* always returns 1 */
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_update(my_md4_ctx *ctx,
                          ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ, unsigned long len)
{
  (ଶୂନ୍ୟ)CC_MD4_Update(ctx, ନିବେଶ, (CC_LONG)len);
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_final(unsigned ଅକ୍ଷର *digest, my_md4_ctx *ctx)
{
  (ଶୂନ୍ୟ)CC_MD4_Final(digest, ctx);
}

#ନଚେତଯଦି defined(USE_WIN32_CRYPTO)
#ଅନ୍ତର୍ଭାଵ <wincrypt.h>

ସଂରଚନା md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା md4_ctx my_md4_ctx;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  ଯଦି(!CryptAcquireContext(&ctx->hCryptProv, ଖାଲି, ଖାଲି, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ଫେରନ୍ତୁ 0;

  ଯଦି(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_update(my_md4_ctx *ctx,
                          ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ, unsigned long len)
{
  CryptHashData(ctx->hHash, (ଅଚଳ BYTE *)ନିବେଶ, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, 0);
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_final(unsigned ଅକ୍ଷର *digest, my_md4_ctx *ctx)
{
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ଖାଲି, &length, 0);
  ଯଦି(length == MD4_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  ଯଦି(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ଯଦି(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ନଚେତଯଦି defined(USE_GNUTLS)
#ଅନ୍ତର୍ଭାଵ <nettle/md4.h>
#ଅନ୍ତର୍ଭାଵ <nettle/version.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା md4_ctx my_md4_ctx;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_update(my_md4_ctx *ctx,
                          ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ, unsigned long len)
{
  md4_update(ctx, len, ନିବେଶ);
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_final(unsigned ଅକ୍ଷର *digest, my_md4_ctx *ctx)
{
#ଯଦି NETTLE_VERSION_MAJOR >= 4
  md4_digest(ctx, digest);
#ଅନ୍ୟଥା
  md4_digest(ctx, MD4_DIGEST_SIZE, digest);
#endif
}

#ଅନ୍ୟଥା
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

ସଂରଚନା md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  unsigned ଅକ୍ଷର buffer[64];
  uint32_t block[16];
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା md4_ctx my_md4_ctx;

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
#ଯଦି defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define MD4_SET(n) (*(ଅଚଳ uint32_t *)(ଅଚଳ ଶୂନ୍ୟ *)&ptr[(n) * 4])
#define MD4_GET(n) MD4_SET(n)
#ଅନ୍ୟଥା
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
ସ୍ଥିର ଅଚଳ ଶୂନ୍ୟ *my_md4_body(my_md4_ctx *ctx,
                               ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ, unsigned long size)
{
  ଅଚଳ unsigned ଅକ୍ଷର *ptr;
  uint32_t a, b, c, d;

  ptr = (ଅଚଳ unsigned ଅକ୍ଷର *)ନିବେଶ;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  କର {
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
  } ଯେତେବେଳେ(size -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  ଫେରନ୍ତୁ ptr;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_update(my_md4_ctx *ctx,
                          ଅଚଳ ଶୂନ୍ୟ *ନିବେଶ, unsigned long len)
{
  uint32_t saved_lo;
  unsigned long used;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ଯଦି(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  ଯଦି(used) {
    unsigned long available = 64 - used;

    ଯଦି(len < available) {
      memcpy(&ctx->buffer[used], ନିବେଶ, len);
      ଫେରନ୍ତୁ;
    }

    memcpy(&ctx->buffer[used], ନିବେଶ, available);
    ନିବେଶ = (ଅଚଳ unsigned ଅକ୍ଷର *)ନିବେଶ + available;
    len -= available;
    my_md4_body(ctx, ctx->buffer, 64);
  }

  ଯଦି(len >= 64) {
    ନିବେଶ = my_md4_body(ctx, ନିବେଶ, len & ~(unsigned long)0x3f);
    len &= 0x3f;
  }

  memcpy(ctx->buffer, ନିବେଶ, len);
}

ସ୍ଥିର ଶୂନ୍ୟ my_md4_final(unsigned ଅକ୍ଷର *digest, my_md4_ctx *ctx)
{
  unsigned long used, available;

  used = ctx->lo & 0x3f;

  ctx->buffer[used++] = 0x80;

  available = 64 - used;

  ଯଦି(available < 8) {
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

  memset(ctx, 0, ଆକାର(*ctx));
}

#endif /* CRYPTO LIBS */

CURLcode Curl_md4it(unsigned ଅକ୍ଷର *output,
                    ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ, ଅଚଳ size_t len)
{
  my_md4_ctx ctx;
  ଯଦି(!my_md4_init(&ctx))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  my_md4_update(&ctx, ନିବେଶ, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  ଫେରନ୍ତୁ CURLE_OK;
}

#endif /* USE_CURL_NTLM_CORE */
