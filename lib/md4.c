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

#ifdef USE_CURL_NTLM_CORE

#ಅನ್ತರ್ಭಾವ "curl_md4.h"

#ifdef USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/opensslv.h>
#ಯದಿ OPENSSL_VERSION_ಸಂಖ್ಯೆ >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#define OPENSSL_NO_MD4
#ಅನ್ಯಥಾ
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#ಅನ್ತರ್ಭಾವ <openssl/opensslconf.h>
#endif
#endif /* USE_OPENSSL */

#ifdef USE_WOLFSSL
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#endif

#ಯದಿ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

ಪ್ರಕಾರನಿರ್ದೇಶ EVP_MD_CTX *my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  ಯದಿ(!*ctx)
    ಪ್ರತ್ಯಾಗಮ 0;

  ಯದಿ(!EVP_DigestInit_ex(*ctx, EVP_md4(), ರಿಕ್ತ)) {
    EVP_MD_CTX_free(*ctx);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು, unsigned ಪೂರ್ಣಾಙ್ಕ len)
{
  (ಶೂನ್ಯ)EVP_DigestUpdate(*ctx, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(unsigned ವರ್ಣ *digest, my_md4_ctx *ctx)
{
  (ಶೂನ್ಯ)EVP_DigestFinal_ex(*ctx, digest, ರಿಕ್ತ);
  EVP_MD_CTX_free(*ctx);
}

#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL) && !defined(NO_MD4)
#ಅನ್ತರ್ಭಾವ <wolfssl/wolfcrypt/md4.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/version.h>

#ಯದಿ LIBWOLFSSL_VERSION_HEX >= 0x05007006
ಪ್ರಕಾರನಿರ್ದೇಶ wc_Md4 my_md4_ctx;
#ಅನ್ಯಥಾ
ಪ್ರಕಾರನಿರ್ದೇಶ Md4 my_md4_ctx;
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು, unsigned long len)
{
  wc_Md4Update(ctx, ಒಳಹರಿವು, (word32)len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(unsigned ವರ್ಣ *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#ನೋಚೇದ್ಯದಿ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#ಅನ್ತರ್ಭಾವ <CommonCrypto/CommonDigest.h>

ಪ್ರಕಾರನಿರ್ದೇಶ CC_MD4_CTX my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ my_md4_init(my_md4_ctx *ctx)
{
  ಪ್ರತ್ಯಾಗಮ CC_MD4_Init(ctx);  /* always returns 1 */
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು, unsigned long len)
{
  (ಶೂನ್ಯ)CC_MD4_Update(ctx, ಒಳಹರಿವು, (CC_LONG)len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(unsigned ವರ್ಣ *digest, my_md4_ctx *ctx)
{
  (ಶೂನ್ಯ)CC_MD4_Final(digest, ctx);
}

#ನೋಚೇದ್ಯದಿ defined(USE_WIN32_CRYPTO)
#ಅನ್ತರ್ಭಾವ <wincrypt.h>

ಸಂರಚನಾ md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ md4_ctx my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  ಯದಿ(!CryptAcquireContext(&ctx->hCryptProv, ರಿಕ್ತ, ರಿಕ್ತ, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ಪ್ರತ್ಯಾಗಮ 0;

  ಯದಿ(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು, unsigned long len)
{
  CryptHashData(ctx->hHash, (ಧ್ರುವ BYTE *)ಒಳಹರಿವು, (unsigned ಪೂರ್ಣಾಙ್ಕ)len, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(unsigned ವರ್ಣ *digest, my_md4_ctx *ctx)
{
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ರಿಕ್ತ, &length, 0);
  ಯದಿ(length == MD4_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  ಯದಿ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ಯದಿ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <nettle/md4.h>
#ಅನ್ತರ್ಭಾವ <nettle/version.h>

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ md4_ctx my_md4_ctx;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು, unsigned long len)
{
  md4_update(ctx, len, ಒಳಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(unsigned ವರ್ಣ *digest, my_md4_ctx *ctx)
{
#ಯದಿ NETTLE_VERSION_MAJOR >= 4
  md4_digest(ctx, digest);
#ಅನ್ಯಥಾ
  md4_digest(ctx, MD4_DIGEST_SIZE, digest);
#endif
}

#ಅನ್ಯಥಾ
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

ಸಂರಚನಾ md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  unsigned ವರ್ಣ buffer[64];
  uint32_t block[16];
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ md4_ctx my_md4_ctx;

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
#ಯದಿ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define MD4_SET(n) (*(ಧ್ರುವ uint32_t *)(ಧ್ರುವ ಶೂನ್ಯ *)&ptr[(n) * 4])
#define MD4_GET(n) MD4_SET(n)
#ಅನ್ಯಥಾ
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
ಸ್ಥಿರ ಧ್ರುವ ಶೂನ್ಯ *my_md4_body(my_md4_ctx *ctx,
                               ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು, unsigned long size)
{
  ಧ್ರುವ unsigned ವರ್ಣ *ptr;
  uint32_t a, b, c, d;

  ptr = (ಧ್ರುವ unsigned ವರ್ಣ *)ಒಳಹರಿವು;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  ಕುರು {
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
  } ಯಾವತ್(size -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  ಪ್ರತ್ಯಾಗಮ ptr;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_update(my_md4_ctx *ctx,
                          ಧ್ರುವ ಶೂನ್ಯ *ಒಳಹರಿವು, unsigned long len)
{
  uint32_t saved_lo;
  unsigned long used;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ಯದಿ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  ಯದಿ(used) {
    unsigned long available = 64 - used;

    ಯದಿ(len < available) {
      memcpy(&ctx->buffer[used], ಒಳಹರಿವು, len);
      ಪ್ರತ್ಯಾಗಮ;
    }

    memcpy(&ctx->buffer[used], ಒಳಹರಿವು, available);
    ಒಳಹರಿವು = (ಧ್ರುವ unsigned ವರ್ಣ *)ಒಳಹರಿವು + available;
    len -= available;
    my_md4_body(ctx, ctx->buffer, 64);
  }

  ಯದಿ(len >= 64) {
    ಒಳಹರಿವು = my_md4_body(ctx, ಒಳಹರಿವು, len & ~(unsigned long)0x3f);
    len &= 0x3f;
  }

  memcpy(ctx->buffer, ಒಳಹರಿವು, len);
}

ಸ್ಥಿರ ಶೂನ್ಯ my_md4_final(unsigned ವರ್ಣ *digest, my_md4_ctx *ctx)
{
  unsigned long used, available;

  used = ctx->lo & 0x3f;

  ctx->buffer[used++] = 0x80;

  available = 64 - used;

  ಯದಿ(available < 8) {
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

  memset(ctx, 0, ಪರಿಮಾಣ(*ctx));
}

#endif /* CRYPTO LIBS */

CURLcode Curl_md4it(unsigned ವರ್ಣ *output,
                    ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು, ಧ್ರುವ size_t len)
{
  my_md4_ctx ctx;
  ಯದಿ(!my_md4_init(&ctx))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  my_md4_update(&ctx, ಒಳಹರಿವು, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#endif /* USE_CURL_NTLM_CORE */
