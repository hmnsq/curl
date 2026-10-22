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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_CURL_NTLM_CORE

#అన్తర్భావ "curl_md4.h"

#నిర్వచితమైతే USE_OPENSSL
#అన్తర్భావ <openssl/opensslv.h>
#ఒకవేళ OPENSSL_సంస్కరణ_సంఖ్య >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#నిర్వచించు OPENSSL_NO_MD4
#లేకపోతే
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#అన్తర్భావ <openssl/opensslconf.h>
#అయితే_ముగింపు
#అయితే_ముగింపు /* USE_OPENSSL */

#నిర్వచితమైతే USE_WOLFSSL
#అన్తర్భావ <wolfssl/options.h>
#అయితే_ముగింపు

#ఒకవేళ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#అన్తర్భావ <openssl/evp.h>

రకనిర్వచనం EVP_MD_CTX *my_md4_ctx;

స్థిర పూర్ణాంకం my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  ఒకవేళ(!*ctx)
    తిరిగి 0;

  ఒకవేళ(!EVP_DigestInit_ex(*ctx, EVP_md4(), ఖాళీ)) {
    EVP_MD_CTX_free(*ctx);
    తిరిగి 0;
  }
  తిరిగి 1;
}

స్థిర శూన్య my_md4_update(my_md4_ctx *ctx,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  (శూన్య)EVP_DigestUpdate(*ctx, ఆదానం, len);
}

స్థిర శూన్య my_md4_final(గుర్తులేని అక్షరం *digest, my_md4_ctx *ctx)
{
  (శూన్య)EVP_DigestFinal_ex(*ctx, digest, ఖాళీ);
  EVP_MD_CTX_free(*ctx);
}

#లేదావేళ defined(USE_WOLFSSL) && !defined(NO_MD4)
#అన్తర్భావ <wolfssl/wolfcrypt/md4.h>
#అన్తర్భావ <wolfssl/version.h>

#ఒకవేళ LIBWOLFSSL_సంస్కరణ_HEX >= 0x05007006
రకనిర్వచనం wc_Md4 my_md4_ctx;
#లేకపోతే
రకనిర్వచనం Md4 my_md4_ctx;
#అయితే_ముగింపు

స్థిర పూర్ణాంకం my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  తిరిగి 1;
}

స్థిర శూన్య my_md4_update(my_md4_ctx *ctx,
                          స్థిరరాశి శూన్య *ఆదానం, గుర్తులేని దీర్ఘ len)
{
  wc_Md4Update(ctx, ఆదానం, (word32)len);
}

స్థిర శూన్య my_md4_final(గుర్తులేని అక్షరం *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#లేదావేళ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#అన్తర్భావ <CommonCrypto/CommonDigest.h>

రకనిర్వచనం CC_MD4_CTX my_md4_ctx;

స్థిర పూర్ణాంకం my_md4_init(my_md4_ctx *ctx)
{
  తిరిగి CC_MD4_Init(ctx);  /* always returns 1 */
}

స్థిర శూన్య my_md4_update(my_md4_ctx *ctx,
                          స్థిరరాశి శూన్య *ఆదానం, గుర్తులేని దీర్ఘ len)
{
  (శూన్య)CC_MD4_Update(ctx, ఆదానం, (CC_LONG)len);
}

స్థిర శూన్య my_md4_final(గుర్తులేని అక్షరం *digest, my_md4_ctx *ctx)
{
  (శూన్య)CC_MD4_Final(digest, ctx);
}

#లేదావేళ defined(USE_WIN32_CRYPTO)
#అన్తర్భావ <wincrypt.h>

నిర్మాణం md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
రకనిర్వచనం నిర్మాణం md4_ctx my_md4_ctx;

స్థిర పూర్ణాంకం my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  ఒకవేళ(!CryptAcquireContext(&ctx->hCryptProv, ఖాళీ, ఖాళీ, PROV_RSA_నిండిన,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    తిరిగి 0;

  ఒకవేళ(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    తిరిగి 0;
  }

  తిరిగి 1;
}

స్థిర శూన్య my_md4_update(my_md4_ctx *ctx,
                          స్థిరరాశి శూన్య *ఆదానం, గుర్తులేని దీర్ఘ len)
{
  Crypt_Hash_దత్తాంశం(ctx->hHash, (స్థిరరాశి BYTE *)ఆదానం, (గుర్తులేని పూర్ణాంకం)len, 0);
}

స్థిర శూన్య my_md4_final(గుర్తులేని అక్షరం *digest, my_md4_ctx *ctx)
{
  గుర్తులేని దీర్ఘ నిడివి = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ఖాళీ, &నిడివి, 0);
  ఒకవేళ(నిడివి == MD4_DIGEST_నిడివి)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &నిడివి, 0);

  ఒకవేళ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ఒకవేళ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#లేదావేళ defined(USE_GNUTLS)
#అన్తర్భావ <nettle/md4.h>
#అన్తర్భావ <nettle/version.h>

రకనిర్వచనం నిర్మాణం md4_ctx my_md4_ctx;

స్థిర పూర్ణాంకం my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  తిరిగి 1;
}

