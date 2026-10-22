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
#समावेश "curl_setup.h"

#अगर (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#समावेश "curl_md5.h"
#समावेश "curl_hmac.h"

#अगर_घोषित USE_OPENSSL
#समावेश <openssl/opensslconf.h>
#पूर्ण_अगर

#अगर_घोषित USE_WOLFSSL
#समावेश <wolfssl/options.h>
#पूर्ण_अगर

#अगर_घोषित USE_MBEDTLS
#समावेश <mbedtls/version.h>
#अगर MBEDTLS_VERSION_संख्या < 0x03020000
#त्रुटि "mbedTLS 3.2.0 or greater required"
#पूर्ण_अगर
#समावेश <psa/crypto_config.h>
#पूर्ण_अगर

#अगर_घोषित USE_GNUTLS
#समावेश <nettle/md5.h>
#समावेश <nettle/version.h>

प्रकार काष्ठा md5_ctx my_md5_ctx;

अटल CURLcode my_md5_init(व्योम *ctx)
{
  md5_init(ctx);
  वापस CURLE_OK;
}

अटल व्योम my_md5_update(व्योम *ctx,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  md5_update(ctx, len, निवेश);
}

अटल व्योम my_md5_final(अचिन्हित अक्षर *digest, व्योम *ctx)
{
#अगर NETTLE_VERSION_MAJOR >= 4
  md5_digest(ctx, digest);
#अन्यथा
  md5_digest(ctx, MD5_DIGEST_LEN, digest);
#पूर्ण_अगर
}

#या_अगर defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD5)
#समावेश <openssl/evp.h>

प्रकार EVP_MD_CTX *my_md5_ctx;

अटल CURLcode my_md5_init(व्योम *in)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)in;
  *ctx = EVP_MD_CTX_new();
  अगर(!*ctx)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(!EVP_DigestInit_ex(*ctx, EVP_md5(), शून्य)) {
    EVP_MD_CTX_free(*ctx);
    *ctx = शून्य;
    वापस CURLE_FAILED_INIT;
  }
  वापस CURLE_OK;
}

अटल व्योम my_md5_update(व्योम *in,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)in;
  (व्योम)EVP_DigestUpdate(*ctx, निवेश, len);
}

अटल व्योम my_md5_final(अचिन्हित अक्षर *digest, व्योम *in)
{
  EVP_MD_CTX **ctx = (EVP_MD_CTX **)in;
  (व्योम)EVP_DigestFinal_ex(*ctx, digest, शून्य);
  EVP_MD_CTX_free(*ctx);
  *ctx = शून्य;
}

#या_अगर defined(USE_WOLFSSL) && !defined(NO_MD5)
#समावेश <wolfssl/wolfcrypt/md5.h>

प्रकार wc_Md5 my_md5_ctx;

अटल CURLcode my_md5_init(व्योम *ctx)
{
  अगर(wc_InitMd5(ctx))
    वापस CURLE_OUT_OF_MEMORY;
  वापस CURLE_OK;
}

अटल व्योम my_md5_update(व्योम *ctx,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  (व्योम)wc_Md5Update(ctx, निवेश, (word32)len);
}

अटल व्योम my_md5_final(अचिन्हित अक्षर *digest, व्योम *ctx)
{
  (व्योम)wc_Md5Final(ctx, digest);
}

#या_अगर defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_MD5) && PSA_WANT_ALG_MD5
#समावेश <psa/crypto.h>

प्रकार psa_hash_operation_t my_md5_ctx;

अटल CURLcode my_md5_init(व्योम *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  अगर(psa_hash_setup(pctx, PSA_ALG_MD5) != PSA_SUCCESS)
    वापस CURLE_OUT_OF_MEMORY;
  वापस CURLE_OK;
}

अटल व्योम my_md5_update(व्योम *ctx,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  (व्योम)psa_hash_update(ctx, निवेश, len);
}

अटल व्योम my_md5_final(अचिन्हित अक्षर *digest, व्योम *ctx)
{
  माप_प्रकार actual_लंबाई;
  (व्योम)psa_hash_finish(ctx, digest, MD5_DIGEST_LEN, &actual_लंबाई);
}

#या_अगर (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#समावेश <CommonCrypto/CommonDigest.h>

