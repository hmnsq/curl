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

#اگر_تعریف_شدہ USE_CURL_NTLM_CORE

#شامل "curl_md4.h"

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/opensslv.h>
#اگر OPENSSL_نسخہ_نمبر >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#تعریف_کرو OPENSSL_NO_MD4
#ورنہ
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#شامل <openssl/opensslconf.h>
#اگر_ختم
#اگر_ختم /* USE_OPENSSL */

#اگر_تعریف_شدہ USE_WOLFSSL
#شامل <wolfssl/options.h>
#اگر_ختم

#اگر defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#شامل <openssl/evp.h>

قسمتعریف EVP_MD_CTX *my_md4_ctx;

ساکن صحیح my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  اگر(!*ctx)
    واپس 0;

  اگر(!EVP_DigestInit_ex(*ctx, EVP_md4(), تہی)) {
    EVP_MD_CTX_free(*ctx);
    واپس 0;
  }
  واپس 1;
}

ساکن خالی my_md4_update(my_md4_ctx *ctx,
                          ثابت غیرعلامتی حرف *داخلہ, غیرعلامتی صحیح len)
{
  (خالی)EVP_DigestUpdate(*ctx, داخلہ, len);
}

ساکن خالی my_md4_final(غیرعلامتی حرف *digest, my_md4_ctx *ctx)
{
  (خالی)EVP_DigestFinal_ex(*ctx, digest, تہی);
  EVP_MD_CTX_free(*ctx);
}

#وگرنہ_اگر defined(USE_WOLFSSL) && !defined(NO_MD4)
#شامل <wolfssl/wolfcrypt/md4.h>
#شامل <wolfssl/version.h>

#اگر LIBWOLFSSL_نسخہ_HEX >= 0x05007006
قسمتعریف wc_Md4 my_md4_ctx;
#ورنہ
قسمتعریف Md4 my_md4_ctx;
#اگر_ختم

ساکن صحیح my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  واپس 1;
}

ساکن خالی my_md4_update(my_md4_ctx *ctx,
                          ثابت خالی *داخلہ, غیرعلامتی طویل len)
{
  wc_Md4Update(ctx, داخلہ, (word32)len);
}

