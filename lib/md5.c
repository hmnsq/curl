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

#ఒకవేళ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#అన్తర్భావ "curl_md5.h"
#అన్తర్భావ "curl_hmac.h"

#నిర్వచితమైతే USE_OPENSSL
#అన్తర్భావ <openssl/opensslconf.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WOLFSSL
#అన్తర్భావ <wolfssl/options.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_MBEDTLS
#అన్తర్భావ <mbedtls/version.h>
#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x03020000
#దోషం "mbedTLS 3.2.0 or greater required"
#అయితే_ముగింపు
#అన్తర్భావ <psa/crypto_config.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_GNUTLS
#అన్తర్భావ <nettle/md5.h>
#అన్తర్భావ <nettle/version.h>

రకనిర్వచనం నిర్మాణం md5_ctx my_md5_ctx;

స్థిర CURLcode my_md5_init(శూన్య *ctx)
{
  md5_init(ctx);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_md5_update(శూన్య *ctx,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  md5_update(ctx, len, ఆదానం);
}

స్థిర శూన్య my_md5_final(గుర్తులేని అక్షరం *digest, శూన్య *ctx)
{
#ఒకవేళ NETTLE_సంస్కరణ_MAJOR >= 4
  md5_digest(ctx, digest);
#లేకపోతే
  md5_digest(ctx, MD5_DIGEST_LEN, digest);
#అయితే_ముగింపు
}

#లేదావేళ defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD5)
#అన్తర్భావ <openssl/evp.h>

రకనిర్వచనం EVP_MD_CTX *my_md5_ctx;

స్థిర CURLcode my_md5_init(శూన్య *లోపల)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)లోపల;
  *ctx = EVP_MD_CTX_new();
  ఒకవేళ(!*ctx)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!EVP_DigestInit_ex(*ctx, EVP_md5(), ఖాళీ)) {
    EVP_MD_CTX_free(*ctx);
    *ctx = ఖాళీ;
    తిరిగి CURLE_FAILED_INIT;
  }
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_md5_update(శూన్య *లోపల,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)లోపల;
  (శూన్య)EVP_DigestUpdate(*ctx, ఆదానం, len);
}

స్థిర శూన్య my_md5_final(గుర్తులేని అక్షరం *digest, శూన్య *లోపల)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)లోపల;
  (శూన్య)EVP_DigestFinal_ex(*ctx, digest, ఖాళీ);
  EVP_MD_CTX_free(*ctx);
  *ctx = ఖాళీ;
}

#లేదావేళ defined(USE_WOLFSSL) && !defined(NO_MD5)
#అన్తర్భావ <wolfssl/wolfcrypt/md5.h>

రకనిర్వచనం wc_Md5 my_md5_ctx;

