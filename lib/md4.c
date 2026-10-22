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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

#अन्तर्भाव "curl_md4.h"

#ifdef USE_OPENSSL
#अन्तर्भाव <openssl/opensslv.h>
#जइ OPENSSL_VERSION_सङ्ख्या >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#define OPENSSL_NO_MD4
#अण्णहा
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#अन्तर्भाव <openssl/opensslconf.h>
#endif
#endif /* USE_OPENSSL */

#ifdef USE_WOLFSSL
#अन्तर्भाव <wolfssl/options.h>
#endif

#जइ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#अन्तर्भाव <openssl/evp.h>

प्रकारनिर्देश EVP_MD_CTX *my_md4_ctx;

स्थिर पूर्णाङ्क my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  जइ(!*ctx)
    प्रत्यागम 0;

  जइ(!EVP_DigestInit_ex(*ctx, EVP_md4(), रिक्त)) {
    EVP_MD_CTX_free(*ctx);
    प्रत्यागम 0;
  }
  प्रत्यागम 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          ध्रुव unsigned वर्ण *निवेशः, unsigned पूर्णाङ्क len)
{
  (शून्य)EVP_DigestUpdate(*ctx, निवेशः, len);
}

स्थिर शून्य my_md4_final(unsigned वर्ण *digest, my_md4_ctx *ctx)
{
  (शून्य)EVP_DigestFinal_ex(*ctx, digest, रिक्त);
  EVP_MD_CTX_free(*ctx);
}

#नोचेद्यदि defined(USE_WOLFSSL) && !defined(NO_MD4)
#अन्तर्भाव <wolfssl/wolfcrypt/md4.h>
#अन्तर्भाव <wolfssl/version.h>

#जइ LIBWOLFSSL_VERSION_HEX >= 0x05007006
प्रकारनिर्देश wc_Md4 my_md4_ctx;
#अण्णहा
प्रकारनिर्देश Md4 my_md4_ctx;
#endif

स्थिर पूर्णाङ्क my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  प्रत्यागम 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          ध्रुव शून्य *निवेशः, unsigned long len)
{
  wc_Md4Update(ctx, निवेशः, (word32)len);
}

स्थिर शून्य my_md4_final(unsigned वर्ण *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#नोचेद्यदि (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#अन्तर्भाव <CommonCrypto/CommonDigest.h>

प्रकारनिर्देश CC_MD4_CTX my_md4_ctx;

स्थिर पूर्णाङ्क my_md4_init(my_md4_ctx *ctx)
{
  प्रत्यागम CC_MD4_Init(ctx);  /* always returns 1 */
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          ध्रुव शून्य *निवेशः, unsigned long len)
{
  (शून्य)CC_MD4_Update(ctx, निवेशः, (CC_LONG)len);
}

स्थिर शून्य my_md4_final(unsigned वर्ण *digest, my_md4_ctx *ctx)
{
  (शून्य)CC_MD4_Final(digest, ctx);
}

#नोचेद्यदि defined(USE_WIN32_CRYPTO)
#अन्तर्भाव <wincrypt.h>

संरचना md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
प्रकारनिर्देश संरचना md4_ctx my_md4_ctx;

स्थिर पूर्णाङ्क my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  जइ(!CryptAcquireContext(&ctx->hCryptProv, रिक्त, रिक्त, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    प्रत्यागम 0;

  जइ(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    प्रत्यागम 0;
  }

  प्रत्यागम 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          ध्रुव शून्य *निवेशः, unsigned long len)
{
  CryptHashData(ctx->hHash, (ध्रुव BYTE *)निवेशः, (unsigned पूर्णाङ्क)len, 0);
}

स्थिर शून्य my_md4_final(unsigned वर्ण *digest, my_md4_ctx *ctx)
{
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, रिक्त, &length, 0);
  जइ(length == MD4_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  जइ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  जइ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#नोचेद्यदि defined(USE_GNUTLS)
#अन्तर्भाव <nettle/md4.h>
#अन्तर्भाव <nettle/version.h>

प्रकारनिर्देश संरचना md4_ctx my_md4_ctx;

स्थिर पूर्णाङ्क my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  प्रत्यागम 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          ध्रुव शून्य *निवेशः, unsigned long len)
{
  md4_update(ctx, len, निवेशः);
}

स्थिर शून्य my_md4_final(unsigned वर्ण *digest, my_md4_ctx *ctx)
{
#जइ NETTLE_VERSION_MAJOR >= 4
  md4_digest(ctx, digest);
#अण्णहा
  md4_digest(ctx, MD4_DIGEST_SIZE, digest);
#endif
}

#अण्णहा
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

संरचना md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  unsigned वर्ण buffer[64];
  uint32_t block[16];
};
प्रकारनिर्देश संरचना md4_ctx my_md4_ctx;

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
#जइ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define MD4_SET(n) (*(ध्रुव uint32_t *)(ध्रुव शून्य *)&ptr[(n) * 4])
#define MD4_GET(n) MD4_SET(n)
#अण्णहा
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
स्थिर ध्रुव शून्य *my_md4_body(my_md4_ctx *ctx,
                               ध्रुव शून्य *निवेशः, unsigned long size)
{
  ध्रुव unsigned वर्ण *ptr;
  uint32_t a, b, c, d;

  ptr = (ध्रुव unsigned वर्ण *)निवेशः;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  करेहि {
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
  } जाव(size -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  प्रत्यागम ptr;
}

स्थिर पूर्णाङ्क my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  प्रत्यागम 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          ध्रुव शून्य *निवेशः, unsigned long len)
{
  uint32_t saved_lo;
  unsigned long used;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  जइ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  जइ(used) {
    unsigned long available = 64 - used;

    जइ(len < available) {
      memcpy(&ctx->buffer[used], निवेशः, len);
      प्रत्यागम;
    }

    memcpy(&ctx->buffer[used], निवेशः, available);
    निवेशः = (ध्रुव unsigned वर्ण *)निवेशः + available;
    len -= available;
    my_md4_body(ctx, ctx->buffer, 64);
  }

  जइ(len >= 64) {
    निवेशः = my_md4_body(ctx, निवेशः, len & ~(unsigned long)0x3f);
    len &= 0x3f;
  }

  memcpy(ctx->buffer, निवेशः, len);
}

स्थिर शून्य my_md4_final(unsigned वर्ण *digest, my_md4_ctx *ctx)
{
  unsigned long used, available;

  used = ctx->lo & 0x3f;

  ctx->buffer[used++] = 0x80;

  available = 64 - used;

  जइ(available < 8) {
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

  memset(ctx, 0, परिमाण(*ctx));
}

#endif /* CRYPTO LIBS */

CURLcode Curl_md4it(unsigned वर्ण *output,
                    ध्रुव unsigned वर्ण *निवेशः, ध्रुव size_t len)
{
  my_md4_ctx ctx;
  जइ(!my_md4_init(&ctx))
    प्रत्यागम CURLE_FAILED_INIT;
  my_md4_update(&ctx, निवेशः, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  प्रत्यागम CURLE_OK;
}

#endif /* USE_CURL_NTLM_CORE */