ساکن خالی my_md4_final(غیرعلامتی حرف *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
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

قسمتعریف CC_MD4_CTX my_md4_ctx;

ساکن صحیح my_md4_init(my_md4_ctx *ctx)
{
  واپس CC_MD4_Init(ctx);  /* always returns 1 */
}

ساکن خالی my_md4_update(my_md4_ctx *ctx,
                          ثابت خالی *داخلہ, غیرعلامتی طویل len)
{
  (خالی)CC_MD4_Update(ctx, داخلہ, (CC_LONG)len);
}

ساکن خالی my_md4_final(غیرعلامتی حرف *digest, my_md4_ctx *ctx)
{
  (خالی)CC_MD4_Final(digest, ctx);
}

#وگرنہ_اگر defined(USE_WIN32_CRYPTO)
#شامل <wincrypt.h>

ساخت md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
قسمتعریف ساخت md4_ctx my_md4_ctx;

ساکن صحیح my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  اگر(!CryptAcquireContext(&ctx->hCryptProv, تہی, تہی, PROV_RSA_بھرا,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    واپس 0;

  اگر(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    واپس 0;
  }

  واپس 1;
}

ساکن خالی my_md4_update(my_md4_ctx *ctx,
                          ثابت خالی *داخلہ, غیرعلامتی طویل len)
{
  Crypt_Hash_ڈیٹا(ctx->hHash, (ثابت BYTE *)داخلہ, (غیرعلامتی صحیح)len, 0);
}

ساکن خالی my_md4_final(غیرعلامتی حرف *digest, my_md4_ctx *ctx)
{
  غیرعلامتی طویل لمبائی = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, تہی, &لمبائی, 0);
  اگر(لمبائی == MD4_DIGEST_لمبائی)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &لمبائی, 0);

  اگر(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  اگر(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#وگرنہ_اگر defined(USE_GNUTLS)
#شامل <nettle/md4.h>
#شامل <nettle/version.h>

قسمتعریف ساخت md4_ctx my_md4_ctx;

ساکن صحیح my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  واپس 1;
}

ساکن خالی my_md4_update(my_md4_ctx *ctx,
                          ثابت خالی *داخلہ, غیرعلامتی طویل len)
{
  md4_update(ctx, len, داخلہ);
}

ساکن خالی my_md4_final(غیرعلامتی حرف *digest, my_md4_ctx *ctx)
{
#اگر NETTLE_نسخہ_MAJOR >= 4
  md4_digest(ctx, digest);
#ورنہ
  md4_digest(ctx, MD4_DIGEST_سائز, digest);
#اگر_ختم
}

#ورنہ
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

ساخت md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  غیرعلامتی حرف بفر[64];
  uint32_t بلاک[16];
};
قسمتعریف ساخت md4_ctx my_md4_ctx;

/*
 * The basic MD4 functions.
 *
 * F and G are optimized compared to their RFC 1320 definitions, with the
 * optimization for F borrowed from Colin Plumb's MD5 implementation.
 */
#تعریف_کرو MD4_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#تعریف_کرو MD4_G(x, y, z)                  (((x) & ((y) | (z))) | ((y) & (z)))
#تعریف_کرو MD4_H(x, y, z)                  ((x) ^ (y) ^ (z))

/*
 * The MD4 transformation for all three rounds.
 */
#تعریف_کرو MD4_قدم(f, a, b, c, d, x, s) \
  (a) += f(b, c, d) + (x); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is an optimization. Nothing will break if it does not work.
 */
#اگر defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#تعریف_کرو MD4_سیٹ(n) (*(ثابت uint32_t *)(ثابت خالی *)&ptr[(n) * 4])
#تعریف_کرو MD4_GET(n) MD4_سیٹ(n)
#ورنہ
#تعریف_کرو MD4_سیٹ(n) (ctx->بلاک[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#تعریف_کرو MD4_GET(n) ctx->بلاک[n]
#اگر_ختم

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
ساکن ثابت خالی *my_md4_body(my_md4_ctx *ctx,
                               ثابت خالی *داخلہ, غیرعلامتی طویل سائز)
{
  ثابت غیرعلامتی حرف *ptr;
  uint32_t a, b, c, d;

  ptr = (ثابت غیرعلامتی حرف *)داخلہ;

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
    MD4_قدم(MD4_F, a, b, c, d, MD4_سیٹ(0), 3)
    MD4_قدم(MD4_F, d, a, b, c, MD4_سیٹ(1), 7)
    MD4_قدم(MD4_F, c, d, a, b, MD4_سیٹ(2), 11)
    MD4_قدم(MD4_F, b, c, d, a, MD4_سیٹ(3), 19)
    MD4_قدم(MD4_F, a, b, c, d, MD4_سیٹ(4), 3)
    MD4_قدم(MD4_F, d, a, b, c, MD4_سیٹ(5), 7)
    MD4_قدم(MD4_F, c, d, a, b, MD4_سیٹ(6), 11)
    MD4_قدم(MD4_F, b, c, d, a, MD4_سیٹ(7), 19)
    MD4_قدم(MD4_F, a, b, c, d, MD4_سیٹ(8), 3)
    MD4_قدم(MD4_F, d, a, b, c, MD4_سیٹ(9), 7)
    MD4_قدم(MD4_F, c, d, a, b, MD4_سیٹ(10), 11)
    MD4_قدم(MD4_F, b, c, d, a, MD4_سیٹ(11), 19)
    MD4_قدم(MD4_F, a, b, c, d, MD4_سیٹ(12), 3)
    MD4_قدم(MD4_F, d, a, b, c, MD4_سیٹ(13), 7)
    MD4_قدم(MD4_F, c, d, a, b, MD4_سیٹ(14), 11)
    MD4_قدم(MD4_F, b, c, d, a, MD4_سیٹ(15), 19)

    /* Round 2 */
    MD4_قدم(MD4_G, a, b, c, d, MD4_GET(0) + 0x5a827999, 3)
    MD4_قدم(MD4_G, d, a, b, c, MD4_GET(4) + 0x5a827999, 5)
    MD4_قدم(MD4_G, c, d, a, b, MD4_GET(8) + 0x5a827999, 9)
    MD4_قدم(MD4_G, b, c, d, a, MD4_GET(12) + 0x5a827999, 13)
    MD4_قدم(MD4_G, a, b, c, d, MD4_GET(1) + 0x5a827999, 3)
    MD4_قدم(MD4_G, d, a, b, c, MD4_GET(5) + 0x5a827999, 5)
    MD4_قدم(MD4_G, c, d, a, b, MD4_GET(9) + 0x5a827999, 9)
    MD4_قدم(MD4_G, b, c, d, a, MD4_GET(13) + 0x5a827999, 13)
    MD4_قدم(MD4_G, a, b, c, d, MD4_GET(2) + 0x5a827999, 3)
    MD4_قدم(MD4_G, d, a, b, c, MD4_GET(6) + 0x5a827999, 5)
    MD4_قدم(MD4_G, c, d, a, b, MD4_GET(10) + 0x5a827999, 9)
    MD4_قدم(MD4_G, b, c, d, a, MD4_GET(14) + 0x5a827999, 13)
    MD4_قدم(MD4_G, a, b, c, d, MD4_GET(3) + 0x5a827999, 3)
    MD4_قدم(MD4_G, d, a, b, c, MD4_GET(7) + 0x5a827999, 5)
    MD4_قدم(MD4_G, c, d, a, b, MD4_GET(11) + 0x5a827999, 9)
    MD4_قدم(MD4_G, b, c, d, a, MD4_GET(15) + 0x5a827999, 13)

    /* Round 3 */
    MD4_قدم(MD4_H, a, b, c, d, MD4_GET(0) + 0x6ed9eba1, 3)
    MD4_قدم(MD4_H, d, a, b, c, MD4_GET(8) + 0x6ed9eba1, 9)
    MD4_قدم(MD4_H, c, d, a, b, MD4_GET(4) + 0x6ed9eba1, 11)
    MD4_قدم(MD4_H, b, c, d, a, MD4_GET(12) + 0x6ed9eba1, 15)
    MD4_قدم(MD4_H, a, b, c, d, MD4_GET(2) + 0x6ed9eba1, 3)
    MD4_قدم(MD4_H, d, a, b, c, MD4_GET(10) + 0x6ed9eba1, 9)
    MD4_قدم(MD4_H, c, d, a, b, MD4_GET(6) + 0x6ed9eba1, 11)
    MD4_قدم(MD4_H, b, c, d, a, MD4_GET(14) + 0x6ed9eba1, 15)
    MD4_قدم(MD4_H, a, b, c, d, MD4_GET(1) + 0x6ed9eba1, 3)
    MD4_قدم(MD4_H, d, a, b, c, MD4_GET(9) + 0x6ed9eba1, 9)
    MD4_قدم(MD4_H, c, d, a, b, MD4_GET(5) + 0x6ed9eba1, 11)
    MD4_قدم(MD4_H, b, c, d, a, MD4_GET(13) + 0x6ed9eba1, 15)
    MD4_قدم(MD4_H, a, b, c, d, MD4_GET(3) + 0x6ed9eba1, 3)
    MD4_قدم(MD4_H, d, a, b, c, MD4_GET(11) + 0x6ed9eba1, 9)
    MD4_قدم(MD4_H, c, d, a, b, MD4_GET(7) + 0x6ed9eba1, 11)
    MD4_قدم(MD4_H, b, c, d, a, MD4_GET(15) + 0x6ed9eba1, 15)

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

ساکن صحیح my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  واپس 1;
}

ساکن خالی my_md4_update(my_md4_ctx *ctx,
                          ثابت خالی *داخلہ, غیرعلامتی طویل len)
{
  uint32_t saved_lo;
  غیرعلامتی طویل مستعمل;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  اگر(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  مستعمل = saved_lo & 0x3f;

  اگر(مستعمل) {
    غیرعلامتی طویل دستیاب = 64 - مستعمل;

    اگر(len < دستیاب) {
      حافظہ_نقل(&ctx->بفر[مستعمل], داخلہ, len);
      واپس;
    }

    حافظہ_نقل(&ctx->بفر[مستعمل], داخلہ, دستیاب);
    داخلہ = (ثابت غیرعلامتی حرف *)داخلہ + دستیاب;
    len -= دستیاب;
    my_md4_body(ctx, ctx->بفر, 64);
  }

  اگر(len >= 64) {
    داخلہ = my_md4_body(ctx, داخلہ, len & ~(غیرعلامتی طویل)0x3f);
    len &= 0x3f;
  }

  حافظہ_نقل(ctx->بفر, داخلہ, len);
}

ساکن خالی my_md4_final(غیرعلامتی حرف *digest, my_md4_ctx *ctx)
{
  غیرعلامتی طویل مستعمل, دستیاب;

  مستعمل = ctx->lo & 0x3f;

  ctx->بفر[مستعمل++] = 0x80;

  دستیاب = 64 - مستعمل;

  اگر(دستیاب < 8) {
    memset(&ctx->بفر[مستعمل], 0, دستیاب);
    my_md4_body(ctx, ctx->بفر, 64);
    مستعمل = 0;
    دستیاب = 64;
  }

  memset(&ctx->بفر[مستعمل], 0, دستیاب - 8);

  ctx->lo <<= 3;
  ctx->بفر[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->بفر[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->بفر[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->بفر[59] = curlx_ultouc((ctx->lo >> 24) & 0xff);
  ctx->بفر[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->بفر[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->بفر[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->بفر[63] = curlx_ultouc(ctx->hi >> 24);

  my_md4_body(ctx, ctx->بفر, 64);

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

CURLcode Curl_md4it(غیرعلامتی حرف *output,
                    ثابت غیرعلامتی حرف *داخلہ, ثابت size_t len)
{
  my_md4_ctx ctx;
  اگر(!my_md4_init(&ctx))
    واپس CURLE_FAILED_INIT;
  my_md4_update(&ctx, داخلہ, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  واپس CURLE_OK;
}

#اگر_ختم /* USE_CURL_NTLM_CORE */