స్థిర శూన్య my_md4_update(my_md4_ctx *ctx,
                          స్థిరరాశి శూన్య *ఆదానం, గుర్తులేని దీర్ఘ len)
{
  md4_update(ctx, len, ఆదానం);
}

స్థిర శూన్య my_md4_final(గుర్తులేని అక్షరం *digest, my_md4_ctx *ctx)
{
#ఒకవేళ NETTLE_సంస్కరణ_MAJOR >= 4
  md4_digest(ctx, digest);
#లేకపోతే
  md4_digest(ctx, MD4_DIGEST_కొలత, digest);
#అయితే_ముగింపు
}

#లేకపోతే
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

నిర్మాణం md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  గుర్తులేని అక్షరం బఫరు[64];
  uint32_t ఖండం[16];
};
రకనిర్వచనం నిర్మాణం md4_ctx my_md4_ctx;

/*
 * The basic MD4 functions.
 *
 * F and G are optimized compared to their RFC 1320 definitions, with the
 * optimization for F borrowed from Colin Plumb's MD5 implementation.
 */
#నిర్వచించు MD4_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#నిర్వచించు MD4_G(x, y, z)                  (((x) & ((y) | (z))) | ((y) & (z)))
#నిర్వచించు MD4_H(x, y, z)                  ((x) ^ (y) ^ (z))

/*
 * The MD4 transformation for all three rounds.
 */
#నిర్వచించు MD4_అడుగు(f, a, b, c, d, x, s) \
  (a) += f(b, c, d) + (x); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is an optimization. Nothing will break if it does not work.
 */
