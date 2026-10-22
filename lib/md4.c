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
#जर OPENSSL_VERSION_संख्या >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#define OPENSSL_NO_MD4
#अन्यथा
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#अन्तर्भाव <openssl/opensslconf.h>
#endif
#endif /* USE_OPENSSL */

#ifdef USE_WOLFSSL
#अन्तर्भाव <wolfssl/options.h>
#endif

#जर defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#अन्तर्भाव <openssl/evp.h>

प्रकारव्याख्या EVP_MD_CTX *my_md4_ctx;

स्थिर पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  जर(!*ctx)
    परत 0;

  जर(!EVP_DigestInit_ex(*ctx, EVP_md4(), रिक्त)) {
    EVP_MD_CTX_free(*ctx);
    परत 0;
  }
  परत 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          स्थिरांक अचिन्हित अक्षर *आदान, अचिन्हित पूर्णांक len)
{
  (शून्य)EVP_DigestUpdate(*ctx, आदान, len);
}

स्थिर शून्य my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  (शून्य)EVP_DigestFinal_ex(*ctx, digest, रिक्त);
  EVP_MD_CTX_free(*ctx);
}

#किंवाजर defined(USE_WOLFSSL) && !defined(NO_MD4)
#अन्तर्भाव <wolfssl/wolfcrypt/md4.h>
#अन्तर्भाव <wolfssl/version.h>

#जर LIBWOLFSSL_VERSION_HEX >= 0x05007006
प्रकारव्याख्या wc_Md4 my_md4_ctx;
#अन्यथा
प्रकारव्याख्या Md4 my_md4_ctx;
#endif

स्थिर पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  परत 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          स्थिरांक शून्य *आदान, अचिन्हित दीर्घ len)
{
  wc_Md4Update(ctx, आदान, (word32)len);
}

स्थिर शून्य my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#किंवाजर (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#अन्तर्भाव <CommonCrypto/CommonDigest.h>

प्रकारव्याख्या CC_MD4_CTX my_md4_ctx;

स्थिर पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  परत CC_MD4_Init(ctx);  /* always returns 1 */
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          स्थिरांक शून्य *आदान, अचिन्हित दीर्घ len)
{
  (शून्य)CC_MD4_Update(ctx, आदान, (CC_LONG)len);
}

स्थिर शून्य my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  (शून्य)CC_MD4_Final(digest, ctx);
}

#किंवाजर defined(USE_WIN32_CRYPTO)
#अन्तर्भाव <wincrypt.h>

संरचना md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
प्रकारव्याख्या संरचना md4_ctx my_md4_ctx;

स्थिर पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  जर(!CryptAcquireContext(&ctx->hCryptProv, रिक्त, रिक्त, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    परत 0;

  जर(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    परत 0;
  }

  परत 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          स्थिरांक शून्य *आदान, अचिन्हित दीर्घ len)
{
  CryptHashData(ctx->hHash, (स्थिरांक BYTE *)आदान, (अचिन्हित पूर्णांक)len, 0);
}

स्थिर शून्य my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  अचिन्हित दीर्घ length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, रिक्त, &length, 0);
  जर(length == MD4_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  जर(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  जर(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#किंवाजर defined(USE_GNUTLS)
#अन्तर्भाव <nettle/md4.h>
#अन्तर्भाव <nettle/version.h>

प्रकारव्याख्या संरचना md4_ctx my_md4_ctx;

स्थिर पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  परत 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          स्थिरांक शून्य *आदान, अचिन्हित दीर्घ len)
{
  md4_update(ctx, len, आदान);
}

स्थिर शून्य my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
#जर NETTLE_VERSION_MAJOR >= 4
  md4_digest(ctx, digest);
#अन्यथा
  md4_digest(ctx, MD4_DIGEST_SIZE, digest);
#endif
}

#अन्यथा
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
  अचिन्हित अक्षर buffer[64];
  uint32_t block[16];
};
प्रकारव्याख्या संरचना md4_ctx my_md4_ctx;

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
#जर defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define MD4_SET(n) (*(स्थिरांक uint32_t *)(स्थिरांक शून्य *)&ptr[(n) * 4])
#define MD4_GET(n) MD4_SET(n)
#अन्यथा
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
स्थिर स्थिरांक शून्य *my_md4_body(my_md4_ctx *ctx,
                               स्थिरांक शून्य *आदान, अचिन्हित दीर्घ size)
{
  स्थिरांक अचिन्हित अक्षर *ptr;
  uint32_t a, b, c, d;

  ptr = (स्थिरांक अचिन्हित अक्षर *)आदान;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  करा {
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
  } पर्यंत(size -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  परत ptr;
}

स्थिर पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  परत 1;
}

स्थिर शून्य my_md4_update(my_md4_ctx *ctx,
                          स्थिरांक शून्य *आदान, अचिन्हित दीर्घ len)
{
  uint32_t saved_lo;
  अचिन्हित दीर्घ used;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  जर(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  जर(used) {
    अचिन्हित दीर्घ available = 64 - used;

    जर(len < available) {
      स_नकल(&ctx->buffer[used], आदान, len);
      परत;
    }

    स_नकल(&ctx->buffer[used], आदान, available);
    आदान = (स्थिरांक अचिन्हित अक्षर *)आदान + available;
    len -= available;
    my_md4_body(ctx, ctx->buffer, 64);
  }

  जर(len >= 64) {
    आदान = my_md4_body(ctx, आदान, len & ~(अचिन्हित दीर्घ)0x3f);
    len &= 0x3f;
  }

  स_नकल(ctx->buffer, आदान, len);
}

स्थिर शून्य my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  अचिन्हित दीर्घ used, available;

  used = ctx->lo & 0x3f;

  ctx->buffer[used++] = 0x80;

  available = 64 - used;

  जर(available < 8) {
    स_रखो(&ctx->buffer[used], 0, available);
    my_md4_body(ctx, ctx->buffer, 64);
    used = 0;
    available = 64;
  }

  स_रखो(&ctx->buffer[used], 0, available - 8);

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

  स_रखो(ctx, 0, आकार(*ctx));
}

#endif /* CRYPTO LIBS */

CURLcode Curl_md4it(अचिन्हित अक्षर *output,
                    स्थिरांक अचिन्हित अक्षर *आदान, स्थिरांक माप_प्रकार len)
{
  my_md4_ctx ctx;
  जर(!my_md4_init(&ctx))
    परत CURLE_FAILED_INIT;
  my_md4_update(&ctx, आदान, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  परत CURLE_OK;
}

#endif /* USE_CURL_NTLM_CORE */