/* For Apple operating systems: CommonCrypto has the functions we need.
   These functions are available on Tiger and later, as well as iOS 2.0
   and later. If you are building for an older cat, well, sorry.

   Declaring the functions as static like this seems to be a bit more
   reliable than defining COMMON_DIGEST_FOR_OPENSSL on older cats. */
#  define my_md5_ctx CC_MD5_CTX

अटल CURLcode my_md5_init(व्योम *ctx)
{
  (व्योम)CC_MD5_Init(ctx);  /* always returns 1 */
  वापस CURLE_OK;
}

अटल व्योम my_md5_update(व्योम *ctx,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  CC_MD5_Update(ctx, निवेश, len);
}

अटल व्योम my_md5_final(अचिन्हित अक्षर *digest, व्योम *ctx)
{
  CC_MD5_Final(digest, ctx);
}

#या_अगर defined(USE_WIN32_CRYPTO)
#समावेश <wincrypt.h>

काष्ठा md5_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
प्रकार काष्ठा md5_ctx my_md5_ctx;

अटल CURLcode my_md5_init(व्योम *in)
{
  my_md5_ctx *ctx = (my_md5_ctx *)in;
  अगर(!CryptAcquireContext(&ctx->hCryptProv, शून्य, शून्य, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    वापस CURLE_OUT_OF_MEMORY;

  अगर(!CryptCreateHash(ctx->hCryptProv, CALG_MD5, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    वापस CURLE_FAILED_INIT;
  }

  वापस CURLE_OK;
}

अटल व्योम my_md5_update(व्योम *in,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  my_md5_ctx *ctx = in;
  Crypt_Hash_तथ्य(ctx->hHash, (स्थिर BYTE *)निवेश, len, 0);
}

अटल व्योम my_md5_final(अचिन्हित अक्षर *digest, व्योम *in)
{
  my_md5_ctx *ctx = (my_md5_ctx *)in;
  अचिन्हित दीर्घ लंबाई = 0;
  CryptGetHashParam(ctx->hHash, HP_HASHVAL, शून्य, &लंबाई, 0);
  अगर(लंबाई == MD5_DIGEST_LEN)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &लंबाई, 0);
  अगर(ctx->hHash)
    CryptDestroyHash(ctx->hHash);
  अगर(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#अन्यथा

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

काष्ठा md5_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  अचिन्हित अक्षर बफर[64];
  uint32_t block[16];
};
प्रकार काष्ठा md5_ctx my_md5_ctx;

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, like in Colin Plumb's
 * implementation.
 */
#घोषणा MD5_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#घोषणा MD5_G(x, y, z)                  ((y) ^ ((z) & ((x) ^ (y))))
#घोषणा MD5_H(x, y, z)                  (((x) ^ (y)) ^ (z))
#घोषणा MD5_H2(x, y, z)                 ((x) ^ ((y) ^ (z)))
#घोषणा MD5_I(x, y, z)                  ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#घोषणा MD5_STEP(f, a, b, c, d, x, t, s) \
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
#अगर defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#घोषणा MD5_SET(n) (*(स्थिर uint32_t *)(स्थिर व्योम *)&ptr[(n) * 4])
#घोषणा MD5_GET(n) MD5_SET(n)
#अन्यथा
#घोषणा MD5_SET(n) (ctx->block[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#घोषणा MD5_GET(n) ctx->block[n]
#पूर्ण_अगर

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
अटल स्थिर व्योम *my_md5_body(my_md5_ctx *ctx,
                               स्थिर व्योम *तथ्य, अचिन्हित दीर्घ आकार)
{
  स्थिर अचिन्हित अक्षर *ptr;
  uint32_t a, b, c, d;

  ptr = (स्थिर अचिन्हित अक्षर *)तथ्य;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  करो {
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
  } जबतक(आकार -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  वापस ptr;
}

अटल CURLcode my_md5_init(व्योम *in)
{
  my_md5_ctx *ctx = (my_md5_ctx *)in;
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  वापस CURLE_OK;
}

अटल व्योम my_md5_update(व्योम *in,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  uint32_t saved_lo;
  अचिन्हित पूर्णांक used;
  my_md5_ctx *ctx = (my_md5_ctx *)in;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  अगर(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  अगर(used) {
    अचिन्हित पूर्णांक available = 64 - used;

    अगर(len < available) {
      स_नकल(&ctx->बफर[used], निवेश, len);
      वापस;
    }

    स_नकल(&ctx->बफर[used], निवेश, available);
    निवेश = (स्थिर अचिन्हित अक्षर *)निवेश + available;
    len -= available;
    my_md5_body(ctx, ctx->बफर, 64);
  }

  अगर(len >= 64) {
    निवेश = my_md5_body(ctx, निवेश, len & ~(अचिन्हित दीर्घ)0x3f);
    len &= 0x3f;
  }

  स_नकल(ctx->बफर, निवेश, len);
}

अटल व्योम my_md5_final(अचिन्हित अक्षर *digest, व्योम *in)
{
  अचिन्हित पूर्णांक used, available;
  my_md5_ctx *ctx = (my_md5_ctx *)in;

  used = ctx->lo & 0x3f;

  ctx->बफर[used++] = 0x80;

  available = 64 - used;

  अगर(available < 8) {
    स_रखो(&ctx->बफर[used], 0, available);
    my_md5_body(ctx, ctx->बफर, 64);
    used = 0;
    available = 64;
  }

  स_रखो(&ctx->बफर[used], 0, available - 8);

  ctx->lo <<= 3;
  ctx->बफर[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->बफर[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->बफर[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->बफर[59] = curlx_ultouc(ctx->lo >> 24);
  ctx->बफर[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->बफर[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->बफर[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->बफर[63] = curlx_ultouc(ctx->hi >> 24);

  my_md5_body(ctx, ctx->बफर, 64);

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

  स_रखो(ctx, 0, माप(*ctx));
}

#पूर्ण_अगर /* CRYPTO LIBS */

स्थिर काष्ठा HMAC_params Curl_HMAC_MD5 = {
  my_md5_init,        /* Hash initialization function. */
  my_md5_update,      /* Hash update function. */
  my_md5_final,       /* Hash computation end function. */
  माप(my_md5_ctx), /* Size of hash context structure. */
  64,                 /* Maximum key length. */
  MD5_DIGEST_LEN      /* Result size. */
};

स्थिर काष्ठा MD5_params Curl_DIGEST_MD5 = {
  my_md5_init,        /* Digest initialization function */
  my_md5_update,      /* Digest update function */
  my_md5_final,       /* Digest computation end function */
  माप(my_md5_ctx), /* Size of digest context struct */
  MD5_DIGEST_LEN      /* Result size */
};

/*
 * @unittest: 1601
 * Returns CURLE_OK on success.
 */
CURLcode Curl_md5it(अचिन्हित अक्षर *output,
                    स्थिर अचिन्हित अक्षर *निवेश, माप_प्रकार len)
{
  CURLcode परिणाम;
  my_md5_ctx ctx;

  परिणाम = my_md5_init(&ctx);
  अगर(!परिणाम) {
    करो {
      अचिन्हित पूर्णांक ilen = (अचिन्हित पूर्णांक)CURLMIN(len, अच_पूर्णांक_उच्च);
      my_md5_update(&ctx, निवेश, ilen);
      निवेश += ilen;
      len -= ilen;
    } जबतक(len);
    my_md5_final(output, &ctx);
  }
  वापस परिणाम;
}

काष्ठा MD5_context *Curl_MD5_init(स्थिर काष्ठा MD5_params *md5params)
{
  काष्ठा MD5_context *ctxt;

  /* Create MD5 context */
  ctxt = curlx_malloc(माप(*ctxt));

  अगर(!ctxt)
    वापस ctxt;

  ctxt->md5_hashctx = curlx_malloc(md5params->md5_ctxtsize);

  अगर(!ctxt->md5_hashctx) {
    curlx_free(ctxt);
    वापस शून्य;
  }

  ctxt->md5_hash = md5params;

  अगर((*md5params->md5_init_func)(ctxt->md5_hashctx)) {
    curlx_free(ctxt->md5_hashctx);
    curlx_free(ctxt);
    वापस शून्य;
  }

  वापस ctxt;
}

CURLcode Curl_MD5_update(काष्ठा MD5_context *context,
                         स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  (*context->md5_hash->md5_update_func)(context->md5_hashctx, निवेश, len);

  वापस CURLE_OK;
}

CURLcode Curl_MD5_final(काष्ठा MD5_context *context, अचिन्हित अक्षर *परिणाम)
{
  (*context->md5_hash->md5_final_func)(परिणाम, context->md5_hashctx);

  curlx_free(context->md5_hashctx);
  curlx_free(context);

  वापस CURLE_OK;
}

#पूर्ण_अगर /* Using NTLM (without SSPI) || Digest */