#ఒకవేళ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#నిర్వచించు MD4_సమితి(n) (*(స్థిరరాశి uint32_t *)(స్థిరరాశి శూన్య *)&ptr[(n) * 4])
#నిర్వచించు MD4_GET(n) MD4_సమితి(n)
#లేకపోతే
#నిర్వచించు MD4_సమితి(n) (ctx->ఖండం[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#నిర్వచించు MD4_GET(n) ctx->ఖండం[n]
#అయితే_ముగింపు

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
స్థిర స్థిరరాశి శూన్య *my_md4_body(my_md4_ctx *ctx,
                               స్థిరరాశి శూన్య *ఆదానం, గుర్తులేని దీర్ఘ కొలత)
{
  స్థిరరాశి గుర్తులేని అక్షరం *ptr;
  uint32_t a, b, c, d;

  ptr = (స్థిరరాశి గుర్తులేని అక్షరం *)ఆదానం;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  చేయి {
    uint32_t saved_a, saved_b, saved_c, saved_d;

    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    MD4_అడుగు(MD4_F, a, b, c, d, MD4_సమితి(0), 3)
    MD4_అడుగు(MD4_F, d, a, b, c, MD4_సమితి(1), 7)
    MD4_అడుగు(MD4_F, c, d, a, b, MD4_సమితి(2), 11)
    MD4_అడుగు(MD4_F, b, c, d, a, MD4_సమితి(3), 19)
    MD4_అడుగు(MD4_F, a, b, c, d, MD4_సమితి(4), 3)
    MD4_అడుగు(MD4_F, d, a, b, c, MD4_సమితి(5), 7)
    MD4_అడుగు(MD4_F, c, d, a, b, MD4_సమితి(6), 11)
    MD4_అడుగు(MD4_F, b, c, d, a, MD4_సమితి(7), 19)
    MD4_అడుగు(MD4_F, a, b, c, d, MD4_సమితి(8), 3)
    MD4_అడుగు(MD4_F, d, a, b, c, MD4_సమితి(9), 7)
    MD4_అడుగు(MD4_F, c, d, a, b, MD4_సమితి(10), 11)
    MD4_అడుగు(MD4_F, b, c, d, a, MD4_సమితి(11), 19)
    MD4_అడుగు(MD4_F, a, b, c, d, MD4_సమితి(12), 3)
    MD4_అడుగు(MD4_F, d, a, b, c, MD4_సమితి(13), 7)
    MD4_అడుగు(MD4_F, c, d, a, b, MD4_సమితి(14), 11)
    MD4_అడుగు(MD4_F, b, c, d, a, MD4_సమితి(15), 19)

    /* Round 2 */
    MD4_అడుగు(MD4_G, a, b, c, d, MD4_GET(0) + 0x5a827999, 3)
    MD4_అడుగు(MD4_G, d, a, b, c, MD4_GET(4) + 0x5a827999, 5)
    MD4_అడుగు(MD4_G, c, d, a, b, MD4_GET(8) + 0x5a827999, 9)
    MD4_అడుగు(MD4_G, b, c, d, a, MD4_GET(12) + 0x5a827999, 13)
    MD4_అడుగు(MD4_G, a, b, c, d, MD4_GET(1) + 0x5a827999, 3)
    MD4_అడుగు(MD4_G, d, a, b, c, MD4_GET(5) + 0x5a827999, 5)
    MD4_అడుగు(MD4_G, c, d, a, b, MD4_GET(9) + 0x5a827999, 9)
    MD4_అడుగు(MD4_G, b, c, d, a, MD4_GET(13) + 0x5a827999, 13)
    MD4_అడుగు(MD4_G, a, b, c, d, MD4_GET(2) + 0x5a827999, 3)
    MD4_అడుగు(MD4_G, d, a, b, c, MD4_GET(6) + 0x5a827999, 5)
    MD4_అడుగు(MD4_G, c, d, a, b, MD4_GET(10) + 0x5a827999, 9)
    MD4_అడుగు(MD4_G, b, c, d, a, MD4_GET(14) + 0x5a827999, 13)
    MD4_అడుగు(MD4_G, a, b, c, d, MD4_GET(3) + 0x5a827999, 3)
    MD4_అడుగు(MD4_G, d, a, b, c, MD4_GET(7) + 0x5a827999, 5)
    MD4_అడుగు(MD4_G, c, d, a, b, MD4_GET(11) + 0x5a827999, 9)
    MD4_అడుగు(MD4_G, b, c, d, a, MD4_GET(15) + 0x5a827999, 13)

    /* Round 3 */
    MD4_అడుగు(MD4_H, a, b, c, d, MD4_GET(0) + 0x6ed9eba1, 3)
    MD4_అడుగు(MD4_H, d, a, b, c, MD4_GET(8) + 0x6ed9eba1, 9)
    MD4_అడుగు(MD4_H, c, d, a, b, MD4_GET(4) + 0x6ed9eba1, 11)
    MD4_అడుగు(MD4_H, b, c, d, a, MD4_GET(12) + 0x6ed9eba1, 15)
    MD4_అడుగు(MD4_H, a, b, c, d, MD4_GET(2) + 0x6ed9eba1, 3)
    MD4_అడుగు(MD4_H, d, a, b, c, MD4_GET(10) + 0x6ed9eba1, 9)
    MD4_అడుగు(MD4_H, c, d, a, b, MD4_GET(6) + 0x6ed9eba1, 11)
    MD4_అడుగు(MD4_H, b, c, d, a, MD4_GET(14) + 0x6ed9eba1, 15)
    MD4_అడుగు(MD4_H, a, b, c, d, MD4_GET(1) + 0x6ed9eba1, 3)
    MD4_అడుగు(MD4_H, d, a, b, c, MD4_GET(9) + 0x6ed9eba1, 9)
    MD4_అడుగు(MD4_H, c, d, a, b, MD4_GET(5) + 0x6ed9eba1, 11)
    MD4_అడుగు(MD4_H, b, c, d, a, MD4_GET(13) + 0x6ed9eba1, 15)
    MD4_అడుగు(MD4_H, a, b, c, d, MD4_GET(3) + 0x6ed9eba1, 3)
    MD4_అడుగు(MD4_H, d, a, b, c, MD4_GET(11) + 0x6ed9eba1, 9)
    MD4_అడుగు(MD4_H, c, d, a, b, MD4_GET(7) + 0x6ed9eba1, 11)
    MD4_అడుగు(MD4_H, b, c, d, a, MD4_GET(15) + 0x6ed9eba1, 15)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } అయితే(కొలత -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  తిరిగి ptr;
}

స్థిర పూర్ణాంకం my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  తిరిగి 1;
}