స్థిర CURLcode my_md5_init(శూన్య *ctx)
{
  ఒకవేళ(wc_InitMd5(ctx))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_md5_update(శూన్య *ctx,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  (శూన్య)wc_Md5Update(ctx, ఆదానం, (word32)len);
}

స్థిర శూన్య my_md5_final(గుర్తులేని అక్షరం *digest, శూన్య *ctx)
{
  (శూన్య)wc_Md5Final(ctx, digest);
}

#లేదావేళ defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_MD5) && PSA_WANT_ALG_MD5
#అన్తర్భావ <psa/crypto.h>

రకనిర్వచనం psa_hash_operation_t my_md5_ctx;

స్థిర CURLcode my_md5_init(శూన్య *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  ఒకవేళ(psa_hash_అమరిక(pctx, PSA_ALG_MD5) != PSA_SUCCESS)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_md5_update(శూన్య *ctx,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  (శూన్య)psa_hash_update(ctx, ఆదానం, len);
}

స్థిర శూన్య my_md5_final(గుర్తులేని అక్షరం *digest, శూన్య *ctx)
{
  size_t actual_నిడివి;
  (శూన్య)psa_hash_ముగింపు(ctx, digest, MD5_DIGEST_LEN, &actual_నిడివి);
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

/* For Apple operating systems: CommonCrypto has the functions we need.
   These functions are available on Tiger and later, as well as iOS 2.0
   and later. If you are building for an older cat, well, sorry.

   Declaring the functions as static like this seems to be a bit more
   reliable than defining COMMON_DIGEST_FOR_OPENSSL on older cats. */
#  define my_md5_ctx CC_MD5_CTX

స్థిర CURLcode my_md5_init(శూన్య *ctx)
{
  (శూన్య)CC_MD5_Init(ctx);  /* always returns 1 */
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_md5_update(శూన్య *ctx,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  CC_MD5_Update(ctx, ఆదానం, len);
}

స్థిర శూన్య my_md5_final(గుర్తులేని అక్షరం *digest, శూన్య *ctx)
{
  CC_MD5_Final(digest, ctx);
}

#లేదావేళ defined(USE_WIN32_CRYPTO)
#అన్తర్భావ <wincrypt.h>

నిర్మాణం md5_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
రకనిర్వచనం నిర్మాణం md5_ctx my_md5_ctx;

స్థిర CURLcode my_md5_init(శూన్య *లోపల)
{
  my_md5_ctx *ctx = (my_md5_ctx *)లోపల;
  ఒకవేళ(!CryptAcquireContext(&ctx->hCryptProv, ఖాళీ, ఖాళీ, PROV_RSA_నిండిన,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!CryptCreateHash(ctx->hCryptProv, CALG_MD5, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    తిరిగి CURLE_FAILED_INIT;
  }

  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_md5_update(శూన్య *లోపల,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  my_md5_ctx *ctx = లోపల;
  Crypt_Hash_దత్తాంశం(ctx->hHash, (స్థిరరాశి BYTE *)ఆదానం, len, 0);
}

స్థిర శూన్య my_md5_final(గుర్తులేని అక్షరం *digest, శూన్య *లోపల)
{
  my_md5_ctx *ctx = (my_md5_ctx *)లోపల;
  గుర్తులేని దీర్ఘ నిడివి = 0;
  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ఖాళీ, &నిడివి, 0);
  ఒకవేళ(నిడివి == MD5_DIGEST_LEN)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &నిడివి, 0);
  ఒకవేళ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);
  ఒకవేళ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#లేకపోతే

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

నిర్మాణం md5_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  గుర్తులేని అక్షరం బఫరు[64];
  uint32_t ఖండం[16];
};
రకనిర్వచనం నిర్మాణం md5_ctx my_md5_ctx;

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, like in Colin Plumb's
 * implementation.
 */
#నిర్వచించు MD5_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#నిర్వచించు MD5_G(x, y, z)                  ((y) ^ ((z) & ((x) ^ (y))))
#నిర్వచించు MD5_H(x, y, z)                  (((x) ^ (y)) ^ (z))
#నిర్వచించు MD5_H2(x, y, z)                 ((x) ^ ((y) ^ (z)))
#నిర్వచించు MD5_I(x, y, z)                  ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#నిర్వచించు MD5_అడుగు(f, a, b, c, d, x, t, s) \
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
#ఒకవేళ defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#నిర్వచించు MD5_సమితి(n) (*(స్థిరరాశి uint32_t *)(స్థిరరాశి శూన్య *)&ptr[(n) * 4])
#నిర్వచించు MD5_GET(n) MD5_సమితి(n)
#లేకపోతే
#నిర్వచించు MD5_సమితి(n) (ctx->ఖండం[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#నిర్వచించు MD5_GET(n) ctx->ఖండం[n]
#అయితే_ముగింపు

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
స్థిర స్థిరరాశి శూన్య *my_md5_body(my_md5_ctx *ctx,
                               స్థిరరాశి శూన్య *దత్తాంశం, గుర్తులేని దీర్ఘ కొలత)
{
  స్థిరరాశి గుర్తులేని అక్షరం *ptr;
  uint32_t a, b, c, d;

  ptr = (స్థిరరాశి గుర్తులేని అక్షరం *)దత్తాంశం;

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
    MD5_అడుగు(MD5_F, a, b, c, d, MD5_సమితి(0), 0xd76aa478, 7)
    MD5_అడుగు(MD5_F, d, a, b, c, MD5_సమితి(1), 0xe8c7b756, 12)
    MD5_అడుగు(MD5_F, c, d, a, b, MD5_సమితి(2), 0x242070db, 17)
    MD5_అడుగు(MD5_F, b, c, d, a, MD5_సమితి(3), 0xc1bdceee, 22)
    MD5_అడుగు(MD5_F, a, b, c, d, MD5_సమితి(4), 0xf57c0faf, 7)
    MD5_అడుగు(MD5_F, d, a, b, c, MD5_సమితి(5), 0x4787c62a, 12)
    MD5_అడుగు(MD5_F, c, d, a, b, MD5_సమితి(6), 0xa8304613, 17)
    MD5_అడుగు(MD5_F, b, c, d, a, MD5_సమితి(7), 0xfd469501, 22)
    MD5_అడుగు(MD5_F, a, b, c, d, MD5_సమితి(8), 0x698098d8, 7)
    MD5_అడుగు(MD5_F, d, a, b, c, MD5_సమితి(9), 0x8b44f7af, 12)
    MD5_అడుగు(MD5_F, c, d, a, b, MD5_సమితి(10), 0xffff5bb1, 17)
    MD5_అడుగు(MD5_F, b, c, d, a, MD5_సమితి(11), 0x895cd7be, 22)
    MD5_అడుగు(MD5_F, a, b, c, d, MD5_సమితి(12), 0x6b901122, 7)
    MD5_అడుగు(MD5_F, d, a, b, c, MD5_సమితి(13), 0xfd987193, 12)
    MD5_అడుగు(MD5_F, c, d, a, b, MD5_సమితి(14), 0xa679438e, 17)
    MD5_అడుగు(MD5_F, b, c, d, a, MD5_సమితి(15), 0x49b40821, 22)

    /* Round 2 */
    MD5_అడుగు(MD5_G, a, b, c, d, MD5_GET(1), 0xf61e2562, 5)
    MD5_అడుగు(MD5_G, d, a, b, c, MD5_GET(6), 0xc040b340, 9)
    MD5_అడుగు(MD5_G, c, d, a, b, MD5_GET(11), 0x265e5a51, 14)
    MD5_అడుగు(MD5_G, b, c, d, a, MD5_GET(0), 0xe9b6c7aa, 20)
    MD5_అడుగు(MD5_G, a, b, c, d, MD5_GET(5), 0xd62f105d, 5)
    MD5_అడుగు(MD5_G, d, a, b, c, MD5_GET(10), 0x02441453, 9)
    MD5_అడుగు(MD5_G, c, d, a, b, MD5_GET(15), 0xd8a1e681, 14)
    MD5_అడుగు(MD5_G, b, c, d, a, MD5_GET(4), 0xe7d3fbc8, 20)
    MD5_అడుగు(MD5_G, a, b, c, d, MD5_GET(9), 0x21e1cde6, 5)
    MD5_అడుగు(MD5_G, d, a, b, c, MD5_GET(14), 0xc33707d6, 9)
    MD5_అడుగు(MD5_G, c, d, a, b, MD5_GET(3), 0xf4d50d87, 14)
    MD5_అడుగు(MD5_G, b, c, d, a, MD5_GET(8), 0x455a14ed, 20)
    MD5_అడుగు(MD5_G, a, b, c, d, MD5_GET(13), 0xa9e3e905, 5)
    MD5_అడుగు(MD5_G, d, a, b, c, MD5_GET(2), 0xfcefa3f8, 9)
    MD5_అడుగు(MD5_G, c, d, a, b, MD5_GET(7), 0x676f02d9, 14)
    MD5_అడుగు(MD5_G, b, c, d, a, MD5_GET(12), 0x8d2a4c8a, 20)

    /* Round 3 */
    MD5_అడుగు(MD5_H, a, b, c, d, MD5_GET(5), 0xfffa3942, 4)
    MD5_అడుగు(MD5_H2, d, a, b, c, MD5_GET(8), 0x8771f681, 11)
    MD5_అడుగు(MD5_H, c, d, a, b, MD5_GET(11), 0x6d9d6122, 16)
    MD5_అడుగు(MD5_H2, b, c, d, a, MD5_GET(14), 0xfde5380c, 23)
    MD5_అడుగు(MD5_H, a, b, c, d, MD5_GET(1), 0xa4beea44, 4)
    MD5_అడుగు(MD5_H2, d, a, b, c, MD5_GET(4), 0x4bdecfa9, 11)
    MD5_అడుగు(MD5_H, c, d, a, b, MD5_GET(7), 0xf6bb4b60, 16)
    MD5_అడుగు(MD5_H2, b, c, d, a, MD5_GET(10), 0xbebfbc70, 23)
    MD5_అడుగు(MD5_H, a, b, c, d, MD5_GET(13), 0x289b7ec6, 4)
    MD5_అడుగు(MD5_H2, d, a, b, c, MD5_GET(0), 0xeaa127fa, 11)
    MD5_అడుగు(MD5_H, c, d, a, b, MD5_GET(3), 0xd4ef3085, 16)
    MD5_అడుగు(MD5_H2, b, c, d, a, MD5_GET(6), 0x04881d05, 23)
    MD5_అడుగు(MD5_H, a, b, c, d, MD5_GET(9), 0xd9d4d039, 4)
    MD5_అడుగు(MD5_H2, d, a, b, c, MD5_GET(12), 0xe6db99e5, 11)
    MD5_అడుగు(MD5_H, c, d, a, b, MD5_GET(15), 0x1fa27cf8, 16)
    MD5_అడుగు(MD5_H2, b, c, d, a, MD5_GET(2), 0xc4ac5665, 23)

    /* Round 4 */
    MD5_అడుగు(MD5_I, a, b, c, d, MD5_GET(0), 0xf4292244, 6)
    MD5_అడుగు(MD5_I, d, a, b, c, MD5_GET(7), 0x432aff97, 10)
    MD5_అడుగు(MD5_I, c, d, a, b, MD5_GET(14), 0xab9423a7, 15)
    MD5_అడుగు(MD5_I, b, c, d, a, MD5_GET(5), 0xfc93a039, 21)
    MD5_అడుగు(MD5_I, a, b, c, d, MD5_GET(12), 0x655b59c3, 6)
    MD5_అడుగు(MD5_I, d, a, b, c, MD5_GET(3), 0x8f0ccc92, 10)
    MD5_అడుగు(MD5_I, c, d, a, b, MD5_GET(10), 0xffeff47d, 15)
    MD5_అడుగు(MD5_I, b, c, d, a, MD5_GET(1), 0x85845dd1, 21)
    MD5_అడుగు(MD5_I, a, b, c, d, MD5_GET(8), 0x6fa87e4f, 6)
    MD5_అడుగు(MD5_I, d, a, b, c, MD5_GET(15), 0xfe2ce6e0, 10)
    MD5_అడుగు(MD5_I, c, d, a, b, MD5_GET(6), 0xa3014314, 15)
    MD5_అడుగు(MD5_I, b, c, d, a, MD5_GET(13), 0x4e0811a1, 21)
    MD5_అడుగు(MD5_I, a, b, c, d, MD5_GET(4), 0xf7537e82, 6)
    MD5_అడుగు(MD5_I, d, a, b, c, MD5_GET(11), 0xbd3af235, 10)
    MD5_అడుగు(MD5_I, c, d, a, b, MD5_GET(2), 0x2ad7d2bb, 15)
    MD5_అడుగు(MD5_I, b, c, d, a, MD5_GET(9), 0xeb86d391, 21)

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

స్థిర CURLcode my_md5_init(శూన్య *లోపల)
{
  my_md5_ctx *ctx = (my_md5_ctx *)లోపల;
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_md5_update(శూన్య *లోపల,
                          స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  uint32_t saved_lo;
  గుర్తులేని పూర్ణాంకం వాడిన;
  my_md5_ctx *ctx = (my_md5_ctx *)లోపల;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  ఒకవేళ(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  వాడిన = saved_lo & 0x3f;

  ఒకవేళ(వాడిన) {
    గుర్తులేని పూర్ణాంకం అందుబాటు = 64 - వాడిన;

    ఒకవేళ(len < అందుబాటు) {
      జ్ఞాపకం_నకలు(&ctx->బఫరు[వాడిన], ఆదానం, len);
      తిరిగి;
    }

    జ్ఞాపకం_నకలు(&ctx->బఫరు[వాడిన], ఆదానం, అందుబాటు);
    ఆదానం = (స్థిరరాశి గుర్తులేని అక్షరం *)ఆదానం + అందుబాటు;
    len -= అందుబాటు;
    my_md5_body(ctx, ctx->బఫరు, 64);
  }

  ఒకవేళ(len >= 64) {
    ఆదానం = my_md5_body(ctx, ఆదానం, len & ~(గుర్తులేని దీర్ఘ)0x3f);
    len &= 0x3f;
  }

  జ్ఞాపకం_నకలు(ctx->బఫరు, ఆదానం, len);
}

స్థిర శూన్య my_md5_final(గుర్తులేని అక్షరం *digest, శూన్య *లోపల)
{
  గుర్తులేని పూర్ణాంకం వాడిన, అందుబాటు;
  my_md5_ctx *ctx = (my_md5_ctx *)లోపల;

  వాడిన = ctx->lo & 0x3f;

  ctx->బఫరు[వాడిన++] = 0x80;

  అందుబాటు = 64 - వాడిన;

  ఒకవేళ(అందుబాటు < 8) {
    memset(&ctx->బఫరు[వాడిన], 0, అందుబాటు);
    my_md5_body(ctx, ctx->బఫరు, 64);
    వాడిన = 0;
    అందుబాటు = 64;
  }

  memset(&ctx->బఫరు[వాడిన], 0, అందుబాటు - 8);

  ctx->lo <<= 3;
  ctx->బఫరు[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->బఫరు[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->బఫరు[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->బఫరు[59] = curlx_ultouc(ctx->lo >> 24);
  ctx->బఫరు[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->బఫరు[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->బఫరు[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->బఫరు[63] = curlx_ultouc(ctx->hi >> 24);

  my_md5_body(ctx, ctx->బఫరు, 64);

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

స్థిరరాశి నిర్మాణం HMAC_params Curl_HMAC_MD5 = {
  my_md5_init,        /* Hash initialization function. */
  my_md5_update,      /* Hash update function. */
  my_md5_final,       /* Hash computation end function. */
  పరిమాణం(my_md5_ctx), /* Size of hash context structure. */
  64,                 /* Maximum key length. */
  MD5_DIGEST_LEN      /* Result size. */
};

స్థిరరాశి నిర్మాణం MD5_params Curl_DIGEST_MD5 = {
  my_md5_init,        /* Digest initialization function */
  my_md5_update,      /* Digest update function */
  my_md5_final,       /* Digest computation end function */
  పరిమాణం(my_md5_ctx), /* Size of digest context struct */
  MD5_DIGEST_LEN      /* Result size */
};

/*
 * @unittest: 1601
 * Returns CURLE_OK on success.
 */
CURLcode Curl_md5it(గుర్తులేని అక్షరం *output,
                    స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, size_t len)
{
  CURLcode ఫలితం;
  my_md5_ctx ctx;

  ఫలితం = my_md5_init(&ctx);
  ఒకవేళ(!ఫలితం) {
    చేయి {
      గుర్తులేని పూర్ణాంకం ilen = (గుర్తులేని పూర్ణాంకం)CURLMIN(len, UINT_MAX);
      my_md5_update(&ctx, ఆదానం, ilen);
      ఆదానం += ilen;
      len -= ilen;
    } అయితే(len);
    my_md5_final(output, &ctx);
  }
  తిరిగి ఫలితం;
}

నిర్మాణం MD5_context *Curl_MD5_init(స్థిరరాశి నిర్మాణం MD5_params *md5params)
{
  నిర్మాణం MD5_context *ctxt;

  /* Create MD5 context */
  ctxt = curlx_malloc(పరిమాణం(*ctxt));

  ఒకవేళ(!ctxt)
    తిరిగి ctxt;

  ctxt->md5_hashctx = curlx_malloc(md5params->md5_ctxtsize);

  ఒకవేళ(!ctxt->md5_hashctx) {
    curlx_free(ctxt);
    తిరిగి ఖాళీ;
  }

  ctxt->md5_hash = md5params;

  ఒకవేళ((*md5params->md5_init_func)(ctxt->md5_hashctx)) {
    curlx_free(ctxt->md5_hashctx);
    curlx_free(ctxt);
    తిరిగి ఖాళీ;
  }

  తిరిగి ctxt;
}

CURLcode Curl_MD5_update(నిర్మాణం MD5_context *context,
                         స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం, గుర్తులేని పూర్ణాంకం len)
{
  (*context->md5_hash->md5_update_func)(context->md5_hashctx, ఆదానం, len);

  తిరిగి CURLE_OK;
}

CURLcode Curl_MD5_final(నిర్మాణం MD5_context *context, గుర్తులేని అక్షరం *ఫలితం)
{
  (*context->md5_hash->md5_final_func)(ఫలితం, context->md5_hashctx);

  curlx_free(context->md5_hashctx);
  curlx_free(context);

  తిరిగి CURLE_OK;
}

#అయితే_ముగింపు /* Using NTLM (without SSPI) || Digest */