స్థిర శూన్య my_md4_update(my_md4_ctx *ctx,
                          స్థిరరాశి శూన్య *ఆదానం, గుర్తులేని దీర్ఘ len)
{
  uint32_t saved_lo;
  గుర్తులేని దీర్ఘ వాడిన;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ఒకవేళ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  వాడిన = saved_lo & 0x3f;

  ఒకవేళ(వాడిన) {
    గుర్తులేని దీర్ఘ అందుబాటు = 64 - వాడిన;

    ఒకవేళ(len < అందుబాటు) {
      జ్ఞాపకం_నకలు(&ctx->బఫరు[వాడిన], ఆదానం, len);
      తిరిగి;
    }

    జ్ఞాపకం_నకలు(&ctx->బఫరు[వాడిన], ఆదానం, అందుబాటు);
    ఆదానం = (స్థిరరాశి గుర్తులేని అక్షరం *)ఆదానం + అందుబాటు;
    len -= అందుబాటు;
    my_md4_body(ctx, ctx->బఫరు, 64);
  }

  ఒకవేళ(len >= 64) {
    ఆదానం = my_md4_body(ctx, ఆదానం, len & ~(గుర్తులేని దీర్ఘ)0x3f);
    len &= 0x3f;
  }

  జ్ఞాపకం_నకలు(ctx->బఫరు, ఆదానం, len);
}

స్థిర శూన్య my_md4_final(గుర్తులేని అక్షరం *digest, my_md4_ctx *ctx)
{
  గుర్తులేని దీర్ఘ వాడిన, అందుబాటు;

  వాడిన = ctx->lo & 0x3f;

  ctx->బఫరు[వాడిన++] = 0x80;

  అందుబాటు = 64 - వాడిన;

  ఒకవేళ(అందుబాటు < 8) {
    memset(&ctx->బఫరు[వాడిన], 0, అందుబాటు);
    my_md4_body(ctx, ctx->బఫరు, 64);
    వాడిన = 0;
    అందుబాటు = 64;
  }

  memset(&ctx->బఫరు[వాడిన], 0, అందుబాటు - 8);

  ctx->lo <<= 3;
  ctx->బఫరు[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->బఫరు[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->బఫరు[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->బఫరు[59] = curlx_ultouc((ctx->lo >> 24) & 0xff);
  ctx->బఫరు[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->బఫరు[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->బఫరు[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->బఫరు[63] = curlx_ultouc(ctx->hi >> 24);

  my_md4_body(ctx, ctx->బఫరు, 64);

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

  memset(ctx, 0, పరిమాణం(*ctx));
}

#అయితే_ముగింపు /* CRYPTO LIBS */

CURLcode Curl_md4it(గుర్తులేని అక్షరం *output,
                    స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, స్థిరరాశి size_t len)
{
  my_md4_ctx ctx;
  ఒకవేళ(!my_md4_init(&ctx))
    తిరిగి CURLE_FAILED_INIT;
  my_md4_update(&ctx, ఆదానం, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  తిరిగి CURLE_OK;
}

#అయితే_ముగింపు /* USE_CURL_NTLM_CORE */
